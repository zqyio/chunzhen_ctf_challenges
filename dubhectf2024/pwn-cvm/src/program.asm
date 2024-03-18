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