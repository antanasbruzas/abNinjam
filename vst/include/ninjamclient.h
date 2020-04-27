#ifndef NINJAMCLIENT_H
#define NINJAMCLIENT_H

#pragma once

#include "../../../../external/ninjam/ninjam/njclient.h"
#include "connectionproperties.h"
#include <pthread.h>

namespace abNinjam {

class NinjamClient {

public:
  NinjamClient();
  ~NinjamClient();
  int connect(ConnectionProperties connectionProperties);
  void disconnect();
  // NJClient *getNjClient() const { return njClient; }

  // NJClient *&getNjClient() { return njClient; }
  auto &gsNjClient() { return njClient; }
  auto &gsStopConnectionThread() { return stopConnectionThread; }

private:
  pthread_t connectionThread;
  NJClient *njClient;
  bool stopConnectionThread;
};

} // namespace abNinjam

#endif // NINJAMCLIENT_H
