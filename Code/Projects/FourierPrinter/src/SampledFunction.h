#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "EverydayTools/Geom/Vector.h"

template<bool interpolate, typename T,  typename Function>
std::function<T(T)> MakeSampledFunction(T startArg, T range, size_t samples_count, Function&& function)
{
    const auto delta = range / samples_count;
    std::vector<T> samples;

    for (size_t sample_index = 0; sample_index < samples_count; ++sample_index) {
        const auto arg = startArg + delta * sample_index;
        samples.push_back(function(arg));
    }

    if constexpr (interpolate) {
        return[samples = std::move(samples), startArg, delta](T argument) {
            auto sample_index = (argument - startArg) / delta;
            sample_index = std::min<T>(sample_index, samples.size() - 1);
            auto left_index = std::floor(sample_index);
            auto right_index = std::min<T>(sample_index, samples.size() - 1);
            auto left_sample = samples[left_index];
            auto right_sample = samples[right_index];
            auto delta_sample = right_sample - left_sample;
            auto delta_index = sample_index - left_index;
            auto sample = left_sample + delta_sample * delta_index;
            return sample;
        };
    }
    else {
        return[samples = std::move(samples), startArg, delta](T argument) {
            auto sample_index = (argument - startArg) / delta;
            sample_index = std::min<T>(sample_index, samples.size() - 1);
            return samples[sample_index];
        }
    }
}
