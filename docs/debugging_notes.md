# Debugging notes

## Debugging NDS ROM

melonDS can run Bunjallo pretty well, including WiFi, SD cards and Slot-2 RAM
expansion cartridges. You can find several ways to debug programs in the
[documentation of BlocksDS](https://blocksds.skylyrac.net/docs/usage/debugging/).

Bunjalloo enables the default exception handler, you don't need to enable it
manually.

## Notes about GDB

You can create a file called "gdb.script" that contains this:

```
b main
target remote localhost:55555
c
```

Then run the file as follows:

```
gdb-multiarch file-arm9.elf -x gdb.script
```

This saves on a few steps each time you restart the debugging session.

## Test Coverage

**Note: This section is outdated**

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
