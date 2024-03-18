#include "cvm-module.h"
#include<stdexcept>
#include<vector>
#include<functional>

std::expected<int, bool> vm_module::handle_interrupt(size_t data) {
    if(func_map.contains(data)) {
        auto& func = func_map.at(data);

        std::vector<uint64_t> args{};
        for(int i=0;i<func->arg_count;i++){
            args.emplace_back(this->parent_vm->runtime_stack.pop());
        }

        return func->call(args);
    }
    else {
        return std::unexpected(false);
    }
}

