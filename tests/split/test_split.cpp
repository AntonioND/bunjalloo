#include <iostream>
#include "string_utils.h"
#include <cstdio>
#include <cstdlib>

int main(int argc, char *argv[])
{
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
  string cmd("python -c'print \"");
  cmd += s;
  cmd += "\".split(\"";
  cmd += sep;
  cmd += "\")'";
  system(cmd.c_str());
}
