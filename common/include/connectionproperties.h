#ifndef CONNECTIONPROPERTIES_H
#define CONNECTIONPROPERTIES_H

#pragma once
#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif
#include "../filesystem.hpp"
#include "log.h"

using namespace std::filesystem;

namespace AbNinjam {
namespace Common {

class ConnectionProperties {
public:
  ConnectionProperties();
  ~ConnectionProperties();

public:
  auto &gsHost() { return host; }
  auto &gsUsername() { return username; }
  auto &gsPassword() { return password; }
  auto &gsAutoLicenseAgree() { return autoLicenseAgree; }
  auto &gsAutoRemoteVolume() { return autoRemoteVolume; }
  auto &gsAutoSyncBpm() { return autoSyncBpm; }
  void readFromFile(path path);

private:
  char *host, *username, *password;
  bool autoLicenseAgree, autoRemoteVolume, autoSyncBpm;
};

} // namespace Common
} // namespace AbNinjam

#endif // CONNECTIONPROPERTIES_H
