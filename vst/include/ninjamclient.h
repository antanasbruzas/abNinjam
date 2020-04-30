#ifndef NINJAMCLIENT_H
#define NINJAMCLIENT_H

#pragma once

#include "../../../../external/ninjam/ninjam/njclient.h"
#include "connectionproperties.h"
#include "log.h"
#include <mutex>
#include <thread>

namespace abNinjam {

class NinjamClient {

public:
  NinjamClient();
  ~NinjamClient();
  int connect(ConnectionProperties connectionProperties);
  void disconnect();
  auto &gsNjClient() { return njClient; }
  auto &gsStopConnectionThread() { return stopConnectionThread; }
  auto &gsMtx() { return mtx; }

private:
  thread *connectionThread;
  NJClient *njClient;
  bool stopConnectionThread;
  mutex mtx;
};

} // namespace abNinjam

#endif // NINJAMCLIENT_H
