#include "cvm-vm.h"
#include<iostream>
#include<utility>
#include<cstring>

void vm::raise_interrupt(vm_interrupt& interrupt) {
    if(interrupt_raised) {
        halt("Interrupt already raised");
        return;
    }
    interrupt_raised = true;
    switch (interrupt.type)
    {
    case interrupt_type::INT_ACCESS_VIOLAION:
    case interrupt_type::INT_DIV_ZERO:
    case interrupt_type::INT_INVALID_INSTRUCTION:
        halt("Unhandled exception"); //TODO: add exception handlers
        return;
    case interrupt_type::INT_MODULE_CALL:
        bool interrupt_handled=false;
        for (auto& vmmod: modules) {
            auto ret = vmmod->handle_interrupt(interrupt.data);
            if(ret.has_value()) {
                interrupt_handled = true;
                runtime_stack.push<int64_t>(ret.value());
                status = STATUS_SUCCESS;
                break;
            }
        }
        if(!interrupt_handled) {
            status = STATUS_INVALID_CALL;
        }
        break;
    }

    interrupt_raised = false;
}

void vm::start() {
    running = true;
    pc=0;
    for (auto& vmmod: modules) {
        vmmod->on_vm_start();
    }
    while(running) {
        step_once();
    }
}

void vm::halt(const char* message) {
    running = false;
    for (auto& vmmod: modules) {
        vmmod->on_vm_exit();
    }
    std::cerr<<message<<std::endl;
}

void vm::step_once() {
    if(!running || interrupt_raised) {
        std::unreachable();
    }

    try{
        if(pc>=code.size()) {
            throw vm_interrupt{interrupt_type::INT_ACCESS_VIOLAION, pc};
        }
        auto insn = code.read<uint8_t>(code.get_raw_ptr()+pc);
        if(insn>=sizeof(instructions)/sizeof(instructions[0])) {
            throw vm_interrupt{interrupt_type::INT_INVALID_INSTRUCTION, insn};
        }
        instructions[insn].func(this);
    } catch(vm_interrupt& interrupt) {
        raise_interrupt(interrupt);
    } catch(std::exception& e) {
        halt(e.what());
    }
}

void vm::vm_nop(vm* vm) {
    vm->pc++;
}

void vm::vm_pop(vm* vm) {
    vm->pc++;
    vm->runtime_stack.pop<uint64_t>();
}

void vm::vm_add(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(a+b);
}

void vm::vm_sub(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(a-b);
}

void vm::vm_mul(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(a*b);
}

void vm::vm_div(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    if(b==0) {
        vm->runtime_stack.push<uint64_t>(b);
        vm->runtime_stack.push<uint64_t>(a);

        throw vm_interrupt{interrupt_type::INT_DIV_ZERO, 0};
    }
    vm->runtime_stack.push<uint64_t>(a/b);
}

void vm::vm_mod(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(a%b);
}

void vm::vm_and(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(a&b);
}

void vm::vm_or(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(a|b);
}

void vm::vm_xor(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(a^b);
}

void vm::vm_not(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(~a);
}

void vm::vm_call(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    vm->pc=a;
    vm->runtime_stack.push<uint64_t>(vm->pc);
}

void vm::vm_push_imm1(vm* vm) {
    vm->pc++;
    auto a = vm->code.read<uint8_t>(vm->code.get_raw_ptr()+vm->pc);
    vm->pc++;
    vm->runtime_stack.push<int8_t>(a);
}

void vm::vm_push_imm2(vm* vm) {
    vm->pc++;
    auto a = vm->code.read<int16_t>(vm->code.get_raw_ptr()+vm->pc);
    vm->pc+=2;
    vm->runtime_stack.push<int16_t>(a);
}

void vm::vm_push_imm4(vm* vm) {
    vm->pc++;
    auto a = vm->code.read<int32_t>(vm->code.get_raw_ptr()+vm->pc);
    vm->pc+=4;
    vm->runtime_stack.push<int32_t>(a);
}

void vm::vm_push_imm8(vm* vm) {
    vm->pc++;
    auto a = vm->code.read<int64_t>(vm->code.get_raw_ptr()+vm->pc);
    vm->pc+=8;
    vm->runtime_stack.push<int64_t>(a);
}

void vm::vm_hlt(vm* vm) {
    vm->halt("");
}

void vm::vm_jmp(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    vm->pc=a;
}

void vm::vm_lea(vm* vm) {
    vm->pc++;
    auto offset = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(reinterpret_cast<uintptr_t>(vm->runtime_stack.get_raw_ptr()+offset));
}

void vm::vm_modcall(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    throw vm_interrupt{interrupt_type::INT_MODULE_CALL, a};
}

void vm::vm_jz(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<uint64_t>();
    if(b==0) {
        vm->pc=a;
    }
}

void vm::vm_jb(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<int64_t>();
    if(b<0) {
        vm->pc=a;
    }
}

void vm::vm_ja(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    auto b = vm->runtime_stack.pop<int64_t>();
    if(b>0) {
        vm->pc=a;
    }
}

void vm::vm_dup(vm* vm) {
    vm->pc++;
    auto a = vm->runtime_stack.pop<uint64_t>();
    vm->runtime_stack.push<uint64_t>(a);
    vm->runtime_stack.push<uint64_t>(a);
}