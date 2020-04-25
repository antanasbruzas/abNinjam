#ifndef NINJAMCLIENT_H
#define NINJAMCLIENT_H

#pragma once

#include "../../../../external/ninjam/ninjam/njclient.h"
#include <iostream>
#include <pthread.h>

class NinjamClient {

public:
  NinjamClient();
  ~NinjamClient();
  int connect();
  void disconnect();
  NJClient *g_client;
  bool stopConnectionThread;

private:
  pthread_t connectionThread;
};

#endif // NINJAMCLIENT_H
