#include<fcntl.h>
#include<cstdio>
#include "cvm-vm.h"
#include "cvm-iomodule.h"
#include<thread>

int main(){
    setbuf(stdin, nullptr);
    setbuf(stdout, nullptr);
    setbuf(stderr, nullptr);

    auto* file = fopen("program.bin","r");
    if(file==nullptr){
        perror("fopen");
        return 1;
    }
    vm vm;
    vm.add_module<IOModule>();
    auto size = fread(vm.code.get_raw_ptr(), 1, vm.code.size(), file);
    if(size==0){
        perror("fread");
        return 1;
    }
    fclose(file);
    std::thread t([](){
        std::this_thread::sleep_for(std::chrono::seconds(10));
        exit(0);
    });
    vm.start();
    exit(0);
}