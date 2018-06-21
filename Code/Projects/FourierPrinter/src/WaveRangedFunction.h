#pragma once


#include <memory>
#include <type_traits>

#include "EverydayTools/Array/ArrayView.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

#include "RangedFunction.h"
#include "WaveBuffer.h"

template
<
    typename T, bool interpolate,
    typename = std::enable_if_t<std::is_floating_point_v<T>>
>
class WaveRangedFunction : public RangedFunction<T>
{
private:
    using Converter = T(*)(const uint8_t& sampleRef);
    
public:
    WaveRangedFunction(T argumentMin, T argumentMax, std::shared_ptr<const WaveBuffer> waveBuffer) :
        m_waveBuffer(waveBuffer),
        m_minimalArgument(argumentMin),
        m_argumentRange(argumentMax - argumentMin)
    {
        CallAndRethrowM + [&] {
            edt::ThrowIfFailed(m_waveBuffer != nullptr, "Wave buffer is null");
            edt::ThrowIfFailed(m_argumentRange > 0, "Invalid argument range");
            const auto bytesView = m_waveBuffer->GetData();
            const auto sampleSize = waveBuffer->GetBitsPerSample() / 8;
            edt::ThrowIfFailed((bytesView.GetSize() % sampleSize) == 0, "Invalid wave buffer");
            const auto samplesCount = bytesView.GetSize() / sampleSize;
            edt::ThrowIfFailed(samplesCount > 0, "Sound buffer is empty");
            m_samples = edt::SparseArrayView<const uint8_t>(bytesView.GetData(), samplesCount, sampleSize);

            switch (sampleSize)
            {
            case 1:
                m_converter = GenericConverter<int8_t>;
                break;
            case 2:
                m_converter = GenericConverter<int16_t>;
                break;
            case 4:
                m_converter = GenericConverter<int32_t>;
                break;
            default:
                edt::ThrowIfFailed(false, "Unexpected sample size: ", sampleSize * 8, " bit");
            }
        };
    }
    
    virtual T Compute(T argument) const override {
        return CallAndRethrowM + [&] {
            const T deltaArgument = argument - m_minimalArgument;
            edt::ThrowIfFailed((argument >= m_minimalArgument) && (deltaArgument <= m_argumentRange), "Invalid argument");
            const T parameter = deltaArgument / m_argumentRange;
            const size_t samplesCount = m_samples.GetSize();
            const size_t maxIndex = samplesCount - 1;
            const T floatIndex = samplesCount * parameter;
            const size_t index = std::min(static_cast<size_t>(floatIndex), maxIndex);
            const T sample = m_converter(m_samples[index]);
            if constexpr (!interpolate) {
                return static_cast<T>(sample);
            }
            const size_t nextIndex = std::min(index + 1, maxIndex);
            const T nextSample = m_converter(m_samples[nextIndex]);
            const T deltaSample = nextSample - sample;
            const T deltaIndex = floatIndex - std::floor(floatIndex);
            const T interpolatedSample = sample + deltaIndex * deltaSample;
            return static_cast<T>(interpolatedSample);
        };
    }
    
    virtual T GetMinArgument() const override {
        return m_minimalArgument;
    }
    
    virtual T GetMaxArgument() const override {
        return m_minimalArgument + m_argumentRange;
    }
    
    T operator()(T argument) const {
        return Compute(argument);
    }

    size_t GetSamplesCount() const
    {
        return m_samples.GetSize();
    }
    
private:
    template<typename Sample>
    static T GenericConverter(const uint8_t& sample) {
        return static_cast<T>(reinterpret_cast<const Sample&>(sample));
    }

private:
    T m_minimalArgument;
    T m_argumentRange;
    edt::SparseArrayView<const uint8_t> m_samples;
    std::shared_ptr<const WaveBuffer> m_waveBuffer;
    Converter m_converter = nullptr;
};
