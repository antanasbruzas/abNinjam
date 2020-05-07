#ifndef FILEUTIL_H
#define FILEUTIL_H

#pragma once

#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include "../filesystem.hpp"
#include "log.h"
#include <string>

using namespace std;
using namespace std::filesystem;

namespace abNinjam {

path getHomePath();

} // namespace abNinjam

#endif // FILEUTIL_H
