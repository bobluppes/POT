/* This file is a c++ wrapper function for computing the transportation cost
 * between two vectors given a cost matrix.
 *
 * It was written by Antoine Rolet (2014) and mainly consists of a wrapper
 * of the code written by Nicolas Bonneel available on this page
 *          http://people.seas.harvard.edu/~nbonneel/FastTransport/
 *
 * It was then modified to make it more amenable to python inline calling
 *
 * Please give relevant credit to the original author (Nicolas Bonneel) if
 * you use this code for a publication.
 *
 */


#include "network_simplex_simple.h"
#include "network_simplex_simple_omp.h"
#include "EMD.h"
#include <cstdint>


int EMD_wrap(int supply_count, int demand_count, double *supply_weights,
             double *demand_weights, double *distance_matrix,
             double *flow_matrix, double* alpha, double* beta, double *cost,
             uint64_t maxIter)  {
    // beware M and C are stored in row major C style!!!

    using namespace lemon;
    uint64_t retained_supply_nodes, retained_demand_nodes, cur;

    typedef FullBipartiteDigraph Digraph;
    DIGRAPH_TYPEDEFS(Digraph);

    // Get the number of non zero coordinates for r and c
    retained_supply_nodes =0;
    for (int i=0; i< supply_count; i++) {
        double val=*(supply_weights +i);
        if (val>0) {
          retained_supply_nodes++;
        }else if(val<0){
			return INFEASIBLE;
		}
    }
    retained_demand_nodes =0;
    for (int i=0; i< demand_count; i++) {
        double val=*(demand_weights +i);
        if (val>0) {
          retained_demand_nodes++;
        }else if(val<0){
			return INFEASIBLE;
		}
    }

    // Define the graph

    std::vector<uint64_t> indI(retained_supply_nodes), indJ(retained_demand_nodes);
    std::vector<double> weights1(retained_supply_nodes), weights2(retained_demand_nodes);
    Digraph di(retained_supply_nodes, retained_demand_nodes);
    NetworkSimplexSimple<Digraph,double,double, node_id_type> net(di, true, (int) (retained_supply_nodes + retained_demand_nodes), retained_supply_nodes * retained_demand_nodes, maxIter);

    // Set supply and demand, don't account for 0 values (faster)

    cur=0;
    for (uint64_t i=0; i< supply_count; i++) {
        double val=*(supply_weights +i);
        if (val>0) {
            weights1[ cur ] = val;
            indI[cur++]=i;
        }
    }

    // Demand is actually negative supply...

    cur=0;
    for (uint64_t i=0; i< demand_count; i++) {
        double val=*(demand_weights +i);
        if (val>0) {
            weights2[ cur ] = -val;
            indJ[cur++]=i;
        }
    }


    net.supplyMap(&weights1[0], (int)retained_supply_nodes, &weights2[0], (int)retained_demand_nodes);

    // Set the cost of each edge
    int64_t idarc = 0;
    for (uint64_t i=0; i< retained_supply_nodes; i++) {
        for (uint64_t j=0; j< retained_demand_nodes; j++) {
            double val=*(distance_matrix +indI[i]* demand_count +indJ[j]);
            net.setCost(di.arcFromId(idarc), val);
            ++idarc;
        }
    }


    // Solve the problem with the network simplex algorithm

    int ret=net.run();
    uint64_t i, j;
    if (ret==(int)net.OPTIMAL || ret==(int)net.MAX_ITER_REACHED) {
        *cost = 0;
        Arc a; di.first(a);
        for (; a != INVALID; di.next(a)) {
            i = di.source(a);
            j = di.target(a);
            double flow = net.flow(a);
            *cost += flow * (*(distance_matrix +indI[i]* demand_count +indJ[j- retained_supply_nodes]));
            *(flow_matrix +indI[i]* demand_count +indJ[j- retained_supply_nodes]) = flow;
            *(alpha + indI[i]) = -net.potential(i);
            *(beta + indJ[j- retained_supply_nodes]) = net.potential(j);
        }

    }


    return ret;
}







int EMD_wrap_omp(int n1, int n2, double *X, double *Y, double *D, double *G,
             double* alpha, double* beta, double *cost, uint64_t maxIter, int numThreads)  {
    // beware M and C are stored in row major C style!!!

    using namespace lemon_omp;
    uint64_t n, m, cur;

    typedef FullBipartiteDigraph Digraph;
    DIGRAPH_TYPEDEFS(Digraph);

    // Get the number of non zero coordinates for r and c
    n=0;
    for (int i=0; i<n1; i++) {
        double val=*(X+i);
        if (val>0) {
            n++;
        }else if(val<0){
            return INFEASIBLE;
        }
    }
    m=0;
    for (int i=0; i<n2; i++) {
        double val=*(Y+i);
        if (val>0) {
            m++;
        }else if(val<0){
            return INFEASIBLE;
        }
    }

    // Define the graph

    std::vector<uint64_t> indI(n), indJ(m);
    std::vector<double> weights1(n), weights2(m);
    Digraph di(n, m);
    NetworkSimplexSimple<Digraph,double,double, node_id_type> net(di, true, (int) (n + m), n * m, maxIter, numThreads);

    // Set supply and demand, don't account for 0 values (faster)

    cur=0;
    for (uint64_t i=0; i<n1; i++) {
        double val=*(X+i);
        if (val>0) {
            weights1[ cur ] = val;
            indI[cur++]=i;
        }
    }

    // Demand is actually negative supply...

    cur=0;
    for (uint64_t i=0; i<n2; i++) {
        double val=*(Y+i);
        if (val>0) {
            weights2[ cur ] = -val;
            indJ[cur++]=i;
        }
    }


    net.supplyMap(&weights1[0], (int) n, &weights2[0], (int) m);

    // Set the cost of each edge
    int64_t idarc = 0;
    for (uint64_t i=0; i<n; i++) {
        for (uint64_t j=0; j<m; j++) {
            double val=*(D+indI[i]*n2+indJ[j]);
            net.setCost(di.arcFromId(idarc), val);
            ++idarc;
        }
    }


    // Solve the problem with the network simplex algorithm

    int ret=net.run();
    uint64_t i, j;
    if (ret==(int)net.OPTIMAL || ret==(int)net.MAX_ITER_REACHED) {
        *cost = 0;
        Arc a; di.first(a);
        for (; a != INVALID; di.next(a)) {
            i = di.source(a);
            j = di.target(a);
            double flow = net.flow(a);
            *cost += flow * (*(D+indI[i]*n2+indJ[j-n]));
            *(G+indI[i]*n2+indJ[j-n]) = flow;
            *(alpha + indI[i]) = -net.potential(i);
            *(beta + indJ[j-n]) = net.potential(j);
        }

    }


    return ret;
}
