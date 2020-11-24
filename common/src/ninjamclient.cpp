#include "../include/ninjamclient.h"
#include "../include/fileutil.h"
#include "../include/licensedialog.h"
#include "../include/stringutil.h"

#include <sstream>

using namespace AbNinjam::Common;
using namespace std;

static int agree = 1;
static bool autoAgree = false;
static int bpm = 110;

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

  if (parms[2] && !strcmp(parms[2], "No BPM/BPI permission")) {
    L_(ldebug) << parms[2];
    string message = "!vote bpm ";
    message.append(to_string(bpm));
    inst->ChatMessage_Send("MSG", message.c_str());
  }
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

NinjamClientStatus
NinjamClient::connect(ConnectionProperties *connectionProperties) {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::connect";
  path propertiesPath = getHomePath();
  ostringstream oss;
  oss << "abNinjam" << separator() << "connection.properties";
  propertiesPath /= oss.str();
  if (exists(propertiesPath)) {
    L_(ldebug) << "Configuration file provided: " << propertiesPath;
    connectionProperties->readFromFile(propertiesPath);
  } else {
    L_(ldebug) << "[NinjamClient] Configuration file not provided.";
  }

  if (isEmpty(connectionProperties->gsHost())) {
    return serverNotProvided;
  }

  if (isEmpty(connectionProperties->gsUsername())) {
    connectionProperties->gsUsername() = strdup("anonymous");
  } else {
    if (isEmpty(connectionProperties->gsPassword())) {
      string anonUser = string("anonymous:");
      anonUser.append(connectionProperties->gsUsername());
      connectionProperties->gsUsername() = strdup(anonUser.c_str());
    }
  }

  agree = 1;
  autoAgree = connectionProperties->gsAutoLicenseAgree();
  autoRemoteVolume = connectionProperties->gsAutoRemoteVolume();

  L_(ltrace) << "[NinjamClient] Status: " << njClient->GetStatus();
  L_(ltrace) << "[NinjamClient] IsAudioRunning: " << njClient->IsAudioRunning();

  // TODO: check if "if" condition is needed here
  if (njClient->GetStatus() != 0 && njClient->IsAudioRunning() != 1) {
    njClient->Connect(connectionProperties->gsHost(),
                      connectionProperties->gsUsername(),
                      connectionProperties->gsPassword());
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
    return ok;
  }
  if (agree == 256) {
    L_(lwarning) << "[NinjamClient] License not accepted. Not Connected.";
    return licenseNotAccepted;
  }
  L_(lerror) << "[NinjamClient] Connection error";
  return connectionError;
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
  if (connected && njClient) {
    njClient->Disconnect();
  }
  connected = false;
}

void NinjamClient::audiostreamOnSamples(float **inbuf, int innch,
                                        float **outbuf, int outnch, int len,
                                        int srate) {
  // L_(ltrace) << "[NinjamClient] Entering NinjamClient::audiostreamOnSamples";
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
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::audiostreamForSync";
  clearBuffers(outbuf, outnch, len);
  if (connected) {
    clearBuffers(inbuf, innch, len);
    njClient->AudioProc(inbuf, innch, outbuf, outnch, len, srate);
    clearBuffers(outbuf, outnch, len);
    clearBuffers(inbuf, innch, len);
  }
}

void NinjamClient::clearBuffers(float **buf, int nch, int len) {
  // L_(ltrace) << "[NinjamClient] Entering NinjamClient::clearBuffers";
  int x;
  for (x = 0; x < nch; x++)
    memset(buf[x], 0, sizeof(float) * static_cast<unsigned long>(len));
  return;
}

