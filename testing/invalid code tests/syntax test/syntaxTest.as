.entry cat
;number in immediate addressing is too larg
mov #32768, cat

;number in immediate addressing is too small
mov #-32800, cat

; incorrect amount of arguments
mov
mov #45
mov #45, cat , cat

cmp
cmp #45
cmp #45, cat , cat

add
add #45
add #45, cat , cat

sub
sub #45
sub #45, cat , cat

lea
lea cat
lea cat, cat , cat

clr
clr cat, cat

not
not cat, cat

inc
inc cat, cat

dec
dec cat, cat

jmp
jmp cat, cat

bne
bne cat, cat

jsr
jsr cat, cat

red
red cat, cat

prn
prn cat, cat

stop cat

rts cat


; incorrect argument syntax

mov 45, ;cat
mov -+45, 12Cat*
mov --45, ##45
mov mov , cat
mov r5 , cat
mov cat[4] , cat
mov #45 . cat 
mov #45, cat ,

.string : " cat "
.string "cat" hello world"
.string " cat
.string cat"

; label tests

cat: 
cat : stop
#45Cat: stop

dog: .extern fish
mouse: .entry plant



