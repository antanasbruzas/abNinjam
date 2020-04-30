#include "../include/stringutil.h"

namespace abNinjam {

bool isEmpty(char *c) {
  L_(ltrace) << "Entering isEmpty";
  if ((c != nullptr) && (c[0] == '\0')) {
    return true;
  }
  return false;
}

} // namespace abNinjam
