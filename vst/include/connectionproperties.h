#ifndef CONNECTIONPROPERTIES_H
#define CONNECTIONPROPERTIES_H

#pragma once

#include <filesystem>

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
  void readFromFile(path path);

private:
  char *host, *username, *password;
};

} // namespace abNinjam

#endif // CONNECTIONPROPERTIES_H
