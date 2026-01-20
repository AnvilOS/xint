
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
    cmp	    r2, r1
    umaal   lr, r11, r5, r7
    umaal   r11, r10, r4, r7
    umaal   r10, r9, r3, r7

    str.w	r8, [r0]
    bne.n   loop_mul_add_4
    
    str.w	lr, [r0, #4]
    str.w	r11, [r0, #8]
    str.w	r10, [r0, #12]
    mov     r0, r9
    ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc }


@xword_t xll_add(xword_t *W, const xword_t *U, const xword_t *V, size_t n)
.global xll_add_asm
.align 4
xll_add_asm:

    @ r0: W 
    @ r1: U
    @ r2: V
    @ r3: Un then U limit

    stmdb	sp!, { r4, r5, r6, r7 }
    cmn     r0, #0
    tst     r3, #1
    add     r3, r1, r3, lsl #2
    beq.n   xll_add_loop

    ldr.w   r4, [r1], #4
    ldr.w   r5, [r2], #4
    adcs    r5, r4, r5
    teq	    r3, r1
    str.w   r5, [r0], #4
    bne.n   xll_add_loop
    b       xll_add_done

.align 4
xll_add_loop:
    ldmia   r1!, { r4, r6 }
    ldmia   r2!, { r5, r7 }
    adcs    r5, r4, r5
    adcs    r7, r6, r7
    teq	    r3, r1
    stmia   r0!, { r5, r7 }
    bne.n   xll_add_loop

xll_add_done:
    mov     r0, #0
    adc	    r0, r0, #0
    ldmia.w	sp!, { r4, r5, r6, r7 }
    bx      lr 

@xword_t xll_add_1(xword_t *W, const xword_t *U, const xword_t v, size_t n)
.global xll_add_1_asm
.align 4
xll_add_1_asm:

    @ r0: W 
    @ r1: U
    @ r2: v
    @ r3: Un then U limit

    stmdb	sp!, { r4, r5 }
    cmn     r0, #0
    sub     r12, r3, #1
    add     r3, r1, r3, lsl #2

    // Add in tne v first
    ldr.w   r4, [r1], #4
    adcs    r4, r4, r2
    mov     r2, #0
    str.w   r4, [r0], #4
    teq	    r3, r1
    beq.n   xll_add_1_done

    tst     r12, #1
    beq.n   xll_add_1_loop

    ldr.w   r4, [r1], #4
    adcs    r4, r4, r2
    teq	    r3, r1
    str.w   r4, [r0], #4
    bne.n   xll_add_1_loop
    b       xll_add_1_done

.align 4
xll_add_1_loop:
    ldmia   r1!, { r4, r5 }
    adcs    r4, r4, r2
    adcs    r5, r5, r2
    teq	    r3, r1
    stmia   r0!, { r4, r5 }
    bne.n   xll_add_1_loop

xll_add_1_done:
    mov     r0, r2
    adc	    r0, r0, r2
    ldmia.w	sp!, { r4, r5 }
    bx      lr 

@xword_t xll_sub(xword_t *W, const xword_t *U, const xword_t *V, size_t n)
.global xll_sub_asm
.align 4
xll_sub_asm:

    @ r0: W 
    @ r1: U
    @ r2: V
    @ r3: Un then U limit

    stmdb	sp!, { r4, r5, r6, r7 }
    cmp     r0, r0
    tst     r3, #1
    add     r3, r1, r3, lsl #2
    beq.n   xll_sub_loop

    ldr.w   r4, [r1], #4
    ldr.w   r5, [r2], #4
    sbcs    r4, r4, r5
    teq	    r3, r1
    str.w   r4, [r0], #4
    bne.n   xll_sub_loop
    b       xll_sub_done

.align 4
xll_sub_loop:
    ldmia   r1!, { r4, r6 }
    ldmia   r2!, { r5, r7 }
    sbcs    r4, r4, r5
    sbcs    r6, r6, r7
    stmia   r0!, { r4, r6 }
    teq	    r3, r1
    bne.n   xll_sub_loop

xll_sub_done:
    sbcs    r0, r0, r0
    and	    r0, r0, #1
    ldmia.w	sp!, { r4, r5, r6, r7 }
    bx      lr 

@xword_t xll_sub_1(xword_t *W, const xword_t *U, xword_t v, size_t n)
.global xll_sub_1_asm
.align 4
xll_sub_1_asm:

    @ r0: W 
    @ r1: U
    @ r2: v
    @ r3: Un replaced with U limit

    stmdb	sp!, { r4, r5 }
    cmp     r0, r0
    sub     r12, r3, #1
    add     r3, r1, r3, lsl #2

    // Sub tne v first
    ldr.w   r4, [r1], #4
    sbcs    r4, r4, r2
    mov     r2, #0
    str.w   r4, [r0], #4
    teq	    r3, r1
    beq.n   xll_sub_1_done

    tst     r12, #1
    beq.n   xll_sub_1_loop

    ldr.w   r4, [r1], #4
    sbcs    r4, r4, r2
    teq	    r3, r1
    str.w   r4, [r0], #4
    bne.n   xll_sub_1_loop
    b       xll_sub_1_done

.align 4
xll_sub_1_loop:
    ldmia   r1!, { r4, r5 }
    sbcs    r4, r4, r2
    sbcs    r5, r5, r2
    teq	    r3, r1
    stmia   r0!, { r4, r5 }
    bne.n   xll_sub_1_loop

xll_sub_1_done:
    sbcs    r0, r0, r0
    and	    r0, r0, #1
    ldmia.w	sp!, { r4, r5 }
    bx      lr 

@void xll_mul_algm(xword_t *W, const xword_t *U, size_t m, const xword_t *V, size_t n)
.global xll_mul_asm
xll_mul_asm:
    @ r0: W
    @ r1: U
    @ r2: Ulim
    @ r3: tmp
    @ r4: u
    @ r5: w
    @ r6: v3
    @ r7: v2
    @ r8: v1
    @ r9: v0
    @ r10: k3
    @ r11: k2
    @ r12: k1
    @ lr:  k0
    @ sp+52: Vn
    @ sp+0: Un
    @ sp+4: V
    @ sp+8: Vlim 

#define pW r0
#define pU r1
#define Ulim r2
#define tmp r3
#define uu r4
#define ww r5
#define v0 r6
#define v1 r7
#define v2 r8
#define v3 r9
#define k0 r10
#define k1 r11
#define k2 r12
#define k3 lr
#define Vn [sp, #48]
#define Un [sp, #0]
#define pV [sp, #4]
#define Vlim [sp, #8]

    stmdb	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr }
    subs    sp, sp, #12

    // Vlim - use k0 as a 2nd tmp
    str.w   r3, pV
    ldr.w   k0, Vn
    add.w	tmp, tmp, k0, lsl #2
    str.w   tmp, Vlim

    // Ulim
    str.w   r2, Un
    add.w	Ulim, pU, r2, lsl #2

    // k0 still contains Vn
    tst     k0, #3
    beq.n   m4
    and     k0, k0, #3
    cmp     k0, #2
    beq.n   m2
    cmp     k0, #3
    beq.n   m3

m1:
    // Load v0
    ldr.w   tmp, pV
    ldr.w	v0, [tmp], #4
    str.w	tmp, pV
    // Load k0
    mov     k0, #0

loop_mul1:
    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    cmp	    Ulim, pU
    str.w	ww, [pW], #4
    bne.n   loop_mul1
    str.w	k0, [pW], #4
    b       xll_mul_ma4_outer

m2:
    // Load v0, v1
    ldr.w   tmp, pV
    ldmia   tmp!, { v0, v1 }
    str.w	tmp, pV
    // Load k0, k1
    mov     k0, #0
    mov     k1, k0

loop_mul2:
    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    cmp	    Ulim, pU
    str.w	ww, [pW], #4
    bne.n   loop_mul2
    stmia   pW!, { k0, k1 }
    b       xll_mul_ma4_outer

m3:
    // Load v0, v1, v2
    ldr.w   tmp, pV
    ldmia   tmp!, { v0, v1, v2 }
    str.w	tmp, pV
    // Load k0, k1, k2
    mov     k0, #0
    mov     k1, k0
    mov     k2, k0

loop_mul3:
    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    umaal   k1, k2, uu, v2
    cmp	    Ulim, pU
    str.w	ww, [pW], #4
    bne.n   loop_mul3
    stmia   pW!, { k0, k1, k2 }
    b       xll_mul_ma4_outer

m4:
    // Load v0, v1, v2, v3
    ldr.w   tmp, pV
    ldmia   tmp!, { v0, v1, v2, v3 }
    str.w	tmp, pV

    ldr.w   tmp, Un

    // Load k0, k1, k2, k3
    mov     k0, #0
    mov     k1, k0
    mov     k2, k0
    mov     k3, k0

    tst     tmp, #1

    beq.n   xll_mul_m4_loop
    b       xll_mul_m4_loop_mid

.align 4
xll_mul_m4_loop:
    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    umaal   k1, k2, uu, v2
    umaal   k2, k3, uu, v3
    str.w	ww, [pW], #4
xll_mul_m4_loop_mid:
    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    umaal   k1, k2, uu, v2
    umaal   k2, k3, uu, v3
    str.w	ww, [pW], #4
    cmp	    pU, Ulim
    blt.n   xll_mul_m4_loop
    stmia   pW!, { k0, k1, k2, k3 }
    b       xll_mul_ma4_outer

.align 4
xll_mul_ma4_outer:
    // Check whether we've reached Vlim, use k0 as a temp
    ldr.w   k0, pV
    ldr.w   tmp, Vlim
    cmp	    k0, tmp
    beq     xll_mul_done

    // Load v0, v1, v2, v3
    ldmia   k0!, { v0, v1, v2, v3 }
    str.w	k0, pV

    // Move W and U back Un places
    ldr.w   tmp, Un
    subs    pW, pW, tmp, lsl #2
    subs    pU, pU, tmp, lsl #2

    tst     tmp, #1

    // Set k0, k1, k2, k3 to 0
    mov     k0, #0
    mov     k1, k0
    mov     k2, k0
    mov     k3, k0

    beq.n   xll_mul_ma4_loop
    b       xll_mul_ma4_loop_mid

.align 4
xll_mul_ma4_loop:
    ldr.w	uu, [pU], #4
    ldr.w	ww, [pW]
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    umaal   k1, k2, uu, v2
    umaal   k2, k3, uu, v3
    str.w	ww, [pW], #4
    
xll_mul_ma4_loop_mid:
    ldr.w	uu, [pU], #4
    ldr.w	tmp, [pW]
    umaal   tmp, k0, uu, v0
    umaal   k0, k1, uu, v1
    umaal   k1, k2, uu, v2
    umaal   k2, k3, uu, v3
    str.w	tmp, [pW], #4
    
    cmp	    pU, Ulim
    blt.n   xll_mul_ma4_loop
 
    stmia   pW!, { k0, k1, k2, k3 }
    b       xll_mul_ma4_outer

xll_mul_done:
    adds    sp, sp, #12
    ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc }

