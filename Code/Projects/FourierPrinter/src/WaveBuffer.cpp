#include <fstream>
#include <vector>

#include "WaveBuffer.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"
#include "EverydayTools/Exception/CheckedCast.h"

namespace
{
	struct WavFile
	{
		struct
		{
			char chunkId[4];
			uint32_t chunkSize;
			char format[4];
		} riff;

		struct
		{
			char subchunk1Id[4];
			uint32_t subchunk1Size;
			uint16_t audioFormat;
			uint16_t numChannels;
			uint32_t sampleRate;
			uint32_t byteRate;
			uint16_t blockAlign;
			uint16_t bitsPerSample;
		} format;

		struct
		{
			char subchunk2Id[4];
			uint32_t subchunk2Size;
			std::unique_ptr<uint8_t[]> data;
		} data;
	};

	WavFile ReadWavFile(std::istream& file) {
        return CallAndRethrowM + [&] {
            WavFile result;

            auto invalidFileFormatMessage = "Invalid file format";

            file.read(reinterpret_cast<char*>(&result.riff), sizeof(result.riff));
            edt::ThrowIfFailed(strncmp(result.riff.chunkId, "RIFF", 4) == 0, invalidFileFormatMessage);
            edt::ThrowIfFailed(strncmp(result.riff.format, "WAVE", 4) == 0, invalidFileFormatMessage);

            file.read(reinterpret_cast<char*>(&result.format), sizeof(result.format));
            edt::ThrowIfFailed(strncmp(result.format.subchunk1Id, "fmt ", 4) == 0, invalidFileFormatMessage);
            //edt::ThrowIfFailed(result.format.audioFormat == 1, invalidFileFormatMessage);

            file.read(reinterpret_cast<char*>(&result.data.subchunk2Id), sizeof(result.data.subchunk2Id));
            edt::ThrowIfFailed(strncmp(result.data.subchunk2Id, "data", 4) == 0, invalidFileFormatMessage);

            file.read(reinterpret_cast<char*>(&result.data.subchunk2Size), sizeof(result.data.subchunk2Size));

            result.data.data.reset(new uint8_t[result.data.subchunk2Size]);
            file.read(reinterpret_cast<char*>(result.data.data.get()), result.data.subchunk2Size);

            return result;
        };
    }
}

WaveBuffer::WaveBuffer(const char* filename) {
    CallAndRethrowM + [&] {
        std::ifstream file(filename, std::ios::binary);
        edt::ThrowIfFailed(file.is_open(), "Could not create the file");
        auto fileData = ReadWavFile(file);
        m_channelsCount = fileData.format.numChannels;
        m_bitsPerSample = fileData.format.bitsPerSample;
        m_sampleRate = fileData.format.sampleRate;
        m_dataSize = fileData.data.subchunk2Size;
        m_data = std::move(fileData.data.data);
    };
}
WaveBuffer::WaveBuffer(std::istream& input) {
    CallAndRethrowM + [&] {
        auto fileData = ReadWavFile(input);
    };
}

WaveBuffer::WaveBuffer(size_t channelsCount, size_t bitsPerSample, size_t sampleRate, void* data, size_t dataSize) :
	m_channelsCount(channelsCount),
	m_bitsPerSample(bitsPerSample),
	m_sampleRate(sampleRate),
	m_dataSize(dataSize)
{
    CallAndRethrowM + [&] {
        m_data.reset(new uint8_t[m_dataSize]);
        std::memcpy(m_data.get(), data, m_dataSize);
    };
}

WaveBuffer::WaveBuffer(
	size_t channelsCount, size_t bitsPerSample, size_t sampleRate,
	std::unique_ptr<uint8_t[]>&& data, size_t dataSize) :
	m_channelsCount(channelsCount),
	m_bitsPerSample(bitsPerSample),
	m_sampleRate(sampleRate),
	m_dataSize(dataSize),
	m_data(std::move(data))
{}

void WaveBuffer::SaveToFile(std::string_view filename) const
{
    CallAndRethrowM + [&] {
        std::ofstream file(filename.data(), std::ios::binary | std::ios::out | std::ios::trunc);
        edt::ThrowIfFailed(file.is_open(), "Could not create the file");
        SaveToFile(file);
    };
}

void WaveBuffer::SaveToFile(std::ostream& output) const {
    CallAndRethrowM + [&] {
        WavFile wavData;

        // RIFF
        std::memcpy(&wavData.riff.chunkId, "RIFF", sizeof(wavData.riff.chunkId));
        wavData.riff.chunkSize = edt::CheckedCast<uint32_t>(36 + m_dataSize);
        std::memcpy(&wavData.riff.format, "WAVE", sizeof(wavData.riff.format));

        output.write(
            reinterpret_cast<const char*>(&wavData.riff),
            sizeof(wavData.riff));

        // FORMAT
        size_t sampleBytesCount = m_bitsPerSample / 8;
        std::memcpy(&wavData.format.subchunk1Id, "fmt ", sizeof(wavData.format.subchunk1Id));
        wavData.format.subchunk1Size = 16;
        wavData.format.audioFormat = 1;
        wavData.format.numChannels = edt::CheckedCast<uint16_t>(m_channelsCount);
        wavData.format.sampleRate = edt::CheckedCast<uint32_t>(m_sampleRate);
        wavData.format.byteRate = edt::CheckedCast<uint32_t>(m_sampleRate * sampleBytesCount);
        wavData.format.blockAlign = edt::CheckedCast<uint16_t>(m_channelsCount * sampleBytesCount);
        wavData.format.bitsPerSample = edt::CheckedCast<uint16_t>(m_bitsPerSample);

        output.write(
            reinterpret_cast<const char*>(&wavData.format),
            sizeof(wavData.format));

        // Data
        output.write("data", 4);

        auto dataSize = static_cast<uint32_t>(m_dataSize);
        output.write(
            reinterpret_cast<const char*>(&dataSize),
            sizeof(dataSize));
        output.write(
            reinterpret_cast<const char*>(m_data.get()),
            m_dataSize);
    };
}
size_t WaveBuffer::GetChannelsCount() const {
    return m_channelsCount;
}

size_t WaveBuffer::GetBitsPerSample() const {
    return m_bitsPerSample;
}

size_t WaveBuffer::GetSampleRate() const {
    return m_sampleRate;
}

edt::DenseArrayView<const uint8_t> WaveBuffer::GetData() const {
    return edt::DenseArrayView<const uint8_t>(m_data.get(), m_dataSize);
}

WaveBuffer::WaveBuffer(WaveBuffer&&) = default;
WaveBuffer& WaveBuffer::operator=(WaveBuffer&&) = default;