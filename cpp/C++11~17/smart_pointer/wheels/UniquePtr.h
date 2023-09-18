#ifndef __UNIQUE_PTR__
#define __UNIQUE_PTR__

#include <tuple>
#include <cstddef>

// namespace Ephmeral {

template<typename T>
struct default_delete {
    constexpr default_delete() noexcept = default;
    default_delete(const default_delete &) noexcept {}

    void operator()(T* ptr) const {
        delete ptr;
    }
};

template<typename T, typename D>
class unique_ptr_impl {
    using pointer = T*;
public:
    unique_ptr_impl() = default;
    unique_ptr_impl(pointer p) : m_t() { ptr() = p; }
    template<typename Del>
    unique_ptr_impl(pointer p, Del&& d)
	: m_t(p, std::forward<Del>(d)) { }

    pointer&   ptr() { return std::get<0>(m_t); }
    pointer    ptr() const { return std::get<0>(m_t); }
    D&       deleter() { return std::get<1>(m_t); }
    const D& deleter() const { return std::get<1>(m_t); }

    void swap(unique_ptr_impl &rhs) noexcept {
        using std::swap;
        swap(this->ptr(), rhs.ptr());
        swap(this->deleter(), rhs.deleter());
    }
private:
    std::tuple<pointer, D> m_t;
};

template <typename T, typename Del = default_delete<T>>
class unique_ptr {
    unique_ptr_impl<T, Del> m_t;
    using pointer = T*;
    using deleter_type = Del;
public:
    constexpr unique_ptr() noexcept 
        : m_t() {}
    unique_ptr(pointer p) noexcept : m_t(p) {}
    unique_ptr(pointer p, const deleter_type& d) noexcept 
        : m_t(p, d) {}

    unique_ptr(pointer p, deleter_type &&d) 
        : m_t(p, std::move(d)) {}

    unique_ptr(unique_ptr && rhs) noexcept 
        : m_t(rhs.release(), std::forward<deleter_type>(rhs.get_deleter())) {}

    ~unique_ptr() noexcept {
        auto & ptr = m_t.ptr();
        if (ptr != nullptr) {
            get_deleter()(std::move(ptr));
            ptr = pointer();
        }
    }

    unique_ptr& operator=(unique_ptr &&rhs) noexcept {
        reset(rhs.release());
        get_deleter() = std::forward<deleter_type>(rhs.get_deleter());
        return *this;
    }

    unique_ptr& operator=(nullptr_t) noexcept {
        reset();
        return *this;
    }

    T& operator*() const {
        return *get();
    }

    pointer operator->() const noexcept {
        return get();
    }

    pointer get() const noexcept {
        return m_t.ptr();
    }

    deleter_type& get_deleter() noexcept {
        return m_t.deleter();
    }

    const deleter_type& get_deleter() const noexcept {
        return m_t.deleter();
    }
    
    explicit operator bool() const noexcept {
        return get() == pointer() ? false : true;
    }

    pointer release() noexcept {
        pointer p = get();
        m_t.ptr() = pointer();
        return p;
    }

    void reset(pointer p = pointer()) noexcept {
        using std::swap;
        swap(m_t.ptr(), p);
        if (p != pointer()) {
            get_deleter()(std::move(p));
        }
    }

    void swap(unique_ptr& rhs) noexcept {
        m_t.swap(rhs.m_t);
    }

    unique_ptr(const unique_ptr&) = delete;
    unique_ptr& operator=(const unique_ptr&) = delete;
};

template<typename T, typename ...Args>
unique_ptr<T> make_unique(Args&&... args) {
    return unique_ptr<T>(new T(std::forward<Args>(args)...));
}

// }  // namespace Ephmeral

#endif  // __UNIQUE_PTR__