#undef pW
#undef pU
#undef Ulim
#undef tmp
#undef uu
#undef ww
#undef v0
#undef v1
#undef v2
#undef v3
#undef k0
#undef k1
#undef k2
#undef k3
#undef Vn
#undef Un
#undef pV
#undef Vlim

@void xll_squ_asm(xword_t *W, const xword_t *U, int Un);
.global xll_squ_asm
xll_squ_asm:
    @ r0: W
    @ r1: U
    @ r2: Un
    @ r3: V
    @ r4: Vn
    @ r5: w
    @ r6: u
    @ r7: v
    @ r8: k
    @ r9: Ulim
    @ r10: Vlim
#define pW r0
#define pU r1
#define Ulim r2
#define tmp r3
#define uu r4
#define ww r5
#define v0 r6
#define v1 r7
#define v2 r8
#define v3 r9
#define k0 r10
#define k1 r11
#define k2 r12
#define k3 lr
#define Vn [sp, #12]
#define Un [sp, #0]
#define pV [sp, #4]
#define Vlim [sp, #8]

    stmdb	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, lr }
    subs    sp, sp, #16

    // W[0] = 0;
    mov     r4, #0
    str.w	r4, [r0], #4

    mov     r3, pU
    str.w   r3, pV
    add     pU, pU, #4
    str.w   r2, Vn

    // Set Ulim
    subs    r2, r2, #1
    str.w   r2, Un
    add.w	Ulim, pU, r2, lsl #2
    
    mov     tmp, Ulim
    subs    tmp, tmp, #4
    str.W   tmp, Vlim

    // k0 still contains Vn
    ldr.w   tmp, Vn
    subs    tmp, tmp, #1
    tst     tmp, #3
    beq.n   sq_m4
    and     tmp, tmp, #3
    cmp     tmp, #2
    beq.n   sq_m2
    cmp     tmp, #3
    beq.n   sq_m3

