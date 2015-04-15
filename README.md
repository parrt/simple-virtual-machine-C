An extension of the [(split-stack branch) simple VM](https://github.com/parrt/simple-virtual-machine/tree/split-stack) I did that [codyebberson converted to C](https://github.com/codyebberson/vm). I added CALL/RET and make VM a `struct`.  Added more tests.

Branches:

* [master](https://github.com/parrt/simple-virtual-machine-C/tree/master). This is a straight C port of the [split-stack branch from Java](https://github.com/parrt/simple-virtual-machine/tree/split-stack).
* [computed-goto](https://github.com/parrt/simple-virtual-machine-C/tree/computed-goto). A version of the `exec()` method that uses computed `goto`s to directly jump from instruction to instruction like a threaded interpreter, thus, avoiding a `while` loop and `switch` for the *decode* part of the *fetch-decode-execute* cycle. Instead of:
```C
while ( more opcodes ) {
  switch ( code[ip] ) {
  ...
```
we use `goto *dispatch[code[ip]]` to instantly jump to the code that interprets the next opcode via an appropriate label within the `exec` method.

Build and run:

```bash
$ cc -o vm src/*.c
$ vm
```
