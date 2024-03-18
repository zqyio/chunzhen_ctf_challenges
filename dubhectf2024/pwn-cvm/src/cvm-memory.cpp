#include <cstdint>
#include <stdexcept>
#include "cvm-memory.h"
#include <sys/mman.h>
#include <cstring>

memory::memory(uint32_t size) : _size{size} {
    if (size == 0 || (size & 0xfff) != 0) {
        throw std::invalid_argument("Invalid memory size");
    }
    baseaddr = static_cast<char*>(mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0));
    if (baseaddr == MAP_FAILED) {
        throw std::runtime_error("Failed to allocate memory");
    }
    std::memset(baseaddr, 0, size);
}

memory::~memory() {
    munmap(baseaddr, _size);
}

char* memory::get_raw_ptr() const noexcept{
    return baseaddr;
}

uint32_t memory::size() const noexcept {
    return _size;
}

stack::stack(uint32_t size) : mem{size} {
    bp = mem.get_raw_ptr();
    sp = bp;
}

stack::~stack() {}

uint32_t stack::size() const noexcept{
    return mem.size();
}

char* stack::get_raw_ptr() const noexcept{
    return mem.get_raw_ptr();
}