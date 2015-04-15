An extension of the [(split-stack branch) simple VM](https://github.com/parrt/simple-virtual-machine/tree/split-stack) I did that [codyebberson converted to C](https://github.com/codyebberson/vm). I added CALL/RET and make VM a `struct`.  Added more tests.

Build and run:

```bash
$ cc -o vm src/*.c
$ vm
```
