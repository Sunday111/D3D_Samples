#pragma once

#include <algorithm>
#include <functional>
#include <vector>

#include "EverydayTools/Geom/Vector.h"

template<typename T, typename Function>
std::function<T(T)> MakeSampledFunction(T startArg, T range, size_t samples_count, Function&& function)
{
    const auto delta = range / samples_count;
    std::vector<T> samples;

    for (size_t sample_index = 0; sample_index < samples_count; ++sample_index) {
        const auto arg = startArg + delta * sample_index;
        samples.push_back(function(arg));
    }

    return[samples = std::move(samples), startArg, delta](T argument) {
        // Interpolate here?
        auto sample_index = static_cast<size_t>((argument - startArg) / delta);
        return samples[std::min(sample_index, samples.size() - 1)];
    };
}
