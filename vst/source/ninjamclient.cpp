#include "../include/ninjamclient.h"
#include "../include/fileutil.h"
#include "../include/licensedialog.h"
#include "../include/stringutil.h"

#include <sstream>

using namespace abNinjam;

static int agree = 1;
static bool autoAgree = false;

int licensecallback(void *userData, const char *licensetext) {
  L_(ltrace) << "Entering licensecallback";
  if (autoAgree) {
    return true;
  }
  LicenseDialog *licenseDialog = new LicenseDialog();
  agree = licenseDialog->showDialog(licensetext);
  if (agree == 0) {
    return true;
  }
  return false;
}

void chatmsg_cb(void *userData, NJClient *inst, const char **parms,
                int nparms) {
  L_(ltrace) << "Entering chatmsg_cb";
  // TODO: implement
}

NinjamClient::NinjamClient() {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::NinjamClient";
  njClient->config_savelocalaudio = 1;
  njClient->LicenseAgreementCallback = licensecallback;
  njClient->ChatMessage_Callback = chatmsg_cb;
  njClient->SetLocalChannelInfo(0, "channel0", true, 0, false, 0, true, true);
  njClient->SetLocalChannelMonitoring(0, false, 0.0f, false, 0.0f, false, false,
                                      false, false);
  connectionThread = nullptr;
  stopConnectionThread = true;
}

NinjamClient::~NinjamClient() {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::~NinjamClient";
  disconnect();
}

void keepConnectionThread(NinjamClient *ninjamClient) {
  L_(ltrace) << "Entering keepConnectionThread";
  // TODO: use scoped_lock for all environments when becomes available
#ifdef unix
  scoped_lock<mutex> lock(ninjamClient->gsMtx());
#elif defined(_WIN32)
  lock_guard<mutex> lock(ninjamClient->gsMtx());
#endif

  ninjamClient->gsStopConnectionThread() = false;
  NJClient *g_client = ninjamClient->gsNjClient();
  bool connected = false;
  while (g_client->GetStatus() >= 0 &&
         ninjamClient->gsStopConnectionThread() == false) {
    if (g_client->Run()) {
      // Sleep to prevent 100% CPU usage
#ifdef unix
      struct timespec ts = {0, 1000 * 1000};
      nanosleep(&ts, nullptr);
#elif defined(_WIN32)
      HANDLE hTimer = NULL;
      LARGE_INTEGER liDueTime;

      liDueTime.QuadPart = -10000LL;
      // Create an unnamed waitable timer.
      hTimer = CreateWaitableTimer(NULL, TRUE, NULL);
      if (NULL == hTimer) {
        L_(ltrace) << "CreateWaitableTimer failed";
      } else {
        // Set a timer to wait for 1 ms.
        if (!SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, 0)) {
          L_(ltrace) << "SetWaitableTimer failed";
        } else {
          if (WaitForSingleObject(hTimer, INFINITE) != WAIT_OBJECT_0) {
            L_(ltrace) << "WaitForSingleObject failed";
          } else {
            L_(ltrace) << "Timer was signaled.";
          }
        }
      }
#endif

      if (g_client->GetStatus() == 0) {
        if (!connected) {
          L_(ldebug) << "status: " << g_client->GetStatus();
          L_(ldebug) << "bpi: " << g_client->GetBPI();
          L_(ldebug) << "bpm: " << g_client->GetActualBPM();
          L_(ldebug) << "is audio running: " << g_client->IsAudioRunning();
          L_(ldebug) << "user: " << g_client->GetUser();
          L_(ldebug) << "hostname: " << g_client->GetHostName();
          L_(ldebug) << "num users: " << g_client->GetNumUsers() << endl;
        }
        connected = true;
      }
    }
  }
}

int NinjamClient::connect(ConnectionProperties connectionProperties) {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::connect";
  path propertiesPath = getHomePath();

  ostringstream oss;
  oss << "abNinjam" << path::preferred_separator << "connection.properties";
  propertiesPath /= oss.str();
  if (exists(propertiesPath)) {
    L_(ldebug) << "Configuration file provided: " << propertiesPath;
    connectionProperties.readFromFile(propertiesPath);
  } else {
    L_(ldebug) << "[NinjamClient] Configuration file not provided.";
  }

  if (isEmpty(connectionProperties.gsHost())) {
    return -1;
  }

  if (isEmpty(connectionProperties.gsUsername())) {
    connectionProperties.gsUsername() = strdup("anonymous");
  }

  agree = 1;
  autoAgree = connectionProperties.gsLicenseAutoAgree();

  L_(ltrace) << "[NinjamClient] Status: " << njClient->GetStatus();
  L_(ltrace) << "[NinjamClient] IsAudioRunning: " << njClient->IsAudioRunning();

  // TODO: check if "if" condition is needed here
  if (njClient->GetStatus() != 0 && njClient->IsAudioRunning() != 1) {
    njClient->Connect(connectionProperties.gsHost(),
                      connectionProperties.gsUsername(),
                      connectionProperties.gsPassword());
  }

  while (njClient->GetStatus() >= 0) {
    if (njClient->Run()) {
      if (njClient->GetStatus() == 0) {
        L_(ldebug) << "Connected";
        connected = true;
        break;
      }
    }
  }
  if (connected) {
    connectionThread = new thread(keepConnectionThread, this);
    if (connectionThread) {
      connectionThread->detach();
    }
    return 0;
  }
  if (agree == 256) {
    L_(lwarning) << "[NinjamClient] License not accepted. Not Connected.";
    return -2;
  }
  L_(lerror) << "[NinjamClient] Connection error";
  return -3;
}

void NinjamClient::disconnect() {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::disconnect";
  stopConnectionThread = true;
  if (connectionThread) {
    if (connectionThread->joinable()) {
      connectionThread->join();
    } else {
      // L_(ltrace) << "delete";
      // delete connectionThread;
    }
  }
  if (njClient) {
    njClient->Disconnect();
  }
  connected = false;
}

void NinjamClient::audiostreamOnSamples(float **inbuf, int innch,
                                        float **outbuf, int outnch, int len,
                                        int srate) {
  if (!connected) {
    // clear all output buffers
    clearBuffers(outbuf, outnch, len);
  }
  if (connected) {
    njClient->AudioProc(inbuf, innch, outbuf, outnch, len, srate);
  }
}

void NinjamClient::audiostreamForSync(float **inbuf, int innch, float **outbuf,
                                      int outnch, int len, int srate) {
  clearBuffers(outbuf, outnch, len);
  if (connected) {
    clearBuffers(inbuf, innch, len);
    njClient->AudioProc(inbuf, innch, outbuf, outnch, len, srate);
    clearBuffers(outbuf, outnch, len);
    clearBuffers(inbuf, innch, len);
  }
}

void NinjamClient::clearBuffers(float **buf, int nch, int len) {
  int x;
  for (x = 0; x < nch; x++)
    memset(buf[x], 0, sizeof(float) * static_cast<unsigned long>(len));
  return;
}
