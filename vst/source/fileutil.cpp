#include "../include/fileutil.h"

namespace abNinjam {

path getHomePath() {
  L_(ltrace) << "Entering getHomePath";
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

} // namespace abNinjam
