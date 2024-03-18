#pragma once
#ifndef CVM_MEMORY_H
#define CVM_MEMORY_H

#include<cstdint>
#include<concepts>
#include<cstring>
struct memory {
    memory(uint32_t size);
    ~memory();
    memory(const memory&) = delete;
    memory& operator=(const memory&) = delete;
    memory(memory&&) = delete; 

    template<typename T>
    requires std::is_trivially_copyable_v<T>
    const T& read(void* addr) const {
        auto aligned_address = reinterpret_cast<uintptr_t>(addr) & ~0xfff;
        if (aligned_address < reinterpret_cast<uintptr_t>(baseaddr) || aligned_address > reinterpret_cast<uintptr_t>(baseaddr) + _size) { //oob
            throw std::out_of_range("Invalid memory access");
        }
        return *reinterpret_cast<T*>(addr);
    }

    template<typename T>
    requires std::is_trivially_copyable_v<T>
    void write(void* addr, const T& value) {
        auto aligned_address = reinterpret_cast<uintptr_t>(addr) & ~0xfff;
        if (aligned_address < reinterpret_cast<uintptr_t>(baseaddr) || aligned_address > reinterpret_cast<uintptr_t>(baseaddr) + _size) { //oob
            throw std::out_of_range("Invalid memory access");
        }
        std::memcpy(addr, &value, sizeof(T));
    }

    char* get_raw_ptr() const noexcept;
    uint32_t size() const noexcept;

private:
    char* baseaddr;
    uint32_t _size;  
};

struct stack{
    stack(uint32_t size);
    ~stack();
    stack(const stack&) = delete;
    stack& operator=(const stack&) = delete;
    stack(stack&&) = delete;

    template<typename T>
    requires std::is_trivially_copyable_v<T>
    const T& pop() {
        if (sp - bp < sizeof(T)) {
            throw std::out_of_range("Stack underflow");
        }
        sp -= sizeof(T);
        return *reinterpret_cast<T*>(sp);
    }

    const uint64_t& pop() {
        return pop<uint64_t>();
    }

    template<typename T>
    requires std::is_trivially_copyable_v<T>
    void push(const T& value) {
        if (sp + sizeof(T) == bp + mem.size()) {
            throw std::out_of_range("Stack overflow");
        }
        mem.write<T>(sp, value);
        sp += sizeof(T);
    }

    void push(const uint64_t& value) {
        push<uint64_t>(value);
    }

    char* get_raw_ptr() const noexcept;

    uint32_t size() const noexcept;

private:
    char* bp;
    char* sp;
    memory mem;
};

#endif // CVM_MEMORY_H

