#pragma once

#include <vector>
#include <string>
#include <stdexcept>

namespace emd {

template <typename T, typename IDX_T>
class cost_matrix {
public:

    cost_matrix(T* data, IDX_T size) :
            data_{data},
            data_size_{size} {}

    void resize(std::size_t size) {
        const auto extended_data_size{size - data_size_};
        extended_data_.resize(extended_data_size);
    }

    [[nodiscard]] T& operator[](IDX_T index) {
        if (index < data_size_) {
            // This is the cost on an actual arc
            return data_[index];
        }
        // This is the cost of an artificial arc
        return extended_data_[index];
    }

private:
    T* data_{};
    IDX_T data_size_{};

    // The Network Simplex algorithm makes use of costs on artificial arcs.
    // These artificial costs do not fit into the data_ array and are instead stored in the vector below.
    std::vector<T> extended_data_{};
};

}
