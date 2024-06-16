// Copyright (c) 2024 Pyarelal Knowles, MIT License

#pragma once

#include <decodeless/offset_span.hpp>
#include <limits>
#include <stdexcept>
#include <type_traits>

namespace rtr {

using offset_string = decodeless::offset_span<const char>;

template <class T>
class optional {
public:
    constexpr optional() noexcept = default;
    constexpr optional(std::nullopt_t) noexcept
        : optional() {}
    constexpr optional(const optional& other) {
        if (other)
            *this = *other;
    }
    constexpr optional(optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (bool(other))
            *this = std::move(*other);
    }
    void reset() {
        if (m_exists)
            std::destroy_at(&m_storage.value);
        m_exists = false;
    }
    template <class... Args>
    T& emplace(Args&&... args) {
        reset();
        std::construct_at(&m_storage.value, std::forward<Args>(args)...);
        m_exists = true;
        return **this;
    }
    optional& operator=(const optional& other) {
        if (other)
            emplace(*other);
        return *this;
    };
    optional& operator=(optional&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (other)
            emplace(std::move(*other));
        return *this;
    };
    optional& operator=(const T& other) {
        emplace(other);
        return *this;
    };
    optional& operator=(T&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        emplace(std::move(other));
        return *this;
    };
    constexpr explicit operator const bool&() const noexcept { return m_exists; }
    T&       operator*() { return m_storage.value; }
    const T& operator*() const { return m_storage.value; }
    T*       operator->() { return &m_storage.value; }
    const T* operator->() const { return &m_storage.value; }
    T&       value() {
        if (!*this)
            throw std::bad_optional_access();
        return **this;
    }
    const T& value() const {
        if (!*this)
            throw std::bad_optional_access();
        return **this;
    }
    constexpr T value_or(const T& fallback) const { return *this ? **this : fallback; }

private:
    union storage {
        storage()
            : empty() {}
        char empty;
        T    value;
    };
    storage m_storage;
    bool m_exists = false;
};

// Like optional above, but using a sentinel value instead of a separate boolean
template <class T, T InvalidValue>
    requires std::is_trivially_destructible_v<T>
class optional_value {
public:
    static constexpr T invalid_value = InvalidValue;
    constexpr optional_value() noexcept = default;
    constexpr optional_value(std::nullopt_t) noexcept
        : optional_value() {}
    constexpr optional_value(const optional_value& other) {
        if (other)
            *this = *other;
    }
    constexpr optional_value(optional_value&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (bool(other))
            *this = std::move(*other);
    }
    constexpr optional_value(const T& value) { set(value); }
    void set(const T& value) {
        if (value == invalid_value)
            throw std::runtime_error("Setting optional to InvalidValue sentinel");
        m_value = value;
    }
    void      reset() { m_value = invalid_value; }
    optional_value& operator=(const optional_value& other) {
        if (other)
            set(*other);
        return *this;
    };
    optional_value& operator=(optional_value&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (other)
            set(std::move(*other));
        return *this;
    };
    optional_value& operator=(const T& value) {
        set(value);
        return *this;
    };
    optional_value& operator=(T&& other) noexcept(std::is_nothrow_move_constructible_v<T>) {
        set(std::move(other));
        return *this;
    };
    constexpr explicit operator bool() const noexcept { return !(m_value == invalid_value); }
    const T&           operator*() const { return m_value; }
    const T*           operator->() const { return &m_value; }
    T&                 value() {
        if (!*this)
            throw std::bad_optional_access();
        return **this;
    }
    const T& value() const {
        if (!*this)
            throw std::bad_optional_access();
        return **this;
    }
    constexpr T value_or(const T& fallback) const { return *this ? **this : fallback; }

private:
    T m_value = invalid_value;
};

template<class T>
using optional_index = optional_value<T, std::numeric_limits<T>::max()>;

using optional_index32 = optional_index<uint32_t>;

}; // namespace rtr