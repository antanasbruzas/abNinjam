#ifndef FILEUTIL_H
#define FILEUTIL_H

#pragma once

#include "../filesystem.hpp"
#include <string>

using namespace std;
using namespace std::filesystem;

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
