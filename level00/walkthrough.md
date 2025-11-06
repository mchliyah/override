at the start the program print a message level00 and then wait for a password using scanf .

```bash
   0x080484ce <+58>:	mov    $0x8048636,%eax
   0x080484d3 <+63>:	lea    0x1c(%esp),%edx
   0x080484d7 <+67>:	mov    %edx,0x4(%esp)
   0x080484db <+71>:	mov    %eax,(%esp)
   0x080484de <+74>:	call   0x80483d0 <__isoc99_scanf@plt>
```

it stor the value at 0x1c expecting 4 bytes int 

```bash 
(gdb) x/s 0x8048636
0x8048636:	 "%d"
```

the input is compaired to 0x149c (5276)

```bash 
   0x080484e3 <+79>:	mov    0x1c(%esp),%eax
   0x080484e7 <+83>:	cmp    $0x149c,%eax
```

now it just print a message the call system with the "/bin/sh" argument 

```bash
   0x080484ee <+90>:	movl   $0x8048639,(%esp)
   0x080484f5 <+97>:	call   0x8048390 <puts@plt>
   0x080484fa <+102>:	movl   $0x8048649,(%esp)
   0x08048501 <+109>:	call   0x80483a0 <system@plt>
   0x08048506 <+114>:	mov    $0x0,%eax
```

the /bin/sh will be executed as level01 

```bash 
ls -la
-rwsr-s---+ 1 level01 users   7280 Sep 10  2016 level00
``

