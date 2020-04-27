#include "../include/ninjamclient.h"
#include "../include/fileutil.h"
#include "../include/stringutil.h"

#include <iostream>

using namespace abNinjam;

int licensecallback(void *userData, const char *licensetext) {
  cout << "status: " << licensetext << endl;
  return 1;
}

void chatmsg_cb(void *userData, NJClient *inst, const char **parms,
                int nparms) {
  // TODO: implement
}

NinjamClient::NinjamClient() {
  njClient = new NJClient;
  njClient->config_savelocalaudio = 1;
  njClient->LicenseAgreementCallback = licensecallback;
  njClient->ChatMessage_Callback = chatmsg_cb;
  njClient->SetLocalChannelInfo(0, "channel0", true, 0, false, 0, true, true);
  njClient->SetLocalChannelMonitoring(0, false, 0.0f, false, 0.0f, false, false,
                                      false, false);
}

NinjamClient::~NinjamClient() { disconnect(); }

void *keepConnectionThread(void *arg) {
  NinjamClient *ninjamClient = static_cast<NinjamClient *>(arg);
  ninjamClient->gsStopConnectionThread() = false;
  NJClient *g_client = ninjamClient->gsNjClient();

  bool connected = false;
  while (g_client->GetStatus() >= 0 &&
         ninjamClient->gsStopConnectionThread() == false) {
    if (g_client->Run()) {
      if (g_client->GetStatus() == 0) {
        if (!connected) {
          cout << "status: " << g_client->GetStatus() << endl;
          cout << "bpi: " << g_client->GetBPI() << endl;
          cout << "bpm: " << g_client->GetActualBPM() << endl;
          cout << "is audio running: " << g_client->IsAudioRunning() << endl;
          cout << "user: " << g_client->GetUser() << endl;
          cout << "hostname: " << g_client->GetHostName() << endl;
          cout << "num users: " << g_client->GetNumUsers() << "\n" << endl;
        }
        connected = true;
      }
    }
  }
  connected = false;

  return nullptr;
}

int NinjamClient::connect(ConnectionProperties connectionProperties) {
  if (isEmpty(connectionProperties.gsHost())) {
    path propertiesPath = getHomePath();
    propertiesPath /= "abNinjam/connection.properties";
    if (exists(propertiesPath)) {
      cout << "Configuration file provided: " << propertiesPath << endl;
      connectionProperties.readFromFile(propertiesPath);
    } else {
      cout << "Configuration file not provided." << endl;
    }

    if (isEmpty(connectionProperties.gsHost())) {
      return -1;
    }
  }

  if (isEmpty(connectionProperties.gsUsername())) {
    connectionProperties.gsUsername() = strdup("anonymous");
  }

  njClient->Connect(connectionProperties.gsHost(),
                    connectionProperties.gsUsername(),
                    connectionProperties.gsPassword());
  pthread_create(&connectionThread, nullptr, keepConnectionThread, this);

  return 0;
}

void NinjamClient::disconnect() {
  stopConnectionThread = true;

  if (pthread_equal(pthread_self(), connectionThread) != 0) {
    pthread_join(connectionThread, nullptr);
  }
  if (njClient) {
    njClient->Disconnect();
  }
}
