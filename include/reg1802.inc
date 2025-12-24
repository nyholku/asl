                ifndef  __reg1802inc     ; avoid multiple inclusion
__reg1802inc    equ     1

                save
                listing off             ; no listing over this file

;****************************************************************************
;*                                                                          *
;*   AS - File REG1802.INC                                                  *
;*                                                                          *
;*   Purpose : Contains definitions and macros for RCA 1802/CDP1802         *
;*                                                                          *
;****************************************************************************

                if      MOMCPUNAME<>"1802"
                 fatal  "wrong processor type: only 1802 allowed"
                endif

                if      MOMPASS=1
                 message "RCA 1802/CDP1802 Definitions (C) 2025"
                endif

;----------------------------------------------------------------------------
; Register Number Definitions
;----------------------------------------------------------------------------

R0              equ     0               ; Register 0 (DMA address)
R1              equ     1               ; Register 1 (Interrupt address)
R2              equ     2               ; Register 2 (general purpose)
R3              equ     3               ; Register 3 (program counter - default)
R4              equ     4               ; Register 4 (call/return stack pointer)
R5              equ     5               ; Register 5 (general purpose)
R6              equ     6               ; Register 6 (general purpose)
R7              equ     7               ; Register 7 (general purpose)
R8              equ     8               ; Register 8 (general purpose)
R9              equ     9               ; Register 9 (general purpose)
RA              equ     10              ; Register A (hex notation)
RB              equ     11              ; Register B
RC              equ     12              ; Register C
RD              equ     13              ; Register D
RE              equ     14              ; Register E
RF              equ     15              ; Register F

; Common register aliases
DMA_ADDR        equ     R0              ; DMA address pointer
INT_ADDR        equ     R1              ; Interrupt address pointer
STACK_PTR       equ     R2              ; Common stack pointer
PC              equ     R3              ; Program counter (default)
CALL_STACK      equ     R4              ; SCRT call/return stack
LINK_PTR        equ     R5              ; SCRT linkage pointer
WORK1           equ     R6              ; Work register 1
WORK2           equ     R7              ; Work register 2

;----------------------------------------------------------------------------
; I/O Port Number Definitions
;----------------------------------------------------------------------------

PORT1           equ     1               ; I/O Port 1
PORT2           equ     2               ; I/O Port 2
PORT3           equ     3               ; I/O Port 3
PORT4           equ     4               ; I/O Port 4
PORT5           equ     5               ; I/O Port 5
PORT6           equ     6               ; I/O Port 6
PORT7           equ     7               ; I/O Port 7

;----------------------------------------------------------------------------
; Flag Values
;----------------------------------------------------------------------------

DF              equ     1               ; Data Flag bit (for DF tests)

;----------------------------------------------------------------------------
; Common Memory Locations (User-Definable)
;----------------------------------------------------------------------------
; These are typical conventions - adjust for your system

; Standard Call and Return Technique (SCRT) locations
; (Uncomment and adjust as needed for your system)

;SCRT_START     equ     0000h           ; SCRT entry point
;SCRT_LINK      equ     0003h           ; SCRT linkage
;INT_VECTOR     equ     0100h           ; Interrupt vector location

;----------------------------------------------------------------------------
; Useful Macros
;----------------------------------------------------------------------------

; Load 16-bit immediate value into register pair
ld16            macro   reg,value
                ldi     hi(value)
                phi     reg
                ldi     lo(value)
                plo     reg
                endm

; Push register pair to stack (pointed to by R2)
push16          macro   reg
                ghi     reg
                stxd
                glo     reg
                stxd
                endm

; Pop register pair from stack (pointed to by R2)
pop16           macro   reg
                ldxa
                plo     reg
                ldxa
                phi     reg
                endm

; Push D register to stack
pushd           macro
                stxd
                endm

; Pop D register from stack
popd            macro
                ldxa
                endm

; Set register as program counter
setpc           macro   reg
                sep     reg
                endm

; Set register as data pointer
setx            macro   reg
                sex     reg
                endm

; No operation (using 00h/IDL is not recommended)
nop2            macro
                ldi     0
                endm

; Return from subroutine (SCRT style)
; Assumes R5 contains return address
return          macro
                sep     R5
                endm

; Define byte with all registers
allregs_db      macro   instr
                irp     reg,0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15
                 instr  reg
                endm
                endm

;----------------------------------------------------------------------------
; Bit Manipulation Functions
;----------------------------------------------------------------------------

; Include standard bit functions if available
; (These provide hi(), lo(), mask(), etc.)
                ifdef   bitfuncsinc
                ; Already included
                else
; Define minimal set if bitfuncs.inc not included
hi              function x,(x>>8)&0FFh
lo              function x,x&0FFh
                endif

;----------------------------------------------------------------------------

                restore                 ; allow listing again

                endif                   ; __reg1802inc
