section .data;
argv:
	DQ	0;
argc:
	DD	0;
section .text;
global main
main:
	EXTERN	malloc;
	EXTERN	free;
	PUSH	RBP;
	MOV	RBP, RSP;
	SUB	RSP, 8;
	MOV	QWORD[argv], RSI;
	MOV	DWORD[argc], EDI;
	POP	QWORD[RBP-8];
	MOV	EAX, 0;
	POP	RBP;
	MOV	RAX, 60;
	MOV	RDI, 0;
	SYSCALL;
