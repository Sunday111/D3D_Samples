#pragma once

#include <algorithm>
#include <cmath>
#include <functional>
#include <vector>

template<typename T>
inline constexpr T pi = static_cast<T>(3.141592653589793238462643383279L);

template<typename T, typename Function>
T ComputeInteral(T min, T max, size_t precision, Function&& function) {
    T result{};
    T delta = (max - min) / precision;
    for (T arg = min; arg < max; arg += delta)
        result += function(arg) * delta;
    return result;
}

template<typename T>
T ComputeFourierSeriesCoefficientA0(size_t precision, const std::function<T(T)>& function) {
    return ComputeInteral(-pi<T>, pi<T>, precision, function) / pi<T>;
}

template<typename T, typename Function>
T ComputeFourierSeriesCoefficientAN(size_t n, size_t precision, Function&& function) {
    auto integral = ComputeInteral(-pi<T>, pi<T>, precision, [&](T arg) {
        return function(arg) * std::cos(n * arg);
    });

    return integral / pi<T>;
}

template<typename T, typename Function>
T ComputeFourierSeriesCoefficientBN(size_t n, size_t precision, Function&& function) {
    auto integral = ComputeInteral(-pi<T>, pi<T>, precision, [&](T arg) {
        return function(arg) * std::sin(n * arg);
    });

    return integral / pi<T>;
}

template<typename T, typename OutIt>
void ComputeFourierSeriesCoefficientsA(size_t count, size_t precision, OutIt output, const std::function<T(T)>& function)
{
    std::generate_n(output, count, [&, i = 1]() mutable {
        return ComputeFourierSeriesCoefficientAN<T>(i++, precision, function);
    });
}

template<typename T, typename OutIt>
void ComputeFourierSeriesCoefficientsB(size_t count, size_t precision, OutIt output, const std::function<T(T)>& function)
{
    std::generate_n(output, count, [&, i = 1]() mutable {
        return ComputeFourierSeriesCoefficientBN<T>(i++, precision, function);
    });
}

template<typename T>
std::function<T(T)> RestoreFromFourierCoefficients(size_t coefficients_count, std::vector<T> an, std::vector<T> bn, T a0) {
    return [coefficients_count, an = std::move(an), bn = std::move(bn), a0](T argument)
    {
        auto result = a0 / 2;
        for (size_t i = 0; i < coefficients_count; ++i) {
            const auto narg = (i + 1) * argument;
            result +=
                an[i] * std::cos(narg) +
                bn[i] * std::sin(narg);
        }
        return result;
    };
}
