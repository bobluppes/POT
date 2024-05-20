#pragma once

#include <vector>
#include <string>
#include <stdexcept>

namespace emd {

template <typename T, typename IDX_T>
class cost_matrix {
    using index_mapping_t = std::vector<IDX_T>;
public:

    cost_matrix(T* retained_data, IDX_T n, IDX_T m, IDX_T retained_n, IDX_T retained_m,
                const index_mapping_t& retained_n_to_n_mapping, const index_mapping_t& retained_m_to_m_mapping) :
            retained_data_{retained_data},
            retained_data_size_{retained_n * retained_m},
            n_{n},
            m_{m},
            retained_n_{retained_n},
            retained_m_{retained_m},
            retained_n_to_n_mapping_{retained_n_to_n_mapping},
            retained_m_to_m_mapping_{retained_m_to_m_mapping} {}

    void resize(std::size_t size) {
        if (size <= retained_data_size_) {
            return;
        }

        const auto extended_data_size{size - retained_data_size_};
        extended_data_.resize(extended_data_size);
    }

    [[nodiscard]] IDX_T size() const {
        return retained_data_size_ + extended_data_.size();
    }

    [[nodiscard]] T& operator[](IDX_T index) {
        if (index < retained_data_size_) {
            // This is the cost on an actual arc
            return retained_data_[translate_retained_index(index)];
        }

        // This is the cost of an artificial arc
        return extended_data_[index];
    }

private:
    [[nodiscard]] IDX_T translate_retained_index(IDX_T retained_index) const {
        const auto retained_n{retained_index / retained_m_};
        const auto retained_m{retained_index % retained_m_};

        return retained_n_to_n_mapping_.at(retained_n) * m_ + retained_m_to_m_mapping_.at(retained_m);
    }

    T* retained_data_{};
    IDX_T retained_data_size_{};

    // The Network Simplex algorithm makes use of costs on artificial arcs.
    // These artificial costs do not fit into the retained_data_ array and are instead stored in the vector below.
    std::vector<T> extended_data_{};

    IDX_T n_{};
    IDX_T m_{};
    IDX_T retained_n_{};
    IDX_T retained_m_{};

    const index_mapping_t& retained_n_to_n_mapping_{};
    const index_mapping_t& retained_m_to_m_mapping_{};
};

}
