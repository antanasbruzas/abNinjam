#ifndef FILEUTIL_H
#define FILEUTIL_H

#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include "../filesystem.hpp"
#include "log.h"
#include <string>

using namespace std;
using namespace std::filesystem;

namespace abNinjam {

path getHomePath();

inline char separator() {
#ifdef _WIN32
  char s;
  wctomb_s(nullptr, &s, sizeof(char), path::preferred_separator);
  return s;
#else
  return path::preferred_separator;
#endif
}

} // namespace abNinjam

#endif // FILEUTIL_H
