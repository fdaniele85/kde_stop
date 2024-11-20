//
// Created by Tommaso on 02/04/2024.
//

#pragma once

#include <algorithm>
#include <mutex>
#include <vector>

namespace ffp {
    class ProbabilisticStop {
    public:
        enum class Kernel { gaussian, epanechnikov, uniform };

        static Kernel get_kernel(std::string_view kernel);

        /// Constructor
        ///
        /// \param threshold and \param improve_pct  regulate the stopping of the probabilistic stop. If
        /// the probability of improving at least improve_pct is lower than threshold, the stoppping criterion is satisfied.
        /// \param kernel_type specifies the type of kernel to be used in the KDE
        /// \param lb lower bound
        /// \param iterations the probability is updated every iterations elements
        /// \param number_of_queries
        ProbabilisticStop(double threshold, double improve_pct, Kernel kernel_type, double lb, int iterations, int number_of_queries);

        [[nodiscard]] bool stop();
        void add(double cost);

    private:
        class KDE {
        public:
            explicit KDE(ProbabilisticStop::Kernel kernel_type, double lb = 0);

            void feed_data(std::vector<double> &data);

            [[nodiscard]] std::vector<double> kernelDensityEstimate(std::vector<double> &x_values, std::vector<double> data) const;
            [[nodiscard]] double cdf(double x_value, int n_queries) const;

        private:
            double bandwidth_{0.5};
            ProbabilisticStop::Kernel kernel_type_{ProbabilisticStop::Kernel::epanechnikov};

            // TODO: Usare std::queue per mantenere solo gli ultimi n_sample valori e stimare sempre su quelli?
            std::vector<double> data_;
            double lb_;

            std::function<double(double, double)> kernel_function_;
        };

        double threshold_;
        double improve_pct_;
        Kernel estimator_type_;
        int iterations_;
        int number_of_queries_;

        [[nodiscard]] double estimate(double num_value) const;

        KDE estimator_;

        double min_{std::numeric_limits<double>::max()};

        std::vector<double> data_;
        std::mutex mtx_;
        bool fed_{false};
    };
}