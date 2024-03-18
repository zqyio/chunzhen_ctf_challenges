#pragma once
#ifndef CVM_IOMODULE_H
#define CVM_IOMODULE_H

#include "cvm-module.h"
struct IOModule : public vm_module
{
    IOModule();
    ~IOModule() override = default;
    void on_vm_start() override;
    static int32_t io_read(vm* vm, uintptr_t addr, uint64_t size);
    static int32_t io_write(vm* vm, uintptr_t addr, uint64_t size);
};
#endif // CVM_IOMODULE_H