#include "../include/fileutil.h"

namespace AbNinjam {
namespace Common {

path getHomePath() {
  char *home = nullptr;
#ifdef unix
  home = getenv("HOME");
#elif defined(_WIN32)
  size_t len;
  _dupenv_s(&home, &len, "HOMEDRIVE");
  string homeString(home);
  _dupenv_s(&home, &len, "HOMEPATH");
  homeString.append(home);
  home = &homeString[0];
#endif
  return home;
}

} // namespace Common
} // namespace AbNinjam
