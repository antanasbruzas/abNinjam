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
  L_(ltrace) << "Entering NinjamClient::NinjamClient";
  njClient = new NJClient;
  njClient->config_savelocalaudio = 1;
  njClient->LicenseAgreementCallback = licensecallback;
  njClient->ChatMessage_Callback = chatmsg_cb;
  njClient->SetLocalChannelInfo(0, "channel0", true, 0, false, 0, true, true);
  njClient->SetLocalChannelMonitoring(0, false, 0.0f, false, 0.0f, false, false,
                                      false, false);
}

NinjamClient::~NinjamClient() {
  L_(ltrace) << "Entering NinjamClient::~NinjamClient";
  disconnect();
}

void keepConnectionThread(NinjamClient *ninjamClient) {
  L_(ltrace) << "Entering keepConnectionThread";
  scoped_lock<mutex> lock(ninjamClient->gsMtx());
  ninjamClient->gsStopConnectionThread() = false;
  NJClient *g_client = ninjamClient->gsNjClient();
  bool connected = false;
  while (g_client->GetStatus() >= 0 &&
         ninjamClient->gsStopConnectionThread() == false) {
    if (g_client->Run()) {
      // Sleep to prevent 100% CPU usage
      struct timespec ts = {0, 1000 * 1000};
      nanosleep(&ts, nullptr);
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
  connected = false;

  // return nullptr;
}

int NinjamClient::connect(ConnectionProperties connectionProperties) {
  L_(ltrace) << "Entering NinjamClient::connect";
  path propertiesPath = getHomePath();

  ostringstream oss;
  oss << "abNinjam" << separator() << "connection.properties";
  propertiesPath /= oss.str();
  if (exists(propertiesPath)) {
    L_(ldebug) << "Configuration file provided: " << propertiesPath;
    connectionProperties.readFromFile(propertiesPath);
  } else {
    L_(ldebug) << "Configuration file not provided.";
  }

  if (isEmpty(connectionProperties.gsHost())) {
    return -1;
  }

  if (isEmpty(connectionProperties.gsUsername())) {
    connectionProperties.gsUsername() = strdup("anonymous");
  }

  agree = 1;
  autoAgree = connectionProperties.gsLicenseAutoAgree();

  L_(ltrace) << "Status: " << njClient->GetStatus();
  L_(ltrace) << "IsAudioRunning: " << njClient->IsAudioRunning();

  // TODO: check if "if" condition is needed here
  if (njClient->GetStatus() != 0 && njClient->IsAudioRunning() != 1) {
    njClient->Connect(connectionProperties.gsHost(),
                      connectionProperties.gsUsername(),
                      connectionProperties.gsPassword());
  }

  bool connected = false;
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
    return -2;
  }
  return -3;
}

void NinjamClient::disconnect() {
  L_(ltrace) << "Entering NinjamClient::disconnect";
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
}
