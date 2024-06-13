.globl foo
foo:
    pushq %rbp
    movq %rsp, %rbp
    subq $32, %rsp
    movl $1, -8(%rsp)
    movl $0, -16(%rsp)
L2:
    movl $1, %edi
    movl $1, %esi
L1:
    cmpl $0, -8(%rsp)
    cmpl $1, -16(%rsp)
    cmpl $2, -24(%rsp)
    cmpl $200, -32(%rsp)
    cmpl $16, %edi
    cmpl $200, %esi
    jmp L1
	ret
