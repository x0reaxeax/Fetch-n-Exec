# Remote Machine Code Fetch & Exec
_in other words, another self rewriting binary.. boy I just love doing these._

## Description
The idea behind this one is to fetch opcodes and data from a remote server to rewrite the binary with during runtime.  
Except this time, we go x64 and `-nostdlib`, so everything will be done using syscalls and a few helper buddies from [x0lib](https://github.com/x0reaxeax/x0l1b).

## Remote Server Setup 
Alright, we have our `index` file, which contains this long mess:
`26F68656c6c6f20776f726c640a00x31c031ffffc0ffc748beffffffff77777777ba0d0000000f05b83c00000031ff0f05c3`

This can be broken down into 3 parts:
### 1. Data Length
`26F`  
The first 3 characters represent the length of data to be printed out.  
This includes the newline character and null terminator.  

The length is specified with decimal numbers and a non-decimal character to fill up the space, if needed. I just chose `F`, because "F is for Family" was playing on the TV, but literally anything non-decimal should be good to go.

To be more specific, this number specifies is the **number of characters** in the next part of the payload.

There is 13 bytes represented by 26 characters.

### 2. Data
`68656c6c6f20776f726c640a00`  
This is the payload to be printed. ASCII letters represented in hexadecimal. There is 11 of them (11x2). Afterwards comes the newline character (`0x0a`) and the null terminator (`0x00`). In this case, the text "hello world" is defined here.

(11+2) × 2 = 26 characters in this section.

### 3. Instructions
`31c031ffffc0ffc748beffffffff77777777ba0d0000000f05b83c00000031ff0f05c3`  
This final part is separated from the previous part by the letter `'x'`, because it made debugging and constructing the payload easier.
These are our opcodes to be written and executed.

```asm
0:  31 c0                   xor    eax, eax
2:  31 ff                   xor    edi, edi
4:  ff c0                   inc    eax
6:  ff c7                   inc    edi
8:  48 be ff ff ff ff 77    movabs rsi, 0x77777777ffffffff
f:  77 77 77
12: ba 0d 00 00 00          mov    edx, 0xd
17: 0f 05                   syscall
19: b8 3c 00 00 00          mov    eax, 0x3c
1e: 31 ff                   xor    edi, edi
20: 0f 05                   syscall
```

The first 4 lines load the `sys_write` syscall into `eax` and `STDOUT_FILENO` (`0x1`) into `edi`. We save 2 bytes by not using the `mov` instruction.  
The following `mov` instruction at `0x8` loads a placeholder 64bit address `0x77777777FFFFFFFF` into `rsi`. We will be overwriting this, once we can determine our data address during runtime (technically, this could totally be done now without any runtime calculations, but I will benefit from this approach, when I'll be doing **🎇 r e v e r s e _ s h e l l s 🎆** next).  
Next is loading the hardcoded size `0xd` as the `strlen` argument. This will be overwritten during runtime in the upgraded version of this that was just mentioned above, but for now, it's `(int) 13`.  
Now, we just execute this with `syscall`.  
Following is only the `sys_exit` syscall, with return code `0` (`EXIT_SUCCESS`), which just obviously ends the program, since (for now, hihihaha) there's nowhere we can jump to continue execution.


## Order of operations
First, we try to fetch the remote data using the function `read_remote_bytes()`. I say "try", because this makeshift function uses the Linux socket for IPv4 communication, which basic functionality is achieved mostly by glueing some syscalls together.  
Two specific syscalls are known to cause trouble here, that being `sys_recvfrom` and `sys_sendto`. These guys don't want to work all the time for some reason (in other words, me doing something wrong), but due to nature of this function being completely makeshift, I don't really care that much. If the execution fails, one of the following error codes is returned:

| Code | Name      | Description                   | 
|------|-----------|-------------------------------|
| 3    | EINPUT    | Invalid/Corrupted remote data |
| 4    | EMPROTECT | sys_mprotect RWX failure      |
| 5    | EREAD     | Error reading socket fd       |
| 6    | ESOCKSEND | sys_sendto error |
| 7    | ESOCKRECV | sys_recvfrom error |
| 8    | ECLOSE    | Error closing socket fd |

Check yours with `$ echo $?`  

After we successfully fetch our remote data, we need to get `WRITE` permission for the text segment, or to be more specific, the closest, page-aligned address to our target function-to-be-rewrtitten - `int _malloc(void)`. I don't really know why I named it `_malloc`, so just deal with it. We use the `sys_mprotect` syscall to achieve this, which we feed with `PROT_READ | PROT_WRITE | PROT_EXEC`, to flip the bits we need (RWX).  

The only things left to do are converting our remote-fetched characters to hex bytes, writing them at `&_malloc` and calculating a usable address to store our data to be printed, which is actually the very first byte after our last opcode, so that's where we write our data. After that, we are ready to execute!

## Demo


https://user-images.githubusercontent.com/61374847/141669441-661e17da-a441-4d34-899b-ef8fdcf8ba3b.mp4  

![733655604404420648](https://user-images.githubusercontent.com/61374847/141669516-c19286fe-b13c-4f6e-99ff-080a201d94dc.png)

This is the disassembly of compiled code for our `_malloc` function:

Disassembly of `_malloc`
```nasm
000000000000187e <_malloc>:
    187e:       55                      push   rbp
    187f:       48 89 e5                mov    rbp,rsp
    1882:       ff                      (bad)
    1883:       ff                      (bad)
    1884:       ff                      (bad)
    1885:       ff                      (bad)
    ; < ... repeat ... >
    197f:       ff                      (bad)
    1980:       ff                      (bad)
    1981:       ff                      .byte 0xff
    1982:       90                      nop
    1983:       5d                      pop    rbp
    1984:       c3                      ret
```


**Runtime** Dump of assembler code for function `_malloc`:
```asm
   0x000055555555587f <+0>:     31 c0                           xor    eax, eax
   0x0000555555555881 <+2>:     31 ff                           xor    edi, edi
   0x0000555555555883 <+4>:     ff c0                           inc    eax
   0x0000555555555885 <+6>:     ff c7                           inc    edi
   0x0000555555555887 <+8>:     48 be a2 58 55 55 55 55 00 00   movabs rsi, 0x5555555558a2
   0x0000555555555891 <+18>:    ba 0d 00 00 00                  mov    edx, 0xd
   0x0000555555555896 <+23>:    0f 05                           syscall
   0x0000555555555898 <+25>:    b8 3c 00 00 00                  mov    eax, 0x3c
   0x000055555555589d <+30>:    31 ff                           xor    edi,edi
   0x000055555555589f <+32>:    0f 05                           syscall
   0x00005555555558a1 <+34>:    c3                              ret
   < --- DATA BYTES START HERE --- >
   0x00005555555558a2 <+35>:    68 65 6c 6c 6f
   0x00005555555558a7 <+40>:    20 77 6f
   0x00005555555558aa <+43>:    72 6c   jb
   0x00005555555558ac <+45>:    64 0a 00
```
