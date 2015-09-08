/* See COPYRIGHT for copyright information. */

#include <inc/mmu.h>
#include <inc/memlayout.h>
#include <inc/trap.h>

#include <kern/picirq.h>


###################################################################
# exceptions/interrupts
###################################################################

/* TRAPHANDLER defines a globally-visible function for handling a trap.
 * It pushes a trap number onto the stack, then jumps to _alltraps.
 * Use TRAPHANDLER for traps where the CPU automatically pushes an error code.
 *
 * You shouldn't call a TRAPHANDLER function from C, but you may
 * need to _declare_ one in C (for instance, to get a function pointer
 * during IDT setup).  You can declare the function with
 *   void NAME();
 * where NAME is the argument passed to TRAPHANDLER.
 */
#define TRAPHANDLER(name, num)						\
	.globl name;		/* define global symbol for 'name' */	\
	.type name, @function;	/* symbol type is function */		\
	.align 2;		/* align function definition */		\
	name:			/* function starts here */		\
	pushl $(num);							\
	jmp _alltraps

/* Use TRAPHANDLER_NOEC for traps where the CPU doesn't push an error code.
 * It pushes a 0 in place of the error code, so the trap frame has the same
 * format in either case.
 */
#define TRAPHANDLER_NOEC(name, num)					\
	.globl name;							\
	.type name, @function;						\
	.align 2;							\
	name:								\
	pushl $0;							\
	pushl $(num);							\
	jmp _alltraps

.text

/*
 * Lab 3: Your code here for generating entry points for the different traps.
 */
TRAPHANDLER_NOEC(fun_divide, 0)
TRAPHANDLER_NOEC(fun_debug, 1)
TRAPHANDLER_NOEC(fun_nmi, 2)
TRAPHANDLER_NOEC(fun_brkpt, 3)
TRAPHANDLER_NOEC(fun_oflow, 4)
TRAPHANDLER_NOEC(fun_bound, 5)
TRAPHANDLER_NOEC(fun_illop, 6)
TRAPHANDLER_NOEC(fun_device, 7)
TRAPHANDLER(fun_dblflt, 8)
TRAPHANDLER(fun_tss, 10)
TRAPHANDLER(fun_segnp, 11)
TRAPHANDLER(fun_stack, 12)
TRAPHANDLER(fun_gpflt, 13)
TRAPHANDLER(fun_pgflt, 14)
TRAPHANDLER_NOEC(fun_fperr, 16)
TRAPHANDLER(fun_align, 17)
TRAPHANDLER_NOEC(fun_mchk, 18)
TRAPHANDLER_NOEC(fun_simderr, 19)
TRAPHANDLER_NOEC(fun_syscall, T_SYSCALL)

TRAPHANDLER_NOEC(fun_irq0, IRQ_OFFSET+0)
TRAPHANDLER_NOEC(fun_irq1, IRQ_OFFSET+1)
TRAPHANDLER_NOEC(fun_irq2, IRQ_OFFSET+2)
TRAPHANDLER_NOEC(fun_irq3, IRQ_OFFSET+3)
TRAPHANDLER_NOEC(fun_irq4, IRQ_OFFSET+4)
TRAPHANDLER_NOEC(fun_irq5, IRQ_OFFSET+5)
TRAPHANDLER_NOEC(fun_irq6, IRQ_OFFSET+6)
TRAPHANDLER_NOEC(fun_irq7, IRQ_OFFSET+7)
TRAPHANDLER_NOEC(fun_irq8, IRQ_OFFSET+8)
TRAPHANDLER_NOEC(fun_irq9, IRQ_OFFSET+9)
TRAPHANDLER_NOEC(fun_irq10, IRQ_OFFSET+10)
TRAPHANDLER_NOEC(fun_irq11, IRQ_OFFSET+11)
TRAPHANDLER_NOEC(fun_irq12, IRQ_OFFSET+12)
TRAPHANDLER_NOEC(fun_irq13, IRQ_OFFSET+13)
TRAPHANDLER_NOEC(fun_irq14, IRQ_OFFSET+14)
TRAPHANDLER_NOEC(fun_irq15, IRQ_OFFSET+15)


/*
 * Lab 3: Your code here for _alltraps
 */
_alltraps:
	pushl %ds
	pushl %es
	pushal
	movw $GD_KD, %ax
	movw %ax, %ds
	movw %ax, %es
	pushl %esp
	call trap

