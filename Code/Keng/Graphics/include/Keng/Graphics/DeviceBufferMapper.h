#include "Keng/Graphics/IDeviceBuffer.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

namespace keng::graphics
{
    // Uses RAII to guarantee safe map
    class DeviceBufferMapper
    {
    public:
        DeviceBufferMapper() = default;

        DeviceBufferMapper(core::Ptr<IDeviceBuffer> buffer) :
            m_buffer(buffer)
        {
            m_view = buffer->Map();
        }

        DeviceBufferMapper(const DeviceBufferMapper&) = delete;

        DeviceBufferMapper(DeviceBufferMapper&& that) {
            MoveFrom(that);
        }

        ~DeviceBufferMapper() {
            if (m_buffer) {
                m_buffer->Unmap();
            }
        }

        edt::DenseArrayView<uint8_t> GetView() const {
            return CallAndRethrowM + [&] {
                CheckForMoved();
                return m_view;
            };
        }

        template<typename T>
        edt::DenseArrayView<T> GetTypedView() const {
            return CallAndRethrowM + [&] {
                auto byteView = GetView();
                auto bytesCount = byteView.GetSize();
                edt::ThrowIfFailed(
                    (bytesCount % sizeof(T) != 0),
                    "Looks like type mismatch!");
                return edt::DenseArrayView<T>(byteView.GetData(), bytesCount / sizeof(T));
            };
        }

        DeviceBufferMapper& operator=(const DeviceBufferMapper& that) = delete;

        DeviceBufferMapper& operator=(DeviceBufferMapper&& that) {
            MoveFrom(that);
            return *this;
        }

    private:
        void CheckForMoved() const {
            edt::ThrowIfFailed(m_buffer != nullptr, "Trying to use moved mapper");
        }

        void MoveFrom(DeviceBufferMapper& that) {
            m_buffer = that.m_buffer;
            m_view = that.m_view;
            that.m_buffer = nullptr;
            that.m_view = edt::DenseArrayView<uint8_t>();
        }

    private:
        core::Ptr<IDeviceBuffer> m_buffer;
        edt::DenseArrayView<uint8_t> m_view;
    };
}