# CompilerProject4 : Code Generator



#### Testing Code (C)

```c
int a = 11+99;
int i;



int main(){
    int a = 9900-i*2/3;
    int x;
    
    // step into another scope
    if(a <= 998){
        int b = a;
    }
    else{
         a = a + 1;
    }
    
    int b = 12;
    
    while(b<20){
        b = b + 1;
    }
    
    return i+1;
}

```



#### Intermediate Code 

```html
( 1, 11, NONE, temp0)
( 1, 99, NONE, temp1)
( 24, temp0, temp1, temp2)
( 3, temp2, NONE, a-2)
( 3, 0, NONE, i-2)
( 4, NONE, NONE, main)
( 1, 9900, NONE, temp3)
( 2, i-2, NONE, temp4)
( 1, 2, NONE, temp5)
( 26, temp4, temp5, temp6)
( 1, 3, NONE, temp7)
( 36, temp6, temp7, temp8)
( 25, temp3, temp8, temp9)
( 3, temp9, NONE, a-37)
( 3, 0, NONE, x-37)
( 2, a-37, NONE, temp10)
( 1, 998, NONE, temp11)
( 31, temp10, temp11, IF-80)
( 5, NONE, NONE, ELSE-110)
( 4, NONE, NONE, IF-80)
( 2, a-37, NONE, temp13)
( 3, temp13, NONE, b-92)
( 5, NONE, NONE, EXIT-80)
( 4, NONE, NONE, ELSE-110)
( 2, a-37, NONE, temp14)
( 1, 1, NONE, temp15)
( 24, temp14, temp15, temp16)
( 3, temp16, NONE, a-37)
( 4, NONE, NONE, EXIT-80)
( 1, 12, NONE, temp17)
( 3, temp17, NONE, b-37)
( 4, NONE, NONE, WHILE-145)
( 2, b-37, NONE, temp18)
( 1, 20, NONE, temp19)
( 29, temp18, temp19, WHILE_BLOCK-145)
( 5, NONE, NONE, EXIT-145)
( 4, NONE, NONE, WHILE_BLOCK-145)
( 2, b-37, NONE, temp21)
( 1, 1, NONE, temp22)
( 24, temp21, temp22, temp23)
( 3, temp23, NONE, b-37)
( 5, NONE, NONE, WHILE-145)
( 4, NONE, NONE, EXIT-145)
( 2, i-2, NONE, temp24)
( 1, 1, NONE, temp25)
( 24, temp24, temp25, temp26)
( 37, NONE, NONE, temp26)
```



#### Final Code （Mips）

```assembly
li $t0 11
li $t1 99
add $t0 $t0 $t1
sw $t0 0($sp)
sw $zero -4($sp)

main:
li $t0 9900
lw $t1 -4($sp)
li $t2 2
mul $t1 $t2
mflo $t1
li $t2 3
div $t1 $t2
mflo $t1
sub $t0 $t0 $t1
sw $t0 -8($sp)
sw $zero -12($sp)
lw $t0 -8($sp)
li $t1 998
ble $t0 $t1 IF_80
j ELSE_110

IF_80:
lw $t0 -8($sp)
sw $t0 -16($sp)
j EXIT_80

ELSE_110:
lw $t0 -8($sp)
li $t1 1
add $t0 $t0 $t1
sw $t0 -8($sp)

EXIT_80:
li $t0 12
sw $t0 -16($sp)

WHILE_145:
lw $t0 -16($sp)
li $t1 20
blt $t0 $t1 WHILE_BLOCK_145
j EXIT_145

WHILE_BLOCK_145:
lw $t0 -16($sp)
li $t1 1
add $t0 $t0 $t1
sw $t0 -16($sp)
j WHILE_145

EXIT_145:
lw $t0 -4($sp)
li $t1 1
add $t0 $t0 $t1
move $v0 $t0
jr $ra
```

