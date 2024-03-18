#pragma once
#ifndef CVM_INTERRUPT_H
#define CVM_INTERRUPT_H
#include<stdexcept>

enum interrupt_type{
    INT_INVALID_INSTRUCTION,
    INT_DIV_ZERO,
    INT_ACCESS_VIOLAION,
    INT_MODULE_CALL,
};

struct vm_interrupt
{
    interrupt_type type;
    size_t data;
};

#endif // CVM_INTERRUPT_H