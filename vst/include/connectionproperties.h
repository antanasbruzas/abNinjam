#ifndef CONNECTIONPROPERTIES_H
#define CONNECTIONPROPERTIES_H

#pragma once
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include "../filesystem.hpp"
#include "log.h"

using namespace std;
using namespace std::filesystem;

namespace abNinjam {

class ConnectionProperties {
public:
  ConnectionProperties();
  ~ConnectionProperties();

public:
  auto &gsHost() { return host; }
  auto &gsUsername() { return username; }
  auto &gsPassword() { return password; }
  auto &gsLicenseAutoAgree() { return licenseAutoAgree; }
  void readFromFile(path path);

private:
  char *host, *username, *password;
  bool licenseAutoAgree;
};

} // namespace abNinjam

#endif // CONNECTIONPROPERTIES_H
