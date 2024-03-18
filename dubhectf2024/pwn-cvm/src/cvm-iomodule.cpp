#include "cvm-iomodule.h"
#include "cvm-interrupt.h"

IOModule::IOModule() {}

void IOModule::on_vm_start() {
    add_function("read", IOModule::io_read);
    add_function("write", IOModule::io_write);
}


int32_t IOModule::io_read(vm* vm, uintptr_t addr, uint64_t size) {
    auto target = reinterpret_cast<char*>(addr);
    if((target<vm->code.get_raw_ptr() || (target+size)>vm->code.get_raw_ptr()+vm->code.size())
    &&(target<vm->runtime_stack.get_raw_ptr() || (target+size)>vm->runtime_stack.get_raw_ptr()+vm->runtime_stack.size())) {
        throw vm_interrupt{interrupt_type::INT_ACCESS_VIOLAION, 0};
    }
    return read(0, target, size);
}

int32_t IOModule::io_write(vm* vm, uintptr_t addr, uint64_t size) {
    auto target = reinterpret_cast<char*>(addr);
    if((target<vm->code.get_raw_ptr() || (target+size)>vm->code.get_raw_ptr()+vm->code.size())
    &&(target<vm->runtime_stack.get_raw_ptr() || (target+size)>vm->runtime_stack.get_raw_ptr()+vm->runtime_stack.size())) {
        throw vm_interrupt{interrupt_type::INT_ACCESS_VIOLAION, 0};
    }
    return write(1, target, size);
}
