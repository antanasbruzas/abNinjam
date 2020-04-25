#include "../include/ninjamclient.h"

int licensecallback(void *userData, const char *licensetext) {
  // TODO: implement
  return 1;
}

void chatmsg_cb(void *userData, NJClient *inst, const char **parms,
                int nparms) {
  // TODO: implement
}

NinjamClient::NinjamClient() {
  g_client = new NJClient;
  g_client->config_savelocalaudio = 1;
  g_client->LicenseAgreementCallback = licensecallback;
  g_client->ChatMessage_Callback = chatmsg_cb;
  g_client->SetLocalChannelInfo(0, "channel0", true, 0, false, 0, true, true);
  g_client->SetLocalChannelMonitoring(0, false, 0.0f, false, 0.0f, false, false,
                                      false, false);
}

NinjamClient::~NinjamClient() { disconnect(); }

void *keepConnectionThread(void *arg) {
  std::cout << "keepConnectionThread" << std::endl;
  NinjamClient *ninjamClient = (NinjamClient *)arg;
  ninjamClient->stopConnectionThread = false;
  NJClient *g_client = ninjamClient->g_client;

  bool connected = false;
  while (g_client->GetStatus() >= 0 &&
         ninjamClient->stopConnectionThread == false) {
    if (g_client->Run()) {
      if (g_client->GetStatus() == 0) {
        if (!connected) {
          std::cout << "status: " << g_client->GetStatus() << std::endl;
          std::cout << "bpi: " << g_client->GetBPI() << std::endl;
          std::cout << "bpm: " << g_client->GetActualBPM() << std::endl;
          std::cout << "is audio running: " << g_client->IsAudioRunning()
                    << std::endl;
          std::cout << "user: " << g_client->GetUser() << std::endl;
          std::cout << "hostname: " << g_client->GetHostName() << std::endl;
          std::cout << "num users: " << g_client->GetNumUsers() << "\n"
                    << std::endl;
        }
        connected = true;
      }
    }
  }
  connected = false;

  return nullptr;
}

int NinjamClient::connect() {
  std::cout << "NinjamClient::connect()" << std::endl;

  char *hostname = strdup("192.168.11.145");
  char *parmuser = strdup("anonymous");
  char *parmpass = strdup("");

  g_client->Connect(hostname, parmuser, parmpass);
  pthread_create(&connectionThread, nullptr, keepConnectionThread, this);

  return 0;
}

void NinjamClient::disconnect() {
  stopConnectionThread = true;
  pthread_join(connectionThread, nullptr);
  g_client->Disconnect();
}
