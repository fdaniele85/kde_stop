#include "ProbabilisticFilter.h"

namespace ffp {
    ProbabilisticFilter::ProbabilisticFilter(const double threshold, const Kernel kernel_type, const int iterations, const int number_of_queries)
        : threshold_(threshold), estimator_type_(kernel_type), iterations_(iterations), number_of_queries_(number_of_queries), estimator_(estimator_type_, 0) {}

    void ProbabilisticFilter::add(const double gap) {
        std::lock_guard<std::mutex> _(mtx_);

        data_.push_back(gap);

        if (data_.size() % iterations_ == 0) {
            estimator_.feed_data(data_);
            fed_ = true;
        }
    }

    bool ProbabilisticFilter::perform_ls(const double target_gap) {
        std::lock_guard<std::mutex> _(mtx_);
        if (!fed_) {
            return true;
        }

        return (1 - estimate(target_gap)) >= threshold_;
    }

    double ProbabilisticFilter::estimate(const double num_value) const {return estimator_.cdf(num_value, number_of_queries_);}
} // namespace ffp