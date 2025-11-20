/*
  Copyright 2009 Richard Quirk. This program is free software: you can
  redistribute it and/or modify it under the terms of the GNU General Public
  License version 3 or any later version.
*/
#ifndef password_mask_h_seen
#define password_mask_h_seen
#include <string>

/** Create a masked password field*/
std::string createPasswordMask(size_t length);
#endif
