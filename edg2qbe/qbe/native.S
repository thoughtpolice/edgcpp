/* AArch64 ABI bridges for values (notably binary128) unavailable as QBE
 * scalar registers.  Packet: u64 stack_size, x8, x0..x7, q0..q7, stack bytes.
 * Result: x0,x1,q0,q1,q2,q3.  Stack size is rounded up to 16 by each bridge.
 * Both bridges carry unwind information; exceptions transparently cross them.
 */
.text
.p2align 2
.global __edg_qbe_call_native
.type __edg_qbe_call_native, %function
__edg_qbe_call_native:
.cfi_startproc
    stp x29, x30, [sp, #-64]!
.cfi_def_cfa_offset 64
.cfi_offset x29, -64
.cfi_offset x30, -56
    mov x29, sp
.cfi_def_cfa_register x29
    stp x19, x20, [sp, #16]
.cfi_offset x19, -48
.cfi_offset x20, -40
    str x21, [sp, #32]
.cfi_offset x21, -32
    mov x19, x0
    mov x20, x1
    mov x21, x2
    ldr x9, [x21]
    add x10, x9, #15
    and x10, x10, #-16
    sub sp, sp, x10
    add x11, x21, #208
    mov x12, sp
    cmp x9, #16
    b.lo .Lcall_copy_tail
.Lcall_copy_chunk:
    ldp x13, x14, [x11], #16
    stp x13, x14, [x12], #16
    sub x9, x9, #16
    cmp x9, #16
    b.hs .Lcall_copy_chunk
.Lcall_copy_tail:
    cbz x9, .Lcall_copy_done
.Lcall_copy_byte:
    ldrb w13, [x11], #1
    strb w13, [x12], #1
    subs x9, x9, #1
    b.ne .Lcall_copy_byte
.Lcall_copy_done:
    ldr x8, [x21, #8]
    ldp x0, x1, [x21, #16]
    ldp x2, x3, [x21, #32]
    ldp x4, x5, [x21, #48]
    ldp x6, x7, [x21, #64]
    ldp q0, q1, [x21, #80]
    ldp q2, q3, [x21, #112]
    ldp q4, q5, [x21, #144]
    ldp q6, q7, [x21, #176]
    blr x20
    stp x0, x1, [x19]
    stp q0, q1, [x19, #16]
    stp q2, q3, [x19, #48]
    mov sp, x29
    ldr x21, [sp, #32]
    ldp x19, x20, [sp, #16]
    ldp x29, x30, [sp], #64
.cfi_def_cfa sp, 0
.cfi_restore x29
.cfi_restore x30
.cfi_restore x19
.cfi_restore x20
.cfi_restore x21
    ret
.cfi_endproc
.size __edg_qbe_call_native, .-__edg_qbe_call_native

/* Native entry veneers set x16 to a QBE dispatcher and x17 to the exact
 * number of native stack argument bytes, then tail branch here. Dispatcher
 * signature: void dispatcher(const void *packet, void *result).
 * Entry-only packet extension at 208+round16(stack_size) holds original SP.
 */
.p2align 2
.global __edg_qbe_enter_native
.hidden __edg_qbe_enter_native
.type __edg_qbe_enter_native, %function
__edg_qbe_enter_native:
.cfi_startproc
    stp x29, x30, [sp, #-64]!
.cfi_def_cfa_offset 64
.cfi_offset x29, -64
.cfi_offset x30, -56
    mov x29, sp
.cfi_def_cfa_register x29
    stp x19, x20, [sp, #16]
.cfi_offset x19, -48
.cfi_offset x20, -40
    str x21, [sp, #32]
.cfi_offset x21, -32
    mov x20, x16
    add x9, x17, #15
    and x9, x9, #-16
    add x9, x9, #304
    sub sp, sp, x9
    mov x19, sp
    add x21, sp, #80
    stp x17, x8, [x21]
    stp x0, x1, [x21, #16]
    stp x2, x3, [x21, #32]
    stp x4, x5, [x21, #48]
    stp x6, x7, [x21, #64]
    stp q0, q1, [x21, #80]
    stp q2, q3, [x21, #112]
    stp q4, q5, [x21, #144]
    stp q6, q7, [x21, #176]
    add x10, x29, #64
    add x11, x21, #208
    add x9, x17, #15
    and x9, x9, #-16
    str x10, [x11, x9]
    cmp x17, #16
    b.lo .Lentry_copy_tail
.Lentry_copy_chunk:
    ldp x12, x13, [x10], #16
    stp x12, x13, [x11], #16
    sub x17, x17, #16
    cmp x17, #16
    b.hs .Lentry_copy_chunk
.Lentry_copy_tail:
    cbz x17, .Lentry_copy_done
.Lentry_copy_byte:
    ldrb w12, [x10], #1
    strb w12, [x11], #1
    subs x17, x17, #1
    b.ne .Lentry_copy_byte
.Lentry_copy_done:
    mov x0, x21
    mov x1, x19
    blr x20
    ldp x0, x1, [x19]
    ldp q0, q1, [x19, #16]
    ldp q2, q3, [x19, #48]
    mov sp, x29
    ldr x21, [sp, #32]
    ldp x19, x20, [sp, #16]
    ldp x29, x30, [sp], #64
.cfi_def_cfa sp, 0
.cfi_restore x29
.cfi_restore x30
.cfi_restore x19
.cfi_restore x20
.cfi_restore x21
    ret
.cfi_endproc
.size __edg_qbe_enter_native, .-__edg_qbe_enter_native

/* Compare-exchange uses the baseline exclusive instructions, not compiler
 * atomics (which would call these same helpers).  Strong retries are valid
 * for both weak and strong requests.  Acquire/release ordering on every
 * attempt is a permitted strengthening of the requested memory orders,
 * including the LDAXR/STLXR sequence used for sequential consistency.
 * Arguments: object, expected, desired, weak, success_order, failure_order.
 */
.macro primitive_cmpxchg bits, load, store, acquire, release, reg
.p2align 2
.global __edg_qbe_cmpxchg\bits
.type __edg_qbe_cmpxchg\bits, %function
__edg_qbe_cmpxchg\bits:
.cfi_startproc
    \load \reg\()6, [x1]
.Lcmp_retry\@:
    \acquire \reg\()7, [x0]
    cmp \reg\()7, \reg\()6
    b.ne .Lcmp_failed\@
    \release w8, \reg\()2, [x0]
    cbnz w8, .Lcmp_retry\@
    mov w0, #1
    ret
.Lcmp_failed\@:
    clrex
    \store \reg\()7, [x1]
    mov w0, #0
    ret
.cfi_endproc
.size __edg_qbe_cmpxchg\bits, .-__edg_qbe_cmpxchg\bits
.endm
primitive_cmpxchg 8, ldrb, strb, ldaxrb, stlxrb, w
primitive_cmpxchg 16, ldrh, strh, ldaxrh, stlxrh, w
primitive_cmpxchg 32, ldr, str, ldaxr, stlxr, w
primitive_cmpxchg 64, ldr, str, ldaxr, stlxr, x

/* GCC memory-order values: relaxed=0, consume=1, acquire=2, release=3,
 * acq_rel=4, seq_cst=5.  The upper bits can carry target-specific flags.
 */
.p2align 2
.global __edg_qbe_fence
.type __edg_qbe_fence, %function
__edg_qbe_fence:
.cfi_startproc
    and w0, w0, #0xffff
    cbz w0, .Lfence_done
    cmp w0, #2
    b.ls .Lfence_acquire
    dmb ish
    ret
.Lfence_acquire:
    dmb ishld
.Lfence_done:
    ret
.cfi_endproc
.size __edg_qbe_fence, .-__edg_qbe_fence
.section .note.GNU-stack,"",%progbits
