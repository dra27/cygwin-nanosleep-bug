# Reproduction for Cygwin nanosleep bug

Requires `gcc` and `make`, simply run `make`. Output should be of the form:

```
dra@Thor ~/nanosleep
$ make
cc    -c -o signal.o signal.c
cc -o signal.exe signal.o
while bash ./signal.run ; do true; done
baabax - Got CTRL+C, exiting
baababx - Got CTRL+C, exiting
abababx - Got CTRL+C, exiting
ababax - Got CTRL+C, exiting
ababax - Got CTRL+C, exiting
abababx - Got CTRL+C, exiting
baabax - Got CTRL+C, exiting
ababax - Got CTRL+C, exiting
abababx - Got CTRL+C, exiting
baababx - Got CTRL+C, exiting
abababx - Got CTRL+C, exiting
baabax - nanosleep failed: 22, t = 0, -143200
ababax - nanosleep failed: 22, t = 0, -35300
baababx - Got CTRL+C, exiting
abababx - Got CTRL+C, exiting
baababx - Got CTRL+C, exiting
baababx - Got CTRL+C, exiting
baababx - Got CTRL+C, exiting
ababax - Got CTRL+C, exiting
```

`signal.c` is a rough C translation of the [`signal.ml`](https://github.com/ocaml/ocaml/blob/trunk/testsuite/tests/lib-threads/signal.ml)
test from OCaml's testsuite.

The test is supposed to have two threads printing as and bs which should be correctly interrupted by a SIGINT after
a couple of seconds.

`nanosleep` is either fed a positive `timespec` or the previous value it returned. It appears that
NtQueryTimer can return a negative number for a non-signalled timer. The amount returned is always
suspiciously at or below the timer resolution.
