#include "../include/plugprocessor.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

using namespace AbNinjam;
using namespace Vst3;
using namespace std;

//-----------------------------------------------------------------------------
PlugProcessor::PlugProcessor() {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::PlugProcessor";
  // register its editor class
  setControllerClass(abNinjamControllerUID);
  hostController = new HostController();
  ninjamClient = new NinjamClient();
  connectedOld = false;
  oscTransmitter = new OscTransmitter();
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::initialize(FUnknown *context) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::initialize";
  //---always initialize the parent-------
  tresult result = AudioEffect::initialize(context);
  if (result != kResultTrue)
    return kResultFalse;

  //---create Audio In/Out buses------
  // we want a stereo Input and a Stereo Output
  addAudioInput(STR16("AudioInput"), Vst::SpeakerArr::kStereo);
  addAudioOutput(STR16("AudioOutput"), Vst::SpeakerArr::kStereo);

  FUnknownPtr<Vst::IHostApplication> hostApplication(context);
  if (hostApplication) {
    hostApplication->getName(hostProductString);
    L_(ldebug) << "[PlugProcessor] hostProductString: "
               << tCharToCharPtr(hostProductString);
  }

  return kResultTrue;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setBusArrangements(
    Vst::SpeakerArrangement *inputs, int32 numIns,
    Vst::SpeakerArrangement *outputs, int32 numOuts) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::setBusArrangements";
  // we only support one in and output bus and these buses must have the same
  // number of channels
  if (numIns == 1 && numOuts == 1 && inputs[0] == outputs[0]) {
    return AudioEffect::setBusArrangements(inputs, numIns, outputs, numOuts);
  }
  return kResultFalse;
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setupProcessing(Vst::ProcessSetup &setup) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::setupProcessing";
  // here you get, with setup, information about:
  // sampleRate, processMode, maximum number of samples per audio block
  return AudioEffect::setupProcessing(setup);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setActive(TBool state) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::setActive";
  if (state) // Initialize
  {
    // Allocate Memory Here
    // Ex: algo.create ();
  } else // Release
  {
    // Free Memory if still allocated
    // Ex: if(algo.isCreated ()) { algo.destroy (); }
  }

  // reset the indicator value
  connectedOld = false;

  return AudioEffect::setActive(state);
}

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::process(Vst::ProcessData &data) {
  // L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::process";
  //--- Read inputs parameter changes-----------
  if (data.inputParameterChanges) {
    int32 numParamsChanged = data.inputParameterChanges->getParameterCount();
    for (int32 index = 0; index < numParamsChanged; index++) {
      Vst::IParamValueQueue *paramQueue =
          data.inputParameterChanges->getParameterData(index);
      if (paramQueue) {
        Vst::ParamValue value;
        int32 sampleOffset;
        int32 numPoints = paramQueue->getPointCount();

        L_(ltrace) << "[PlugProcessor] paramQueue->getParameterId(): "
                   << paramQueue->getParameterId();

        switch (paramQueue->getParameterId()) {
        case AbNinjamParams::kParamMetronomeVolId:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
              kResultTrue) {
            metronomeVolumeParam = value;
            ninjamClient->gsNjClient()->config_metronome =
                static_cast<float>(metronomeVolumeParam);
          }
          break;
        case AbNinjamParams::kParamConnectId:
          L_(ltrace) << "AbNinjamParams::kParamConnectId has changed";
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
              kResultTrue) {
            connectParam = value > 0 ? 1 : 0;
            connectionProperties.gsHost() = messageTexts.at(0);
            connectionProperties.gsUsername() = messageTexts.at(1);
            connectionProperties.gsPassword() = messageTexts.at(2);
            connectToServer(connectParam, &connectionProperties);
          }
          break;
        case AbNinjamParams::kBypassId:
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
              kResultTrue)
            mBypass = (value > 0.5);
          break;
        }
      }
    }
  }

  //--- Process Audio---------------------
  //--- ----------------------------------
  if (data.numInputs == 0 || data.numOutputs == 0) {
    // nothing to do
    return kResultOk;
  }

  if (data.numSamples > 0) {
    if (ninjamClient->connected) {
      if (hostController->hostIsPlaying(data.processContext)) {
        if (!hostController->hostPlayingInLastBuffer) {
          L_(ldebug) << "[PlugProcessor] hostPlayingInLastBuffer: "
                     << hostController->hostPlayingInLastBuffer;
          synced = false;
        }
        if (!synced) {
          int pos, samplesInInterval;
          ninjamClient->gsNjClient()->GetPosition(&pos, &samplesInInterval);
          if (samplesInInterval > 1000) {
            int startPosition =
                static_cast<int>(hostController->getStartPositionForHostSync(
                    data.processContext));
            long intervalPosition = 0;
            if (startPosition >= 0) {
              intervalPosition = startPosition % samplesInInterval;
            } else {
              intervalPosition =
                  samplesInInterval - abs(startPosition % samplesInInterval);
            }

            if (abs(intervalPosition - pos) < data.numSamples) {
              synced = true;
            }
          } else {
            L_(ldebug) << "[PlugProcessor] audiostreamForSync";
            ninjamClient->audiostreamForSync(
                data.inputs->channelBuffers32, data.inputs->numChannels,
                data.outputs->channelBuffers32, data.outputs->numChannels,
                data.numSamples, static_cast<int>(processSetup.sampleRate));
          }
        }

        if (synced) {
          ninjamClient->audiostreamOnSamples(
              data.inputs->channelBuffers32, data.inputs->numChannels,
              data.outputs->channelBuffers32, data.outputs->numChannels,
              data.numSamples, static_cast<int>(processSetup.sampleRate));

          ninjamBpm = ninjamClient->gsNjClient()->GetActualBPM();
          hostBpm = data.processContext->tempo;

          if (abs(static_cast<double>(ninjamBpm) - hostBpm) >
              numeric_limits<double>::epsilon()) {
            if (connectionProperties.gsAutoSyncBpm()) {
              if (abs(ninjamBpm - previousNinjamBpm) >
                  numeric_limits<float>::epsilon()) {
                // BPM was changed on remote. Sync to host
                L_(ldebug) << "[PlugProcessor] Sending OSC message";
                int oscSendStatus = oscTransmitter->sendInt(
                    "/tempo/raw", static_cast<int>(ninjamBpm));
                L_(ltrace) << "[PlugProcessor] oscSendStatus: "
                           << oscSendStatus;
                clearNotification = true;
              } else if (abs(hostBpm - previousHostBpm) >
                         numeric_limits<double>::epsilon()) {
                // BPM was changed on host. Sync to remote
                L_(ldebug) << "[PlugProcessor] BPM was changed on host. "
                              "Expecting to receive OSC message";
                ninjamClient->setBpm(static_cast<int>(hostBpm));
                clearNotification = true;
              }
            }

            if (!bpmNotification) {
              L_(linfo) << "[PlugProcessor] "
                           "NINJAM BPM: "
                        << ninjamBpm << ", "
                        << tCharToCharPtr(hostProductString)
                        << " BPM: " << data.processContext->tempo;
              bpmNotification = true;
              string notification("Ninjam BPM: " +
                                  to_string(static_cast<int>(ninjamBpm)));
              this->sendTextMessage(notification.c_str());
              clearNotification = false;
              notificationCleared = false;
            }
          }

          previousNinjamBpm = ninjamClient->gsNjClient()->GetActualBPM();
          previousHostBpm = data.processContext->tempo;

        } else {
          ninjamClient->clearBuffers(data.outputs->channelBuffers32,
                                     data.outputs->numChannels,
                                     data.numSamples);
          bpmNotification = false;
          clearNotification = true;
        }

      } else {
        // Connected but not synced to host
        ninjamClient->audiostreamOnSamples(
            data.inputs->channelBuffers32, data.inputs->numChannels,
            data.outputs->channelBuffers32, data.outputs->numChannels,
            data.numSamples, static_cast<int>(processSetup.sampleRate));
        synced = false;
        clearNotification = true;
        previousNinjamBpm = 0.f;
      }
      if (ninjamClient->gsNjClient()->HasUserInfoChanged()) {
        // optimize channel levels
        if (!manualMixingTouched) {
          ninjamClient->adjustVolume();
        }
        vector<Common::RemoteUser> remoteUsers = ninjamClient->getRemoteUsers();
        if (IPtr<IMessage> message = allocateMessage()) {
          message->setMessageID("BinaryMessage");
          message->getAttributes()->setBinary(
              "remoteUsers", &remoteUsers,
              remoteUsers.capacity() * sizeof(Common::RemoteUser) +
                  sizeof(remoteUsers));
          sendMessage(message);
        }
      }
    } else {
      // Not connected
      ninjamClient->clearBuffers(data.outputs->channelBuffers32,
                                 data.outputs->numChannels, data.numSamples);
      synced = false;
      clearNotification = true;
      previousNinjamBpm = 0.f;
    }

    if (clearNotification && !notificationCleared) {
      this->sendTextMessage("");
      notificationCleared = true;
    }

    hostController->hostPlayingInLastBuffer =
        hostController->hostIsPlaying(data.processContext);
  }

  //--- Write outputs parameter changes-----------
  Steinberg::Vst::IParameterChanges *outParamChanges =
      data.outputParameterChanges;
  // a new value of VuMeter will be send to the host
  // (the host will send it back in sync to our controller for updating our
  // editor)
  if (outParamChanges && connectedOld != ninjamClient->connected) {
    int32 index = 0;
    Steinberg::Vst::IParamValueQueue *paramQueue =
        outParamChanges->addParameterData(
            AbNinjamParams::kParamConnectionIndicatorId, index);
    if (paramQueue) {
      int32 index2 = 0;
      paramQueue->addPoint(0, ninjamClient->connected, index2);
    }
  }
  connectedOld = ninjamClient->connected;

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::setState(IBStream *state) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::setState";
  if (!state)
    return kResultFalse;

  // called when we load a preset or project, the model has to be reloaded

  IBStreamer streamer(state, kLittleEndian);

  double savedMetronomeVolumeParam = 0;
  if (streamer.readDouble(savedMetronomeVolumeParam) == false)
    return kResultFalse;

  int32 savedBypass = 0;
  if (streamer.readInt32(savedBypass) == false)
    return kResultFalse;

  int32 savedConnectParam = 0;
  if (streamer.readInt32(savedConnectParam) == false)
    return kResultFalse;

  int32 savedConnectionIndicatorParam = 0;
  if (streamer.readInt32(savedConnectionIndicatorParam) == false)
    return kResultFalse;

  metronomeVolumeParam = savedMetronomeVolumeParam;
  mBypass = savedBypass > 0;
  connectParam = savedConnectParam > 0 ? 1 : 0;
  connectionIndicatorParam = savedConnectionIndicatorParam > 0 ? 1 : 0;

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::getState(IBStream *state) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::getState";
  // here we need to save the model (preset or project)

  double toSaveMetronomeVolumeParam = metronomeVolumeParam;
  int32 toSaveBypass = mBypass ? 1 : 0;
  int32 toSaveConnectParam = connectParam;
  int32 toSaveConnectionIndicatorParam = connectionIndicatorParam;

  IBStreamer streamer(state, kLittleEndian);
  streamer.writeDouble(toSaveMetronomeVolumeParam);
  streamer.writeInt32(toSaveBypass);
  streamer.writeInt32(toSaveConnectParam);
  streamer.writeInt32(toSaveConnectionIndicatorParam);

  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugProcessor::notify(Vst::IMessage *message) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::notify";
  if (!message)
    return kInvalidArgument;

  if (!strcmp(message->getMessageID(), "TextMessage")) {
    Steinberg::Vst::TChar string[256] = {0};

    if (message->getAttributes()->getString(
            "host", string, sizeof(string) / sizeof(char16)) == kResultOk) {
      messageTexts[0] = tCharToCharPtr(string);
    }

    if (message->getAttributes()->getString(
            "user", string, sizeof(string) / sizeof(char16)) == kResultOk) {
      messageTexts[1] = tCharToCharPtr(string);
    }

    if (message->getAttributes()->getString(
            "pass", string, sizeof(string) / sizeof(char16)) == kResultOk) {
      messageTexts[2] = tCharToCharPtr(string);
    }
  }

  if (!strcmp(message->getMessageID(), "BinaryMessage")) {
    const void *data;
    uint32 size;
    if (message->getAttributes()->getBinary("remoteUserChannel", data, size) ==
        kResultOk) {
      L_(ltrace) << "[PlugProcessor] Received remoteUserChannel";
      if (size > 0) {
        const RemoteUserChannel remoteUserChannel =
            *static_cast<const Common::RemoteUserChannel *>(data);

        L_(ltrace) << "[PlugProcessor] remoteUserChannel.userId: "
                   << remoteUserChannel.userId;
        L_(ltrace) << "[PlugProcessor] remoteUserChannel.channelId: "
                   << remoteUserChannel.channelId;
        L_(ltrace) << "[PlugProcessor] remoteUserChannel.volume: "
                   << remoteUserChannel.volume;

        ninjamClient->setUserChannelVolume(remoteUserChannel.userId,
                                           remoteUserChannel.channelId,
                                           remoteUserChannel.volume);
      }
    }
  }

  return AudioEffect::notify(message);
}

