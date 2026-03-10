#include <per4m/ProbabilisticStop.h>

#include <algorithm>
#include <cctype>

namespace per4m {
    ProbabilisticStop::ProbabilisticStop(const double threshold, const double improve_pct, const Kernel kernel_type, const double lb, const int iterations,
                                         const int number_of_queries, const BandwidthType bandwidth_type)
        : threshold_(threshold),
          improve_pct_(improve_pct),
          estimator_type_(kernel_type),
          iterations_(iterations),
          estimator_(estimator_type_, iterations, number_of_queries, bandwidth_type, lb),
          data_(iterations) {}

    bool ProbabilisticStop::stop() {
        detail::LockGuard _(mtx_);

        if (!fed_) {
            return false;
        }

        const double target_val = min_ - (improve_pct_ * min_);
        return estimator_.estimate(target_val) < threshold_;
    }

    void ProbabilisticStop::add(double cost) {
        detail::LockGuard _(mtx_);

        data_.add(cost);
        min_ = std::min(min_, cost);
        if (data_.full()) {
            estimator_.feed_data(data_);
            fed_ = true;
        }
    }
} // namespace per4m