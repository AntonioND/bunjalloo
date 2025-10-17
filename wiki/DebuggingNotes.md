# Debugging Notes

## GDB and Desmume

```
$ desmume-cli --arm9gdb=55555 file.nds
```

```
$ $DEVKITARM/bin/arm-eabi-gdb file-arm9.elf
```

```
(gdb) target remote localhost:55555
Remote debugging using localhost:55555 0x02000000 in _start ()
(gdb) b main
```

Create a file "gdb.script" that contains this:

```
b main
target remote localhost:55555
c
```

Then run the file as follows:

```
$ $DEVKITARM/bin/arm-eabi-gdb file-arm9.elf -x gdb.script
```

This saves on a few steps each time you restart the debugging session.

## Add exception handler

Add the following code:

```
ifdef ARM9
defaultExceptionHandler();
endif
```

Then start the program on hardware and wait for a crash.

```
Guru Meditation Error! data abort!

pc:0200D3C4 addr:03000000
```

Copy the pc value and start gdb:

```
$DEVKITARM/bin/arm-eabi-gdb file.elf
```

```
(gdb) b *0x0200D3C4
Breakpoint 1 at 0x200d3c4: file arm9/Somefile.cpp, line 99.
```

or even easier

```
arm-eabi-addr2line -e file.elf 0x0200D3C4

arm9/Somefile.cpp:99
```

Now open the file and see what has gone wrong.

## Test Coverage

These are my notes on how to check the coverage of test programs using gcov in
Bunjalloo (can be applied to any Linux program really). First, compile the code
to test with some new flags:

```
CCFLAGS="-fprofile-arcs -ftest-coverage" LIBS=['gcov']
```

These are commented out in the SConstruct file. Run the tests in question. For
example, in Bunjalloo the unit test programs are in the tests sub-directory:

```
cd tests/document ./tester cd tests/parser ./tester
```

This creates a gcda file for each source code file touched. Then check the output of gcov.
For example, in Bunjalloo again, you need to pass the paths to the object files:

```
cd /path/to/bunjalloo gcov -o build/pc arm9/HtmlParser.cpp | grep -2 HtmlParser
```

(The grep is to only show lines that interest us) This gives output as follows:

```
File 'arm9/HtmlParser.cpp' Lines executed:77.84% of 677
arm9/HtmlParser.cpp:creating 'HtmlParser.cpp.gcov'
```

In the `*.cpp.gcov` file, find `#####` marks. These indicate lines not executed.
Add tests that run through these branches of code. Rinse, repeat.