char *PlugProcessor::tCharToCharPtr(Steinberg::Vst::TChar *tChar) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::tCharToCharPtr";
  Steinberg::String str(tChar);
  str.toMultiByte(kCP_Utf8);
  if (str.text8()) {
    return strdup(str.text8());
  }
  return nullptr;
}

void PlugProcessor::connectToServer(
    int16 value, ConnectionProperties *connectionProperties) {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::connectToServer";
  NinjamClientStatus status = ninjamClientStatus;
  if (value > 0) {
    L_(ldebug) << "[PlugProcessor] Connect initiated";
    status = ninjamClient->connect(connectionProperties);
  } else {
    L_(ldebug) << "[PlugProcessor] Disconnect initiated";
    if (ninjamClient) {
      ninjamClient->disconnect();
      status = disconnected;
      manualMixingTouched = false;
    }
  }

  L_(ldebug) << "[PlugProcessor] NinjamClient status: " << status;
  if (ninjamClientStatus != status) {
    ninjamClientStatus = status;
    if (IPtr<IMessage> message = allocateMessage()) {
      message->setMessageID("StatusMessage");
      message->getAttributes()->setInt("ninjamClientStatus",
                                       ninjamClientStatus);
      sendMessage(message);
    }
  }
}

//------------------------------------------------------------------------
tresult PlugProcessor::receiveText(const char *text) {
  L_(ltrace) << "[PlugProcessor] Entering PlugController::receiveText";
  // received from Component
  if (text) {
    L_(ldebug) << "[PlugProcessor] received: " << text;
  }
  if (strcmp(text, "manualMixingTouched") == 0) {
    manualMixingTouched = true;
  }
  return kResultOk;
}

//------------------------------------------------------------------------
