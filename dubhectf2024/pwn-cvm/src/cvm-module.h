#pragma once
#ifndef CVM_MODULE_H
#define CVM_MODULE_H
#include<cstdint>
#include<expected>
#include<cstddef>
#include<unordered_map>
#include<string>
#include "cvm-vm.h"
#include<memory>
#include<vector>

struct vm;

struct vm_func_base{
    virtual int32_t call(const std::vector<uint64_t>& args)=0;
    vm* parent_vm;
    size_t arg_count;
};

template<typename R, typename ... Args>
struct vm_func : vm_func_base{
    vm_func(vm* parent_vm, R(*f)(Args...)){
        this->parent_vm=parent_vm;
        this->f=f;
        this->arg_count = sizeof...(Args) - 1;
    }
    R(*f)(Args...);
    
    template <typename Func, typename Vec, std::size_t... I>
    constexpr auto apply_impl(Func&& f, Vec&& v, std::index_sequence<I...>) {
        return std::forward<Func>(f)(this->parent_vm, std::forward<Vec>(v)[I]...);
    }


    int call(const std::vector<uint64_t>& args) override{
        return apply_impl(this->f, args, std::make_index_sequence<sizeof...(Args) - 1>{});
    }
};

struct vm_module {
    virtual void on_vm_start() {};
    virtual void on_vm_exit() {};
    virtual ~vm_module() = default;

    virtual std::expected<int, bool> handle_interrupt(size_t data);
    
    template<typename R, typename ... Args>
    void add_function(const std::string& name, R(*func)(Args...)) {
        if(this->parent_vm == nullptr){
            throw std::invalid_argument("unexpected nullptr");
        };
        if(func_map.contains(std::hash<std::string>{}(name))) {
            throw std::invalid_argument("Function already exists");
        }
        func_map.emplace(std::hash<std::string>{}(name), std::make_unique<vm_func<R, Args...>>(this->parent_vm, func));
    }

private:
    vm* parent_vm = nullptr;
    friend class vm;
    std::unordered_map<size_t, std::unique_ptr<vm_func_base>> func_map{};
};


#endif // CVM_MODULE_H
