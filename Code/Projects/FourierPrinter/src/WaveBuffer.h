#pragma once

#include <functional>
#include <memory>
#include <ostream>
#include <istream>
#include <string_view>

#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/Math/Clamp.h"

class WaveBuffer
{
public:
    WaveBuffer(WaveBuffer&&);
    WaveBuffer(const WaveBuffer&) = delete;
    WaveBuffer(const char* filename);
    WaveBuffer(std::istream& input);
    WaveBuffer(
        size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
        const void* data, size_t dataSize);
    WaveBuffer(
        size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
        std::unique_ptr<uint8_t[]>&& data, size_t dataSize);

    void SaveToFile(std::string_view filename) const;
    void SaveToFile(std::ostream& output) const;

    size_t GetChannelsCount() const;
    size_t GetBitsPerSample() const;
    size_t GetSampleRate() const;
    edt::DenseArrayView<const uint8_t> GetData() const;

    WaveBuffer& operator=(WaveBuffer&&);
    WaveBuffer& operator=(const WaveBuffer&) = delete;

private:
    size_t m_channelsCount;
    size_t m_bitsPerSample;
    size_t m_sampleRate;
    size_t m_dataSize;
    std::unique_ptr<uint8_t[]> m_data;
};

template<typename T, typename Sample>
WaveBuffer MakeWaveBuffer(size_t channelsCount, size_t sampleRate, T argumentBegin, T argumentRange, size_t samplesCount, const std::function<T(T)>& function) {
    std::unique_ptr<uint8_t[]> data;
    const size_t sampleSize = sizeof(Sample);
    const size_t bytesCount = sampleSize * samplesCount;
    data.reset(new uint8_t[bytesCount]);
    edt::DenseArrayView<Sample> samplesView(reinterpret_cast<Sample*>(data.get()), samplesCount);
    const float deltaArgument = argumentRange / samplesCount;
    for (size_t sampleIndex = 0; sampleIndex < samplesCount; ++sampleIndex) {
        auto argument = argumentBegin + sampleIndex * deltaArgument;
        auto value = function(argument);
        auto sample = edt::math::ClampToAnotherTypeBounds<Sample>(value);
        Sample casted = static_cast<Sample>(sample);
        samplesView[sampleIndex] = casted;
    }
    return WaveBuffer(1, sampleSize * 8, sampleRate, std::move(data), bytesCount);
}

template<typename T>
WaveBuffer MakeWaveBuffer(size_t channelsCount, size_t sampleRate, edt::DenseArrayView<const T> data) {
    return WaveBuffer(channelsCount, sizeof(T) * 8, sampleRate, data.GetData(), data.GetSize() * sizeof(T));
}