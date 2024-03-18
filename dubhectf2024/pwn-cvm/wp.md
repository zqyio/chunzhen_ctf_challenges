# cvm出题手记

## 前言

没想到这道题会保持零解到最后，可能是因为strip之后二进制太恶心了，在这里先给大家磕一个orz

不过除了vm_interrupt是throw出来的之外，我也没有刻意给大家添逆向难度，源码除去空行一共也就600行，都是C++的问题（嗯

当时出题的时候考虑的是结合动调、少逆多猜，毕竟在程序中，若干个opcode还是挺明显的，通过program.bin倒推也行

对这道题我也有几个不太满意的地方：

1. 为了埋洞把vm写得很别扭
2. 滥用了exception用于vm_interrupt
3. 有几个函数模板的约束不够
4. interrupt处理得不太好
5. 该用pch的，编译速度太慢（）

## 分析

### VM

就是一个简单的栈机，实现了若干代数运算，jmp等。栈是通过host memory address直接访问的。其中比较重要的指令

- lea: 根据一个偏移获取真实的栈上地址
- jmp: 根据绝对PC地址跳转
- ja/jb/jz: 根据栈上值的比较结果条件跳转
- dup: 将栈上的8字节内容复制一份压入栈
- push1/push2/push4/push8: 压入对应字节长度的立即数
- pop: 将8字节长度的元素弹出栈
- modcall: 调用vm module的函数，实现与宿主机的交互（例如IO）

### 用户程序

```
read equ 0xf06d7659e3c6851c
write equ 0x14242c101498d2fc

push8 0x20656d6f636c6557
push8 0x4654435354206f74
push8 0x74726174530a0a21
push8 0x78652072756f7920
push8 0x72662074696f6c70
push8 0x2165726568206d6f
push8 48
lea 0
modcall write
sub 48
dup
ja [label1]
jb [label1]

push8 [label2]

push8 0x1000
lea 0
modcall read
pop
jmp


[label1]
hlt

[label2]
push8 0x6e75662065766148
push8 8
lea 48
modcall write
hlt
```

可以发现read的时候是直接由栈底向上读0x1000字节，一个很明显的栈溢出，正好能覆盖[label2]的地址，从而实现控制流劫持。`lea 0`相当于`push8 0; lea;`，而read可以向code和stack读，所以我们可以控制lea的偏移，使其写到code部分，从而实现shellcode写入；再次控制jmp的地址，从而实现shellcode执行

### VM

通过lea指令，我们可以很容易地实现地址泄露：先lea地址到栈上然后write即可

通过分析，发现地址的边界检查存在问题

```c++
//cvm-memory.h
struct memory{
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
}

struct stack{
    template<typename T>
    requires std::is_trivially_copyable_v<T>
    void push(const T& value) {
        if (sp + sizeof(T) == bp + mem.size()) { //oob
            throw std::out_of_range("Stack overflow");
        }
        mem.write<T>(sp, value);
        sp += sizeof(T);
    }
}

```

通过push1,2,4,8的使用，我们可以绕过`sp + sizeof(T) == bp + mem.size()`的检查，实现0xfff字节的越界写

通过查看vmmap，我们发现溢出的部分是TLS部分，而这个程序刚好是个多线程程序，可以想到覆盖pthread结构体实现控制流劫持。

即设置pthread_cancel，然后覆盖cleanup_jmp_buf指针到伪造的buf。

## exp

见https://github.com/zqyio/chunzhen_ctf_challenges/tree/main/dubhectf2024/pwn-cvm/exp

（请忽略我写的构式assembler）