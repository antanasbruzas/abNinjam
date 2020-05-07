#ifndef FILEUTIL_H
#define FILEUTIL_H

#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "log.h"
#include <experimental/filesystem>
#include <string>

using namespace std;
using namespace std::experimental::filesystem;

namespace abNinjam {

path getHomePath();

inline char separator() {
#ifdef _WIN32
  return '\\';
#else
  return '/';
#endif
}

} // namespace abNinjam

#endif // FILEUTIL_H
