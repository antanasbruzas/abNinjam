#ifndef NINJAMCLIENT_H
#define NINJAMCLIENT_H

#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#ifdef ABNINJAM_USE_LIBLO
// Prevent windows.h including winsock.h
#include <winsock2.h>
#endif
#endif

#include "../../external/ninjam/ninjam/njclient.h"
#include "connectionproperties.h"
#include "log.h"
#include "ninjamclientstatus.h"
#include <mutex>
#include <thread>

#define ADJUST_VOLUME 10

namespace AbNinjam {
namespace Common {

class NinjamClient {

public:
  NinjamClient();
  ~NinjamClient();
  NinjamClientStatus connect(ConnectionProperties connectionProperties);
  void disconnect();
  void audiostreamOnSamples(float **inbuf, int innch, float **outbuf,
                            int outnch, int len, int srate);
  void audiostreamForSync(float **inbuf, int innch, float **outbuf, int outnch,
                          int len, int srate);
  auto &gsNjClient() { return njClient; }
  auto &gsStopConnectionThread() { return stopConnectionThread; }
  auto &gsMtx() { return mtx; }
  bool connected = false;
  void clearBuffers(float **buf, int nch, int len);
  void adjustVolume();
  void setBpm(int bpm);

private:
  thread *connectionThread;
  NJClient *njClient = new NJClient;
  bool stopConnectionThread, autoRemoteVolume;
  mutex mtx;
};

} // namespace Common
} // namespace AbNinjam

#endif // NINJAMCLIENT_H
