#ifndef CONNECTIONPROPERTIES_H
#define CONNECTIONPROPERTIES_H

#pragma once

#include "log.h"
#include <experimental/filesystem>

using namespace std;
using namespace std::experimental::filesystem;

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
