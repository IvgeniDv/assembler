cat: .data 1 , 2 , 3 , -9
dog: .data 1 , 1 , 0 , 1
fish: .string " hello wrold "
plant: .string " cat "
.extern Dog

.entry dog
.entry cat
mov #45 , cat
mov dog , r1
mov dog[r10] , r1
mov dog[r11] , r2
mov dog[r12] , r3
mov dog[r14] , r4
mov dog[r15] , r5
mov r0 , r0
mov r1 , r1
mov r2 , r2
mov r3 , r3
mov r4 , r4
mov r5 , r5
mov r6 , r6
mov r7 , r7
mov r8 , r8
mov r9 , r9
mov r10 , r10
mov r11 , r11
mov r12 , r12
mov r13 , r13
mov r14 , r14
mov r15 , r15
cmp #45 , #45
cmp dog , cat
cmp dog[r10] , cat[r10]
cmp dog[r11] , cat[r11]
cmp dog[r12] , cat[r12]
cmp dog[r13] , cat[r13]
cmp dog[r14] , cat[r14]
cmp dog[r15] , cat[r15]
cmp r0 , r0
cmp r1 , r1
cmp r2 , r2
cmp r3 , r3
cmp r4 , r4
cmp r5 , r5
cmp r6 , r6
cmp r7 , r7
cmp r8 , r8
cmp r9 , r9
cmp r10 , r10
cmp r11 , r11
cmp r12 , r12
cmp r13 , r13
cmp r14 , r14
cmp r15 , r15
add #45 , cat
add dog , cat[r10]
add dog[r10] , cat[r11]
add dog[r11] , cat[r12]
add dog[r12] , cat[r13]
add dog[r14] , cat[r14]
add dog[r15] , cat[r15]
add r0 , r0
add r1 , r1
add r2 , r2
add r3 , r3
add r4 , r4
add r5 , r5
add r6 , r6
add r7 , r7
add r8 , r8
add r9 , r9
add r10 , r10
add r11 , r11
add r12 , r12
add r13 , r13
add r14 , r14
add r15 , r15
sub #45 , cat
sub dog , cat[r10]
sub dog[r10] , cat[r11]
sub dog[r11] , cat[r12]
sub dog[r12] , cat[r13]
sub dog[r14] , cat[r14]
sub dog[r15] , cat[r15]
sub r0 , r0
sub r1 , r1
sub r2 , r2
sub r3 , r3
sub r4 , r4
sub r5 , r5
sub r6 , r6
sub r7 , r7
sub r8 , r8
sub r9 , r9
sub r10 , r10
sub r11 , r11
sub r12 , r12
sub r13 , r13
sub r14 , r14
sub r15 , r15
lea dog , cat
lea dog[r10] , cat[r10]
lea dog[r11] , cat[r11]
lea dog[r12] , cat[r12]
lea dog[r13] , cat[r13]
lea dog[r14] , cat[r14]
lea dog[r15] , cat[r15]
lea cat , r0
lea cat , r2
lea cat , r3
lea cat , r4
lea cat , r5
lea cat , r6
lea cat , r7
lea cat , r8
lea cat , r9
lea cat , r10
lea cat , r11
lea cat , r12
lea cat , r13
lea cat , r14
clr cat
clr cat[r10]
clr cat[r11]
clr cat[r12]
clr cat[r13]
clr cat[r14]
clr cat[r15]
clr r0
clr r1
clr r2
clr r3
clr r4
clr r5
clr r6
clr r7
clr r8
clr r9
clr r10
clr r11
clr r12
clr r13
clr r14
clr r15
not cat
not cat[r10]
not cat[r11]
not cat[r12]
not cat[r13]
not cat[r14]
not cat[r15]
not r0
not r1
not r2
not r3
not r4
not r5
not r6
not r7
not r8
not r9
not r10
not r11
not r12
not r13
not r14
not r15
inc cat
inc cat[r10]
inc cat[r11]
inc cat[r12]
inc cat[r13]
inc cat[r14]
inc cat[r15]
inc r0
inc r1
inc r2
inc r3
inc r4
inc r5
inc r6
inc r7
inc r8
inc r9
inc r10
inc r11
inc r12
inc r13
inc r14
inc r15
dec cat
dec cat[r10]
dec cat[r11]
dec cat[r12]
dec cat[r13]
dec cat[r14]
dec cat[r15]
dec r0
dec r1
dec r2
dec r3
dec r4
dec r5
dec r6
dec r7
dec r8
dec r9
dec r10
dec r11
dec r12
dec r13
dec r14
dec r15
jmp cat
jmp cat[r10]
jmp cat[r11]
jmp cat[r12]
jmp cat[r13]
jmp cat[r14]
jmp cat[r15]
bne cat
bne cat[r10]
bne cat[r11]
bne cat[r12]
bne cat[r13]
bne cat[r14]
bne cat[r15]
jsr cat
jsr cat[r10]
jsr cat[r11]
jsr cat[r12]
jsr cat[r13]
jsr cat[r14]
jsr cat[r15]
red cat
red cat[r10]
red cat[r11]
red cat[r12]
red cat[r13]
red cat[r14]
red cat[r15]
red r0
red r1
red r2
red r3
red r4
red r5
red r6
red r7
red r8
red r9
red r10
red r11
red r12
red r13
red r14
red r15
prn #45
prn cat
prn cat[r10]
prn cat[r11]
prn cat[r12]
prn cat[r13]
prn cat[r14]
prn cat[r15]
prn r0
prn r1
prn r2
prn r3
prn r4
prn r5
prn r6
prn r7
prn r8
prn r9
prn r10
prn r11
prn r12
prn r13
prn r14
Cat: prn r15
rts
stop
.extern cats

