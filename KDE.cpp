//
// Created by Tommaso on 27/03/2024.
//

#include "ProbabilisticStop.h"
#include <cmath>
#include <numeric>

namespace ffp {
    double gaussianKernel(double x, double bandwidth) { return exp(-0.5 * pow(x, 2) / pow(bandwidth, 2)) / (sqrt(2 * M_PI) * bandwidth); }

    double epanechnikovKernel(double x, double bandwidth) {
        auto base = x / bandwidth;
        if(std::abs(x) <= bandwidth) {
            return (3.0 / 4.0) * (1 - pow(base, 2));
        } else {
            return 0;
        }
    }

    double uniformKernel(double x, double bandwidth) { return (0.5 / bandwidth) * (std::abs(x) <= bandwidth); }

    ProbabilisticStop::KDE::KDE(ProbabilisticStop::Kernel kernel_type, double lb) : kernel_type_(kernel_type), lb_(lb) {
        switch(kernel_type_) {
        case ProbabilisticStop::Kernel::gaussian: kernel_function_ = &gaussianKernel; break;
        case ProbabilisticStop::Kernel::epanechnikov: kernel_function_ = &epanechnikovKernel; break;
        case ProbabilisticStop::Kernel::uniform: kernel_function_ = &uniformKernel; break;
        }
    }

    std::vector<double> ProbabilisticStop::KDE::kernelDensityEstimate(std::vector<double> &x_values, std::vector<double> data) const {
        auto n = data.size();
        std::vector<double> result(x_values.size(), 0.0);
        std::vector<double> kernel_values(x_values.size(), 0.0);

        for(uint i = 0; i < n; ++i) {
            for(uint j = 0; j < x_values.size(); ++j) {
                kernel_values[j] = kernel_function_(x_values[j] - data[i], bandwidth_);

                if(kernel_values[j] > 0) {
                    result[j] += kernel_values[j] / (static_cast<double>(n) * bandwidth_);
                }
            }
        }
        return result;
    }

    void ProbabilisticStop::KDE::feed_data(std::vector<double> &data) {
        data_ = data;

        double sigma = 0.0;
        auto n = static_cast<double>(data_.size());
        auto accumulation = std::accumulate(data_.begin(), data_.end(), 0.0);
        for(double val : data_) {
            sigma += pow(val - (accumulation / n), 2);
        }
        sigma = sqrt(sigma / n);
        bandwidth_ = pow((4 * pow(sigma, 5)) / (3 * n), 0.2);
    }

    double ProbabilisticStop::KDE::cdf(double x_value, int n_queries) const {
        std::vector<double> x_values;

        double current_val = lb_;
        double delta_val = (x_value - lb_) / n_queries;

        for(int i = 0; i < n_queries; ++i) {
            x_values.push_back(current_val);
            current_val += delta_val;
        }

        // TODO implementare per la massimizzazione

        std::vector<double> pdf_values = kernelDensityEstimate(x_values, data_);

        double cdf_value = 0.0;
        for(uint i = 1; i < pdf_values.size(); ++i) {
            cdf_value += 0.5 * (pdf_values[i] + pdf_values[i - 1]) * (x_values[i] - x_values[i - 1]);
        }
        return cdf_value;
    }
}