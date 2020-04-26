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
  int connect(char *host, char *user, char *pass);
  void disconnect();
  NJClient *g_client;
  bool stopConnectionThread;

private:
  pthread_t connectionThread;
  bool isEmpty(char *c);
};

#endif // NINJAMCLIENT_H
