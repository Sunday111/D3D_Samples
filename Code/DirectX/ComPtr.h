#pragma once

template<typename T>
class ComPtr {
public:
    ComPtr(T* p = nullptr) :
        m_p(p)
    {}

    ComPtr(const ComPtr&) = delete;

    ComPtr(ComPtr&& that) {
        MoveFrom<false>(std::move(that));
    }

    ~ComPtr() {
        Release<false>();
    }

    T* Get() const {
        return m_p;
    }

    template<bool release_prev = true>
    void Set(T* ptr) {
        if constexpr (release_prev) {
            Release<false>();
        }
        m_p = ptr;
    }

    template<bool set_to_null>
    void Release() {
        if (m_p != nullptr) {
            m_p->Release();
            if constexpr (set_to_null) {
                m_p = nullptr;
            }
        }
    }

    template<bool release_prev = true>
    void MoveFrom(ComPtr<T>&& that) {
        Set<release_prev>(that.m_p);
        that.m_p = nullptr;
    }

    T* operator->() const {
        return m_p;
    }

    T& operator*() const {
        return *m_p;
    }

    ComPtr& operator=(const ComPtr&) = delete;

    ComPtr& operator=(ComPtr&& that) {
        MoveFrom<true>(std::move(that));
        return *this;
    }

    T** Receive() {
        Release<true>();
        return &m_p;
    }

private:
    T * m_p;
};