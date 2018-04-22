#ifdef USE_KENG_MEMORY
#include "Keng/Memory/Memory.h"
USE_KENG_MEMORY_MANAGER
#endif

#include "Keng/Core/ISystem.h"
#include "EverydayTools/UnusedVar.h"
#include "EverydayTools/Exception/CallAndRethrow.h"
#include "EverydayTools/Exception/ThrowIfFailed.h"

#include <ctime>
#include <iomanip>
#include <chrono>
#include <vector>
#include <stdlib.h>
#include <fstream>

using namespace keng;
using namespace core;

class System : public RefCountImpl<ISystem>
{
public:
    virtual void Initialize(const keng::core::IApplicationPtr&) override {
        srand(105);
    }

    virtual bool Update() override {
        return CallAndRethrowM + [&] {
            auto t0 = std::chrono::high_resolution_clock::now();

            for (int frame = 0; frame < 50000; ++frame)
            {
                for (int i = 0; i < 100; ++i) {
                    int size = rand() % 64;
                    std::vector<uint8_t> arr;
                    arr.reserve(size);
                    for (int j = 0; j < size; ++j) {
                        arr.push_back((uint8_t)rand());
                    }
                    m_arrs.push_back(std::move(arr));
                }
            
                while (!m_arrs.empty()) {
                    auto eraseIt = m_arrs.begin() + (rand() % m_arrs.size());
                    auto& arr = *eraseIt;
                    while (!arr.empty()) {
                        auto it = arr.begin() + (rand() % arr.size());
                        arr.erase(it);
                    }
                    m_arrs.erase(eraseIt);
                }
            }
            
            auto t1 = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(t1 - t0).count();
            std::ofstream stream("Benchmark.txt", std::ios_base::app | std::ios_base::out);

            auto now = std::chrono::system_clock::now();
            auto in_time_t = std::chrono::system_clock::to_time_t(now);

            stream << std::put_time(std::localtime(&in_time_t), "%Y-%m-%d %X") << std::endl;
            stream << "    Duration: " << duration << "ms" << std::endl;
            stream << std::endl;

            return false;
        };
    }

    virtual void Shutdown() override {

    }

    virtual std::string_view GetSystemName() const override {
        return "MemoryProfiling";
    }

    virtual bool ForEachDependency(const edt::Delegate<bool(std::string_view)>& delegate) const override {
        UnusedVar(delegate);
        return false;
    }

private:
    std::vector<std::vector<uint8_t>> m_arrs;
};

extern "C"
{
    void __declspec(dllexport) __cdecl CreateSystem(void** result) {
        *result = new System();
    }
}
