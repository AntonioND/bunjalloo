#include <iostream>
#include "string_utils.h"
#include <cstdio>
#include <cstdlib>

// This compares std::split() with python's split()
//
// Run the program like this:
//
//     ./test.elf 123/124/1566/777// /
//
// You'll get something similar to this:
//
//     ['123', '124', '1566', '777', '', '']
//     ['123', '124', '1566', '777', '', '']

int main(int argc, char *argv[])
{
  if (argc < 3)
    return -1;

  using std::string;
  using std::vector;
  string s(argv[1]);
  string sep(argv[2]);
  vector<string> out;
  split(s, out, sep);
  printf("[");
  bool needComma = false;
  for (vector<string>::iterator it(out.begin()); it != out.end(); ++it)
  {
    if (needComma) {
      printf(", ");
    }
    needComma = true;
    printf("'%s'", it->c_str());
  }
  printf("]\n");
  string cmd("python3 -c'print(\"");
  cmd += s;
  cmd += "\".split(\"";
  cmd += sep;
  cmd += "\"))'";
  system(cmd.c_str());

  return 0;
}
