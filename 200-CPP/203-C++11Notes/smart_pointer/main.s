	.file	"main.c"
	.option pic
	.text
	.section	.rodata
	.align	3
.LC0:
	.string	"*p = %d\n"
	.text
	.align	1
	.globl	main
	.type	main, @function
main:
	addi	sp,sp,-48
	sd	ra,40(sp)
	sd	s0,32(sp)
	addi	s0,sp,48
	la	a5,__stack_chk_guard
	ld	a4, 0(a5)
	sd	a4, -24(s0)
	li	a4, 0
	li	a5,10
	sw	a5,-36(s0)
	addi	a5,s0,-36
	sd	a5,-32(s0)
	# 加载局部变量
	ld	a5,-32(s0) # 从相对于s0偏移量为-32的位置加载局部变量的值
	lw	a5,0(a5)   # 将加载的值存储在a5中
	mv	a1,a5      # 将a5的值移动到a1作为参数
	lla	a0,.LC0
	call	printf@plt # 调用printf函数
	li	a5,0
	mv	a4,a5
	la	a5,__stack_chk_guard
	ld	a3, -24(s0)
	ld	a5, 0(a5)
	xor	a5, a3, a5
	li	a3, 0
	beq	a5,zero,.L3
	call	__stack_chk_fail@plt
.L3:
	mv	a0,a4
	ld	ra,40(sp)
	ld	s0,32(sp)
	addi	sp,sp,48
	jr	ra
	.size	main, .-main
	.ident	"GCC: (Ubuntu 11.3.0-1ubuntu1~22.04.1) 11.3.0"
	.section	.note.GNU-stack,"",@progbits
