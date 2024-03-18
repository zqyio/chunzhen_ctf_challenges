import sys
import os

def vm_nop():
    return b'\x00'

def vm_pop():
    return b'\x01'

def vm_add():
    return b'\x02'

def vm_sub():
    return b'\x03'

def vm_mul():
    return b'\x04'

def vm_div():
    return b'\x05'

def vm_mod():
    return b'\x06'

def vm_and():
    return b'\x07'

def vm_or():
    return b'\x08'

def vm_xor():
    return b'\x09'

def vm_not():
    return b'\x0A'

def vm_call():
    return b'\x0B'

def vm_jmp():
    return b'\x0C'

def vm_lea():
    return b'\x0D'

def vm_push1(value):
    return b'\x0E' + value.to_bytes(1, 'little')

def vm_push2(value):
    return b'\x0F' + value.to_bytes(2, 'little')

def vm_push4(value):
    return b'\x10' + value.to_bytes(4, 'little')

def vm_push8(value):
    return b'\x11' + value.to_bytes(8, 'little')

def vm_modcall():
    return b'\x12'

def vm_jz():
    return b'\x13'

def vm_jb():
    return b'\x14'

def vm_ja():
    return b'\x15'

def vm_dup():
    return b'\x16'

def vm_hlt():
    return b'\x17'

if __name__ == '__main__':
    if len(sys.argv) != 3:
        print(f'Usage: {sys.argv[0]} <input file> <output file>')
        sys.exit(1)
    bytecode = b''
    const_dict = {}
    code = open(sys.argv[1], 'r').read()
    code = [line for line in code.splitlines() if line and not line.startswith(';')]
    # preprocess equ
    for i in range(len(code)):
        line_content = [word for word in code[i].split() if word]
        if len(line_content) == 3 and line_content[1] == 'equ':
            const_dict[line_content[0]] = eval(line_content[2])
    code = [line for line in code if 'equ' not in line]
    # preprocess times
    while [line for line in code if line.startswith('times ')]:
        for i in range(len(code)):
            if code[i].startswith('times '):
                line_content = [word for word in code[i].split() if word]
                code = code[:i] + [''.join(line_content[2:])]*eval(line_content[1]) + code[i+1:]
                break
    
    # preprocess labels
    labels = []
    line_len = [0]*len(code)
    for i in range(len(code)):
        line_content = [word for word in code[i].split() if word]
        if len(line_content) == 1:
            if line_content[0][0]!='[':
                line_len[i]=1
            else:
                labels.append((code[i],i))
        elif line_content[0].startswith('push'):
            line_len[i]=1+eval(line_content[0][4])
        else:
            line_len[i]=1+1+8
    for i in range(len(code)):
        print(str(sum(line_len[:i]))+ ' > ' + code[i])

    for label,i in labels:
        code=[line.replace(label,str(sum(line_len[:i]))) if line!=label else '' for line in code]
    code = [line for line in code if line]

    # generate bytecode
    for i in range(len(code)):
        line_content = [word for word in code[i].split() if word]
        if len(line_content) == 1:
            bytecode += eval(f'vm_{line_content[0]}')()
        elif line_content[0].startswith('push'):
            if line_content[1] in const_dict:
                bytecode += eval(f'vm_{line_content[0]}')(const_dict[line_content[1]])
            else:
                bytecode += eval(f'vm_{line_content[0]}')(eval(line_content[1]))
        else:
            if line_content[1] in const_dict:
                bytecode += vm_push8(const_dict[line_content[1]])
            else:
                bytecode += vm_push8(eval(line_content[1]))
            bytecode += eval(f'vm_{line_content[0]}')()
    
    open(sys.argv[2], 'wb').write(bytecode)

    

