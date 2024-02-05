// Copyright (c) 2024 Pyarelal Knowles, MIT License

#pragma once

#include <cstddef>
#include <memory_resource>
#include <span>
#include <vector>

// std::allocator backed linear allocator. Use decodeless_writer for efficiently
// creating large files.
class creator {
public:
    creator(size_t size)
        : m_buffer((size + sizeof(std::max_align_t) - 1) / sizeof(std::max_align_t)),
          m_resource(m_buffer.data(), m_buffer.size() * sizeof(std::max_align_t)),
          m_alloc(&m_resource) {}
    template <class T, class... Args>
    T* construct(Args&&... args) {
        T* result = m_alloc.allocate_object<T>(1);
        m_alloc.construct(result, std::forward<Args>(args)...);
        return result;
    }
    template <class T>
    std::span<T> constructArray(size_t size) {
        std::span<T> result(m_alloc.allocate_object<T>(size), size);
        for (auto& r : result)
            m_alloc.construct(&r);
        return result;
    }
    void* data() { return m_buffer.data(); }
    std::pmr::polymorphic_allocator<std::byte>& allocator() { return m_alloc; }

private:
    std::vector<std::max_align_t>              m_buffer;
    std::pmr::monotonic_buffer_resource        m_resource;
    std::pmr::polymorphic_allocator<std::byte> m_alloc;
};