# ASL Macro Programming Guide

## Overview

ASL (Macro Assembler) supports a comprehensive macro system with features including:
- Parameterized macros
- Iterative constructs (IRP, IRPC, REPT, WHILE)
- Special macro variables
- Functions (inline expression evaluation)
- Conditional macro expansion
- Nested macros

## Basic Macro Syntax

### Defining a Simple Macro

```assembly
; Basic macro definition
delay   macro   n
        ld      c,n
loop    nop
        djnz    loop
        endm

; Using the macro
        delay   10      ; Expands with parameter n=10
```

### Macro with Multiple Parameters

```assembly
; Macro with two parameters
move16  macro   src,dst
        lda     src
        sta     dst
        lda     src+1
        sta     dst+1
        endm

; Usage
        move16  $1000,$2000
```

## Special Macro Variables

ASL provides several built-in macro variables:

### ARGCOUNT - Number of Arguments

```assembly
dc_len  macro   args
        dc.ATTRIBUTE    ARGCOUNT        ; Number of arguments passed
        if      ARGCOUNT<>0
         dc.ATTRIBUTE   ALLARGS         ; All arguments
        endif
        endm

        dc_len.b        1,2,3           ; ARGCOUNT = 3
        dc_len.w        1               ; ARGCOUNT = 1
        dc_len.l        5,6,7,8,9,10    ; ARGCOUNT = 6
        dc_len.q                        ; ARGCOUNT = 0
```

### ALLARGS - All Arguments as List

```assembly
; Use ALLARGS to pass all arguments at once
data_list macro args
        db      ALLARGS
        endm

        data_list 10,20,30,40,50  ; Expands to: db 10,20,30,40,50
```

### ATTRIBUTE - Instruction Attribute (.b, .w, .l, etc.)

```assembly
; Access the size attribute from the macro call
store   macro   value
        mov.ATTRIBUTE   #value,d0
        endm

        store.b 5       ; Uses .b attribute
        store.w 1000    ; Uses .w attribute
        store.l 100000  ; Uses .l attribute
```

### __LABEL__ - Capture Calling Line's Label

```assembly
moveq2  macro   arg
__LABEL__ moveq #arg,d2
        endm

loop    moveq2  20      ; Label 'loop' is attached to the moveq instruction
        dbra    d2,loop
```

## Iterative Constructs

### IRP - Iterate over List

```assembly
; IRP iterates through a comma-separated list
allregs_nozero  macro   instr
        irp     reg,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
         instr  reg
        endm
        endm

; Usage (from 1802 example)
        allregs_nozero  ldn     ; Generates: ldn 1, ldn 2, ... ldn 15
```

### IRPC - Iterate over Characters

```assembly
; IRPC iterates through characters in a string
        irpc    name,{GLOBALSYMBOLS},"abc"
Rec_name Record
        endm
; Generates: Rec_a Record, Rec_b Record, Rec_c Record
```

### REPT - Repeat N Times

```assembly
; REPT repeats a block N times
NStruct         macro   name,cnt,{GLOBALSYMBOLS}
z               set     0
                rept    cnt,{GLOBALSYMBOLS}
z_str           set     "\{z}"
name_{z_str}    Record
z               set     z+1
                endm
                endm

; Usage
        NStruct Array,5         ; Creates Array_0 through Array_4
```

### WHILE - Conditional Loop

```assembly
; WHILE repeats while condition is true
cnt     set     0
        while   cnt!=10
var{"\{CNT}"} equ       cnt
cnt     set     cnt+1
        endm
; Generates var0 through var9
```

## Advanced Macro Features

### String Substitution with \{variable}

```assembly
; Use \{var} to substitute variable value in string context
z       set     1
        while   z<5,{GLOBALSYMBOLS}
z_str           set     "\{z}"          ; Convert number to string
Array2_{z_str}  Record                  ; Creates Array2_1, Array2_2, etc.
z               set     z+1
        endm
```

### Nested Macros

```assembly
; Macros can call other macros
allregs macro   instr
        instr   0
        allregs_nozero instr    ; Calls another macro
        endm

allregs_nozero  macro   instr
        irp     reg,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
         instr  reg
        endm
        endm
```

### Conditional Assembly in Macros

```assembly
; Macros can contain IF statements
link    macro   reg,count,{NoExpand}
        push    reg                     ; Save old base pointer
        ld      reg,sp                  ; Set up new base pointer
        if      count<>0
         add    sp,count                ; Reserve space on stack
        endif
        endm
```

## Macro Expansion Control

### MACEXP_DFT - Default Macro Expansion

```assembly
; Control whether macros are expanded in listings by default
        macexp_dft off          ; Turn off macro expansion

delay   macro   n
        ld      c,n
loop    nop
        djnz    loop
        endm

        delay   10              ; Won't be expanded in listing
```

### MACEXP_OVR - Override Macro Expansion

```assembly
; Override the default expansion setting
        macexp_ovr on           ; Force expansion for next macro

        delay   20              ; Will be expanded in listing

        macexp_ovr              ; Return to default

        delay   30              ; Back to default (not expanded)
```

