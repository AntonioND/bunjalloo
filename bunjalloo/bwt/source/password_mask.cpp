/*
  Copyright 2009 Richard Quirk. This program is free software: you can
  redistribute it and/or modify it under the terms of the GNU General Public
  License version 3 or any later version.
*/
#include "password_mask.h"
#include "utf8.h"

static const char PASSWORD_MASK[] = "●";

std::string createPasswordMask(size_t length) {
  std::string tmp;
  for (unsigned int i = 0; i < length; ++i) {
    tmp += PASSWORD_MASK;
  }
  return tmp;
}
