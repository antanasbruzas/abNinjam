#ifndef NINJAMCLIENT_H
#define NINJAMCLIENT_H

#pragma once

#include "../../../../external/ninjam/ninjam/njclient.h"
#include <iostream>

class NinjamClient {

public:
  NinjamClient();
  int connect();

private:
  NJClient *g_client;
  int g_done = 0;
};

#endif // NINJAMCLIENT_H
