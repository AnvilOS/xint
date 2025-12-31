
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

@ @xword_t x_mul_add_1(xword_t *W, const xword_t *U, size_t m, xword_t v)
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
//    subs    r1, #4
    add.w	r2, r1, r2, lsl #2
    sub     r2, r2, #4
//    subs	r0, #4

loop_mul_add_2:
//    ldr.w	r5, [r1, #4]!
//    ldr.w	r6, [r0, #4]!
    ldr.w	r5, [r1], #4
    ldr.w	r6, [r0]
    umaal   r6, r7, r4, r5
    umaal   r7, lr, r3, r5
    str.w	r6, [r0], #4
    cmp	    r2, r1
//    str.w	r6, [r0]
    bne.n   loop_mul_add_2
    
//    str.w	r7, [r0, #4]
    str.w	r7, [r0], #4
    mov     r0, lr
    ldmia.w	sp!, {r4, r5, r6, r7, pc }
