#pragma once
#ifndef CVM_VM_H
#define CVM_VM_H

#include "cvm-module.h"
#include "cvm-interrupt.h"
#include<vector>
#include<memory>
#include<concepts>
#include "cvm-memory.h"
#include<array>

struct vm;
struct vm_module;

struct instruction
{
    constexpr instruction(int opcode, void(*func)(vm*)): func(func) {}
    void(*func)(vm*);
};


enum vm_status{
    STATUS_SUCCESS = 0,
    STATUS_FAILED = -1,
    STATUS_INVALID_CALL = -2,
};

struct vm{
    friend class vm_module;

    template<typename T>
    requires std::is_base_of_v<vm_module, T>
    void add_module() {
        auto mod = std::make_unique<T>();
        mod->parent_vm = this;
        modules.push_back(std::move(mod));
    }

    void start();

    int64_t status = STATUS_SUCCESS;

    stack runtime_stack{0x20000};
    memory code{0x20000};
private:
    std::vector<std::unique_ptr<vm_module>> modules = {};
    void raise_interrupt(vm_interrupt& interrupt);
    bool interrupt_raised = false;
    void halt(const char* message);
    bool running = false;
    void step_once();

    uint32_t pc = 0;

    static void vm_nop(vm* vm);
    static void vm_pop(vm* vm);
    static void vm_add(vm* vm);
    static void vm_sub(vm* vm);
    static void vm_mul(vm* vm);
    static void vm_div(vm* vm);
    static void vm_mod(vm* vm);
    static void vm_and(vm* vm);
    static void vm_or(vm* vm);
    static void vm_xor(vm* vm);
    static void vm_not(vm* vm);
    static void vm_call(vm* vm);
    static void vm_jmp(vm* vm);
    static void vm_lea(vm* vm);
    static void vm_push_imm1(vm* vm);
    static void vm_push_imm2(vm* vm);
    static void vm_push_imm4(vm* vm);
    static void vm_push_imm8(vm* vm);
    static void vm_modcall(vm* vm);
    static void vm_jz(vm* vm);
    static void vm_jb(vm* vm);
    static void vm_ja(vm* vm);
    static void vm_dup(vm* vm);
    static void vm_hlt(vm* vm);

    constexpr static std::array<instruction,0x18> instructions = {
        instruction(0x00, &vm_nop),
        instruction(0x01, &vm_pop),
        instruction(0x02, &vm_add),
        instruction(0x03, &vm_sub),
        instruction(0x04, &vm_mul),
        instruction(0x05, &vm_div),
        instruction(0x06, &vm_mod),
        instruction(0x07, &vm_and),
        instruction(0x08, &vm_or),
        instruction(0x09, &vm_xor),
        instruction(0x0a, &vm_not),
        instruction(0x0b, &vm_call),
        instruction(0x0c, &vm_jmp),
        instruction(0x0d, &vm_lea),
        instruction(0x0e, &vm_push_imm1),
        instruction(0x0f, &vm_push_imm2),
        instruction(0x10, &vm_push_imm4),
        instruction(0x11, &vm_push_imm8),
        instruction(0x12, &vm_modcall),
        instruction(0x13, &vm_jz),
        instruction(0x14, &vm_jb),
        instruction(0x15, &vm_ja),
        instruction(0x16, &vm_dup),
        instruction(0x17, &vm_hlt),
    };
};

#endif // CVM_VM_H