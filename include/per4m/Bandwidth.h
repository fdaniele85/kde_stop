#pragma once
#include <vector>
#include <optional>

namespace per4m {

    struct Bandwidth {
        static double scott_1d(const std::vector<double>& x);
        static double silverman_1d(const std::vector<double>& x);

        // ISJ (Improved Sheather-Jones) per 1D, fedele a KDEpy.
        // weights opzionali: se presenti, pesi <= 0 vengono rimossi come in KDEpy.
        static double isj_1d(const std::vector<double>& x,
                             const std::optional<std::vector<double>>& weights = std::nullopt);
    };

} // namespace per4m