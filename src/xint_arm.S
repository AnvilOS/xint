
    .syntax unified
    .cpu cortex-m33
    .fpu softvfp
    .thumb

@xword_t x_mul_1(xword_t *W, const xword_t *U, size_t m, xword_t v, xword_t k)
.global xll_mul_1
xll_mul_1:

    @ r0: W 
    @ r1: U 
    @ r2: Un replaced with U limit
    @ r3: v0
    @ r4: k
    @ r5: u
    @ r6: w

    stmdb	sp!, {r4, r5, r6 }
    subs	r1, #4
    mov	    r4, #0
    add.w	r2, r1, r2, lsl #2
    subs	r0, #4

loop_mul_1:
    ldr.w	r5, [r1, #4]!
    mov     r6, #0 
    umaal   r6, r4, r3, r5
    cmp	    r2, r1
    str.w	r6, [r0, #4]!
    bne.n   loop_mul_1

    mov     r0, r4
    ldmia.w	sp!, {r4, r5, r6 }
    bx      lr 

@xword_t x_mul_add_1(xword_t *W, const xword_t *U, size_t m, xword_t v)
.global xll_mul_add_1
xll_mul_add_1:

    @ r0: W 
    @ r1: U 
    @ r2: Un replaced with U limit
    @ r3: v0
    @ r4: k
    @ r5: u
    @ r6: w

    stmdb	sp!, {r4, r5, r6 }
    subs	r1, #4
    mov	    r4, #0
    add.w	r2, r1, r2, lsl #2
    subs	r0, #4

loop_mul_add_1:
    ldr.w	r5, [r1, #4]!
    ldr.w	r6, [r0, #4]!
    umaal   r6, r4, r3, r5
    cmp	    r2, r1
    str.w	r6, [r0]
    bne.n   loop_mul_add_1

    mov     r0, r4
    ldmia.w	sp!, {r4, r5, r6 }
    bx      lr

@xword_t xll_mul_add_2(xword_t *W, const xword_t *U, size_t m, xword_t v1, xword_t v0)
.global xll_mul_add_2
xll_mul_add_2:

    @ r0: W 
    @ r1: U 
    @ r2: Un replaced with U limit
    @ r3: v1
    @ r4: v0
    @ r5: u
    @ r6: w
    @ r7: carry 0
    @ lr: carry 1

    stmdb	sp!, { r4, r5, r6, r7, lr }
    ldr.w   r4, [sp, #20]
    mov     r7, #0
    mov     lr, #0
    subs    r1, #4
    add.w	r2, r1, r2, lsl #2
    subs	r0, #4

loop_mul_add_2:
    ldr.w	r5, [r1, #4]!
    ldr.w	r6, [r0, #4]!
    umaal   r6, r7, r4, r5
    umaal   r7, lr, r3, r5
    cmp	    r2, r1
    str.w	r6, [r0]
    bne.n   loop_mul_add_2
    
    str.w	r7, [r0, #4]
    mov     r0, lr
    ldmia.w	sp!, {r4, r5, r6, r7, pc }


@xword_t xll_mul_add_4(xword_t *W, const xword_t *U, size_t m, xword_t *V)
.global xll_mul_add_4
xll_mul_add_4:

    @ r0: W 
    @ r1: U 
    @ r2: Un replaced with U limit
    @ r3: V replaced with v3
    @ r4: v2
    @ r5: v1
    @ r6: v0
    @ r7: u
    @ r8: w
    @ r9: k3
    @ r10: k2
    @ r11: k1
    @ lr: k0

    stmdb	sp!, { r4, r5, r6, r7, r8, r9, r10, r11, lr }
    
    ldr.w   r6, [r3, #0]
    ldr.w   r5, [r3, #4]
    ldr.w   r4, [r3, #8]
    ldr.w   r3, [r3, #12]

    mov     r9, #0
    mov     r10,r9
    mov     r11, r9
    mov     lr, r9

    subs    r1, #4
    add.w	r2, r1, r2, lsl #2
    subs	r0, #4

loop_mul_add_4:
    ldr.w	r7, [r1, #4]!
    ldr.w	r8, [r0, #4]!

    umaal   r8, lr, r6, r7
    umaal   lr, r11, r5, r7
    umaal   r11, r10, r4, r7
    umaal   r10, r9, r3, r7

    cmp	    r2, r1
    str.w	r8, [r0]
    bne.n   loop_mul_add_4
    
    str.w	lr, [r0, #4]
    str.w	r11, [r0, #8]
    str.w	r10, [r0, #12]
    mov     r0, r9
    ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc }


@xword_t xll_sub(xword_t *W, const xword_t *U, const xword_t *V, size_t n)
.global xll_sub
xll_sub:

    @ r0: W 
    @ r1: U
    @ r2: V
    @ r3: Un replaced with U limit

    stmdb	sp!, { r4, r5 }
    subs	r1, #4
    subs	r2, #4
    subs	r0, #4
    add.w	r3, r1, r3, lsl #2

    cmp     r0, r0

loop_xll_sub:
    ldr.w	r4, [r1, #4]!
    ldr.w	r5, [r2, #4]!
    sbcs    r5, r4, r5
    teq	    r3, r1
    str.w	r5, [r0, #4]!
    bne.n   loop_xll_sub

    sbc	    r0, r0, r0
    and	    r0, r0, #1
    ldmia.w	sp!, { r4, r5 }
    bx      lr 

@xword_t xll_sub_1(xword_t *W, const xword_t *U, xword_t v, size_t n)
.global xll_sub_1
xll_sub_1:

    @ r0: W 
    @ r1: U
    @ r2: v
    @ r3: Un replaced with U limit

    stmdb	sp!, { r4 }
    subs	r1, #4
    subs	r0, #4
    add.w	r3, r1, r3, lsl #2

    cmp     r0, r0

    ldr.w	r4, [r1, #4]!
    sbcs    r4, r4, r2
    str.w	r4, [r0, #4]!

    teq	    r3, r1
    beq.n   done_xll_sub_1

loop_xll_sub_1:
    ldr.w	r4, [r1, #4]!
    sbcs    r4, r4, $0
    teq	    r3, r1
    str.w	r4, [r0, #4]!
    bne.n   loop_xll_sub_1

done_xll_sub_1:
    sbc	    r0, r0, r0
    and	    r0, r0, #1
    ldmia.w	sp!, { r4 }
    bx      lr 
