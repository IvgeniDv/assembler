; file ps.as
.entry LIST
.extern W
m1
m2
m3
MAIN:	add	r3,LIST
LOOP:	prn	#48
	macro m1
	     inc r6
	     mov r3,W
	endm
	lea	STR, r6
	m1
	sub	r1, r4
	bne	END
macro m2
	stop
endm

macro m3
	mov #45 , r10
	mov r3, W
endm
	cmp	vall,#-6
	bne	END[r15]
	dec	K
.entry MAIN
	sub	LOOP[r10],r14
END:	stop
STR:	.string "abcd"
LIST:	.data	6,-9
	.data	-100
m2

.entry K
K:	.data 31
m3
m3
.extern vall 