sq_m1:
    // Load v0
    ldr.w   tmp, pV
    ldr.w   v0, [tmp], #4
    str.w   tmp, pV
    // Load k0
    mov     k0, #0

sq_loop_mul1:
    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    cmp	    Ulim, pU
    str.w	ww, [pW], #4
    bne.n   sq_loop_mul1

    str.w	k0, [pW], #4

sq_m2:
    b       sq_xll_mul_ma4_outer



sq_m3:
    // Load v0, v1, v2
    ldr.w   tmp, pV
    ldmia   tmp!, { v0, v1, v2 }
    str.w	tmp, pV
    // Load k0, k1, k2
    mov     k0, #0
    mov     k1, k0
    mov     k2, k0

    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    str.w	ww, [pW], #4

    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    str.w	ww, [pW], #4

    b.n     sq_loop_mul3

.align 4
sq_loop_mul3:
    ldr.w	uu, [pU], #4
    mov     ww, #0 
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    umaal   k1, k2, uu, v2
    cmp	    Ulim, pU
    str.w	ww, [pW], #4
    bne.n   sq_loop_mul3
    stmia   pW!, { k0, k1, k2 }

    ldr.w   tmp, Un
    sub     tmp, tmp, #2
    str.w   tmp, Un

    b       sq_xll_mul_ma4_outer


