#include <iostream>
#include <vector>

#include "lib/compress.hpp"
#include "lib/fixquat.hpp"
#include "lib/quant.hpp"

int main() {
    std::vector<quat::quat> quats;

    quat::quat q{};
    for (int i = 0; i < 1000; ++i) {
        quats.push_back(q);
        q = q * quat::quat(
                    quat::vec(quat::base_type{0.01}, quat::base_type{0.005}, quat::base_type{0.0}));
    }

    uint8_t data[8192];
    int8_t scratch[8192];

    quant::State state{};
    auto res = compress::compress_block(state, quats.data(), quats.size(), 14, data, sizeof(data),
                                        scratch, sizeof(scratch));

    std::cout << "success:   " << res.success << std::endl;
    std::cout << "bytes_put: " << res.bytes_put << std::endl;

    return 0;
}