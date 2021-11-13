# Remote Machine Code Fetch & Exec



Remote Server Index:  
The remote server index consists of 3 parts:
### 1. Data Length
The first 3 characters represent the length of data to be printed out.  
This includes the newline character and null terminator.  

### 2. Data
### 3. Instructions
  
`26F68656c6c6f20776f726c640a00x31c031ffffc0ffc748beffffffff77777777ba0d0000000f05b83c00000031ff0f05c3`
   
```  ↑                          ↑```

Disassembly of `_malloc`
```
000000000000187e <_malloc>:
    187e:       55                      push   rbp
    187f:       48 89 e5                mov    rbp,rsp
    1882:       ff                      (bad)
    1883:       ff                      (bad)
    1884:       ff                      (bad)
    1885:       ff                      (bad)
    ...
    197f:       ff                      (bad)
    1980:       ff                      (bad)
    1981:       ff                      .byte 0xff
    1982:       90                      nop
    1983:       5d                      pop    rbp
    1984:       c3                      ret
```