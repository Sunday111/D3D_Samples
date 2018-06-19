#pragma once

#include <algorithm>
#include <functional>
#include <vector>

template<typename T, typename Function>
std::function<T(T)> MakeSampledFunction(size_t samples_count, T min, T max, Function&& function)
{
    const auto phase = min;
    const auto period = max - min;
    const auto delta = period / samples_count;

    std::vector<double> samples;
    for (size_t sample_index = 0; sample_index < samples_count; ++sample_index) {
        const auto arg = phase + delta * sample_index;
        samples.push_back(function(arg));
    }

    return[samples = std::move(samples), phase, delta](T argument) {
        // Interpolate here?
        auto sample_index = static_cast<size_t>((argument - phase) / delta);
        return samples[std::min(sample_index, samples.size() - 1)];
    };
}
