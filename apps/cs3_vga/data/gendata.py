#!/bin/python

import glob
import binascii

CHUNK_SIZE = 32

def chunks(l, n):
    for i in range(0, len(l), n):
        yield l[i:i+n]


symbols = ['#include <stdint.h>', '', '#pragma once', '']

with open('../src/data.cpp', 'wb') as tfile:
    i =0
    tfile.write('#include <cstdint>\n'.encode('utf-8'))
    for fname in glob.glob('*'):
        if fname.endswith('.py'):
            continue
        symbol = fname.replace('.', "_")
        print(fname)
        sfile = open(fname, 'rb')
        data = sfile.read()
        typex = 'uint8_t'
        if fname.endswith('.mcz'):
            # strip header from mcz file
            data = data[12:]        
        #elif fname.endswith('.pal'):
        #    typex = 'uint32_t'
        symbols.append(f'extern {typex} {symbol}[{len(data)}];')
        sfile.close()
        tfile.write(f'uint8_t {symbol}[]={{\n'.encode('utf-8'))
        for line in chunks(binascii.b2a_hex(data), CHUNK_SIZE):
            s = '    ' + ','.join(['0x'+ch.decode("utf-8") for ch in chunks(line, 2)])
            tfile.write(s.encode('utf-8'))
            if len(line) == CHUNK_SIZE:
                tfile.write(','.encode('utf-8'))
            tfile.write('\n'.encode('utf-8'))
        tfile.write('};\n\n'.encode('utf-8'))
        i += 1

#// extern uint8_t tiles_mcz;
with open('../src/data.h', 'wb') as tfile:
    t = '\n'.join(symbols)
    tfile.write(t.encode('utf-8'))