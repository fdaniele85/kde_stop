//
// Created by Tommaso on 02/04/2024.
//

#pragma once

#include <algorithm>
#include <mutex>
#include <vector>
#include "KDE.h"

namespace ffp {
    class ProbabilisticFilter {
    public:
        ProbabilisticFilter(double threshold, Kernel kernel_type, int iterations, int number_of_queries);

        [[nodiscard]] bool perform_ls(double target_gap);
        void add(double gap);

    private:
        double threshold_;
        Kernel estimator_type_;
        int iterations_;
        int number_of_queries_;

        [[nodiscard]] double estimate(double num_value) const;

        KDE estimator_;

        std::vector<double> data_;
        std::mutex mtx_;
        bool fed_{false};
    };
}