sq_m4:
    b       sq_xll_mul_ma4_outer

sq_xll_mul_ma4_outer:
    // Check whether we've reached Vlim, use k0 as a temp
    ldr.w   k0, pV
    ldr.w   tmp, Vlim
    cmp	    k0, tmp

    beq     sq_xll_mul_done

    // Move W and U back Un places
    ldr.w   tmp, Un
    sub     tmp, tmp, #1
    subs    pW, pW, tmp, lsl #2
    subs    pU, pU, tmp, lsl #2
    str.w   tmp, Un

    // Load v0, v1, v2, v3
    ldr.w   tmp, pV
    ldmia   tmp!, { v0, v1, v2, v3 }
    str.w   tmp, pV

    // Set k0, k1, k2, k3 to 0
    mov     k0, #0
    mov     k1, k0
    mov     k2, k0
    mov     k3, k0

    ldr.w	uu, [pU], #4
    ldr.w   ww, [pW]
    umaal   ww, k0, uu, v0
    str.w	ww, [pW], #4

    ldr.w	uu, [pU], #4
    ldr.w   ww, [pW]
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    str.w	ww, [pW], #4

    ldr.w	uu, [pU], #4
    ldr.w   ww, [pW]
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    umaal   k1, k2, uu, v2
    cmp	    Ulim, pU
    str.w	ww, [pW], #4

    b       sq_xll_mul_ma4_loop

.align 4
sq_xll_mul_ma4_loop:
    ldr.w	uu, [pU], #4
    ldr.w   ww, [pW]
    umaal   ww, k0, uu, v0
    umaal   k0, k1, uu, v1
    umaal   k1, k2, uu, v2
    umaal   k2, k3, uu, v3
    cmp	    Ulim, pU
    str.w	ww, [pW], #4
    blt.n   sq_xll_mul_ma4_loop

    stmia   pW!, { k0, k1, k2, k3 }

    b       sq_xll_mul_ma4_outer

sq_xll_mul_done:
    // W[2*Un-1] = 0;
    mov     r12, #0
    str.w	r12, [pW], #4

// Square loop
#define ww0 r5
#define ww1 r11
#define t1 r12
#define t0 lr

    // Move W and U back Un places
    ldr.w   tmp, Vn
    subs    pW, pW, tmp, lsl #3
    subs    pU, pU, tmp, lsl #2

    // Set Ulim again
    add.w	Ulim, pW, tmp, lsl #3

    mov     k0, #0

    b       sq_xll_sq_loop

.align 4
sq_xll_sq_loop:
    ldmia   pW, { ww0, ww1 }
    ldr.w	uu, [pU], #4
    // Square u
    mov     t0, k0
    mov     t1, #0
    umaal   t0, t1, uu, uu
    // Left shift w
    adds    ww0, ww0, ww0
    adcs    ww1, ww1, ww1
    mov     k0, #0
    adc	    k0, k0, #0
    // u^2 + 2w
    adds    ww0, ww0, t0
    adcs    ww1, ww1, t1
    adcs    k0, k0, #0
    stmia   pW!, { ww0, ww1 }
    teq	    Ulim, pW
    bne.n   sq_xll_sq_loop

    adds    sp, sp, #16
    ldmia.w	sp!, {r4, r5, r6, r7, r8, r9, r10, r11, pc }

#undef pW
#undef pU
#undef Ulim
#undef tmp
#undef uu
#undef ww
#undef v0
#undef v1
#undef v2
#undef v3
#undef k0
#undef k1
#undef k2
#undef k3
#undef Vn
#undef Un
#undef pV
#undef Vlim

