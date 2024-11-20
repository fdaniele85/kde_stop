#include "ProbabilisticStop.h"

#include <cctype>
#include <stdexcept>
#include <algorithm>

namespace ffp {
    bool icase_compare(std::string_view s1, std::string_view s2) {
        auto comparator = [](char a, char b) { return std::tolower(a) == std::tolower(b); };

        return (s1.size() == s2.size()) &&
               std::equal(s1.begin(), s1.end(), s2.begin(), comparator);
    }

    ProbabilisticStop::ProbabilisticStop(double threshold, double improve_pct, Kernel kernel_type, double lb,
                                         int iterations, int number_of_queries)
            : threshold_(threshold),
              improve_pct_(improve_pct),
              estimator_type_(kernel_type),
              iterations_(iterations),
              number_of_queries_(number_of_queries),
              estimator_(estimator_type_, lb) {}

    bool ProbabilisticStop::stop() {
        std::lock_guard<std::mutex> _(mtx_);
        if (!fed_) {
            return false;
        }

        double target_val = min_ - (improve_pct_ * min_);
        return estimate(target_val) < threshold_;
    }

    double ProbabilisticStop::estimate(double num_value) const { return estimator_.cdf(num_value, number_of_queries_); }

    void ProbabilisticStop::add(double cost) {
        std::lock_guard<std::mutex> _(mtx_);

        data_.push_back(cost);
        min_ = std::min(min_, cost);

        if (data_.size() % iterations_ == 0) {
            estimator_.feed_data(data_);
            fed_ = true;
        }
    }

    ProbabilisticStop::Kernel ProbabilisticStop::get_kernel(std::string_view kernel) {
        if (icase_compare(kernel, "gaussian")) {
            return Kernel::gaussian;
        }

        if (icase_compare(kernel, "epanechnikov")) {
            return Kernel::epanechnikov;
        }

        if (icase_compare(kernel, "uniform")) {
            return Kernel::uniform;
        }

        throw std::invalid_argument("Kernel not defined");
    }
} // namespace ffp