## Functions (Inline Expressions)

Functions are like macros but evaluate to a single expression:

```assembly
; Define functions for bit manipulation
mask            function start,bits,((1<<bits)-1)<<start
invmask         function start,bits,~mask(start,bits)
cutout          function x,start,bits,x&mask(start,bits)

; Byte extraction functions
hi              function x,(x>>8)&255
lo              function x,x&255
hiword          function x,(x>>16)&65535
loword          function x,x&65535

; Boolean functions
odd             function x,(x&1)=1
even            function x,(x&1)=0
getbit          function x,n,(x>>n)&1

; Usage
        lda     #lo($1234)      ; Loads $34
        lda     #hi($1234)      ; Loads $12
```

## Real-World Macro Examples

### 1802 Register Iteration (from t_1802.asm)

```assembly
        cpu     1802

allregs_nozero  macro   instr
        irp     reg,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
         instr  reg
        endm
        endm

allregs macro   instr
        instr   0
        allregs_nozero instr
        endm

; Usage - generates all register variants
        allregs ldn             ; ldn 0, ldn 1, ... ldn 15
        allregs lda             ; lda 0, lda 1, ... lda 15
        allregs inc             ; inc 0, inc 1, ... inc 15
```

### Procedure Framework (from macros.inc)

```assembly
; Define a procedure with local variable support
proc            macro   name,{NoExpand}
                section name
                forward LocalSize
LocalSize       eval    0
                public  name
name            label   $
                endm

; Define local variables
DefLocal        macro   Name,Size,{NoExpand}
LocalSize       eval    LocalSize-Size
Name            equ     LocalSize
                endm

; End procedure
endp            macro   name,{NoExpand}
LocalSize       eval    0-LocalSize
                endsection name
                endm

; Usage
                proc    MyFunc
                DefLocal buffer,16
                DefLocal counter,2
                ; ... code here ...
                endp    MyFunc
```

### Structure Definition (from t_structs.asm)

```assembly
        cpu     6301

; Define a structure
Record          STRUCT
val8            rmb     1
val16           rmb     2
val96           rmb     12
                ENDSTRUCT

; Create multiple instances
        irp     name,{GLOBALSYMBOLS},Rec1,Rec2,Rec3
name            Record
        endm

; Access structure fields
        ldaa    Rec1_val8
        staa    Rec2_val8
        ldx     Rec1_val16
        stx     Rec2_val16
```

### MSP430 Instruction Emulation (from emulmsp.inc)

```assembly
; Emulate missing MSP430 instructions with macros
adc             macro   op
                addc.attribute #0,op
                endm

dec             macro   op
                sub.attribute #1,op
                endm

decd            macro   op
                sub.attribute #2,op
                endm

inc             macro   op
                add.attribute #1,op
                endm

incd            macro   op
                add.attribute #2,op
                endm
```

## Best Practices

1. **Use {NoExpand} or {GLOBALSYMBOLS}** to control listing expansion and symbol scope
2. **Use descriptive macro parameter names** for readability
3. **Add comments** to explain complex macro logic
4. **Use FORWARD** for forward-referenced symbols in macros
5. **Test macros** with different parameter combinations
6. **Use functions** for simple calculations, macros for code generation
7. **Use proper indentation** inside macro definitions

## Special Directives

- `FORWARD` - Declare a forward reference for a symbol
- `EVAL` - Evaluate an expression and assign to a symbol
- `SECTION` / `ENDSECTION` - Create named code sections
- `PUBLIC` - Make symbols visible outside current section
- `SAVE` / `RESTORE` - Save/restore assembler state (like listing settings)
- `LISTING OFF` / `LISTING ON` - Control listing output

## Macro Debugging

To debug macros, use:
- `-M` option to extract macro definitions
- `-P` option to see preprocessor output
- `MACEXP_OVR ON` to see macro expansion in listings
- Add `MESSAGE` directives inside macros for trace output

## Complete Working Example: 1802 Delay Routine

```assembly
        cpu     1802
        page    0

; Macro to generate delay loops
delay_ms macro  milliseconds,clockMHz
        ldi     hi(milliseconds*clockMHz/3)
        phi     2
        ldi     lo(milliseconds*clockMHz/3)
        plo     2
loop    dec     2
        glo     2
        bnz     loop
        ghi     2
        bnz     loop
        endm

        org     0000h

start:  delay_ms 1000,4         ; 1 second delay at 4MHz
        delay_ms 500,4          ; 0.5 second delay at 4MHz
        br      start
```

## Summary

ASL's macro system provides:
- **Parameterized macros** with special variables (ARGCOUNT, ALLARGS, ATTRIBUTE, __LABEL__)
- **Iteration constructs** (IRP, IRPC, REPT, WHILE)
- **Functions** for inline expression evaluation
- **Conditional assembly** with IF/ENDIF
- **Expansion control** (MACEXP_DFT, MACEXP_OVR)
- **String manipulation** with \{variable} substitution
- **Nested macros** and recursive capability

This makes ASL suitable for complex code generation, DSL creation, and maintaining large assembly projects with reusable code patterns.