void NinjamClient::adjustVolume() {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::adjustVolume";
  if (autoRemoteVolume) {
    int totalRemoteChannels = 0;
    float adjustedVolume = 1.f;
    int mostUserChannels = 0;
    int numUsers = njClient->GetNumUsers();
    L_(ldebug) << "[NinjamClient] numUsers: " << numUsers;
    if (numUsers < ADJUST_VOLUME) {
      for (int useridx = 0; useridx < numUsers; useridx++) {
        for (int channelidx = 0; channelidx < MAX_USER_CHANNELS; channelidx++) {
          if (njClient->EnumUserChannels(useridx, channelidx) > -1) {
            totalRemoteChannels++;
            if (channelidx > mostUserChannels) {
              mostUserChannels = channelidx;
            }
          } else {
            break;
          }
        }
      }
      L_(ldebug) << "[PlugProcessor] totalRemoteChannels: "
                 << totalRemoteChannels;
      if (totalRemoteChannels > 0) {
        adjustedVolume = adjustedVolume / totalRemoteChannels;
        L_(ldebug) << "[PlugProcessor] adjustedVolume: " << adjustedVolume;
        for (int useridx = 0; useridx < numUsers; useridx++) {
          for (int channelidx = 0; channelidx <= mostUserChannels;
               channelidx++) {
            setUserChannelVolume(useridx, channelidx, adjustedVolume);
          }
          L_(ltrace) << "[PlugProcessor] Channels volume updated for useridx: "
                     << useridx;
        }
      }
    }
  }
}

std::vector<AbNinjam::Common::RemoteUser> NinjamClient::getRemoteUsers() {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::getRemoteUsers";
  std::vector<RemoteUser> users;
  int numUsers = njClient->GetNumUsers();
  L_(ltrace) << "[NinjamClient] numUsers: " << numUsers;
  for (int useridx = 0; useridx < numUsers; useridx++) {
    RemoteUser *user = new RemoteUser();
    user->id = useridx;
    user->name = njClient->GetUserState(useridx);
    L_(ltrace) << "[NinjamClient] user->id: " << user->id;
    L_(ltrace) << "[NinjamClient] user->name: " << user->name;
    for (int channelidx = 0; channelidx < MAX_USER_CHANNELS; channelidx++) {
      float vol;
      char *result =
          njClient->GetUserChannelState(useridx, channelidx, nullptr, &vol);
      if (result != 0) {
        RemoteChannel *channel = new RemoteChannel();
        channel->id = channelidx;
        channel->volume = vol;
        channel->name = result;
        user->channels.push_back(*channel);
        L_(ltrace) << "[NinjamClient] channel->id: " << channel->id;
        L_(ltrace) << "[NinjamClient] channel->volume: " << channel->volume;
        L_(ltrace) << "[NinjamClient] channel->name: " << channel->name;
      } else {
        L_(ltrace) << "[NinjamClient] No channel with id " << channelidx
                   << " found for user " << user->name;
      }
    }
    users.push_back(*user);
  }
  return users;
}
void NinjamClient::setBpm(int tempo) {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::setBpm";
  bpm = tempo;
  string message = "bpm ";
  message.append(to_string(tempo));
  njClient->ChatMessage_Send("ADMIN", message.c_str());
}

void NinjamClient::setUserChannelVolume(int userId, int channelId,
                                        float volume) {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::setUserChannelVolume";
  L_(ltrace) << "[NinjamClient] userId: " << userId;
  L_(ltrace) << "[NinjamClient] channelId: " << channelId;
  L_(ltrace) << "[NinjamClient] volume: " << volume;
  if (njClient) {
    njClient->SetUserChannelState(userId, channelId, false, false, true, volume,
                                  false, 0.f, false, false, false, false);
  }
}

void NinjamClient::setLocalChannelVolume(int channelId, float volume) {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::setLocalChannelVolume";
  L_(ltrace) << "[NinjamClient] channelId: " << channelId;
  L_(ltrace) << "[NinjamClient] volume: " << volume;
  if (njClient) {
    if (volume > 1 || volume < 0) {
      L_(lwarning) << "[NinjamClient] monitor volume is out of range";
    } else {
      njClient->SetLocalChannelMonitoring(channelId, true, volume, false, 0.0f,
                                          false, false, false, false);
    }
  }
}

void NinjamClient::sendChatMessage(std::string message) {
  L_(ltrace) << "[NinjamClient] Entering NinjamClient::sendChatMessage";
  if (njClient) {
    njClient->ChatMessage_Send("MSG", message.c_str());
  }
}
