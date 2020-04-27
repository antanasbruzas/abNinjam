#include "../include/fileutil.h"

namespace abNinjam {

path getHomePath() {
  char *home;
#ifdef unix
  home = getenv("HOME");
#elif defined(_WIN32)
  home = getenv("HOMEDRIVE");
  const char *homepath = getenv("HOMEPATH");
  home = static_cast<char *>(malloc(strlen(home) + strlen(homepath) + 1));
  strcat(home, homepath);
#endif
  return home;
}

} // namespace abNinjam
