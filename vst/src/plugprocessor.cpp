#include "../include/plugprocessor.h"

#include "base/source/fstreamer.h"
#include "pluginterfaces/base/ibstream.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"

using namespace AbNinjam;
using namespace Vst3;
using namespace std;

static int bpm = 110;
WDL_String g_topic;
int g_need_disp_update = 0;
WDL_PtrList<char> g_chat_buffers;

void addChatLine(const char *src, const char *text) {
  while (g_chat_buffers.GetSize() > 256) {
    free(g_chat_buffers.Get(0));
    g_chat_buffers.Delete(0);
  }
  WDL_String tmp;
  if (src && *src && !strncmp(text, "/me ", 4)) {
    tmp.Set("* ");
    tmp.Append(src);
    tmp.Append(" ");
    const char *p = text + 3;
    while (*p == ' ')
      p++;
    tmp.Append(p);
  } else {
    if (src && *src) {
      tmp.Set("<");
      tmp.Append(src);
      tmp.Append("> ");
    } else if (src) {
      tmp.Set("*** ");
    }
    tmp.Append(text);
  }
  g_chat_buffers.Add(strdup(tmp.Get()));
}

void chatMessageCallback(void * /* userData */, NJClient *inst,
                         const char **parms, int /* nparms */) {
  L_(ltrace) << "Entering chatMessageCallback";

  if (parms[2] && !strcmp(parms[2], "No BPM/BPI permission")) {
    L_(ldebug) << parms[2];
    string message = "!vote bpm ";
    message.append(to_string(bpm));
    inst->ChatMessage_Send("MSG", message.c_str());
  }

  if (!strcmp(parms[0], "TOPIC")) {
    if (parms[2]) {
      WDL_String tmp;
      if (parms[1] && *parms[1]) {
        tmp.Set(parms[1]);
        tmp.Append(" sets topic to: ");
      } else
        tmp.Set("Topic is: ");
      tmp.Append(parms[2]);

      g_topic.Set(parms[2]);
      addChatLine("", tmp.Get());

      g_need_disp_update = 1;
    }
  } else if (!strcmp(parms[0], "MSG")) {
    if (parms[1] && parms[2])
      addChatLine(parms[1], parms[2]);
    g_need_disp_update = 1;
  } else if (!strcmp(parms[0], "PRIVMSG")) {
    if (parms[1] && parms[2]) {
      WDL_String tmp;
      tmp.Set("*");
      tmp.Append(parms[1]);
      tmp.Append("* ");
      tmp.Append(parms[2]);
      addChatLine(NULL, tmp.Get());
    }
    g_need_disp_update = 1;
  } else if (!strcmp(parms[0], "JOIN") || !strcmp(parms[0], "PART")) {
    if (parms[1] && *parms[1]) {
      WDL_String tmp(parms[1]);
      tmp.Append(" has ");
      tmp.Append(parms[0][0] == 'P' ? "left" : "joined");
      tmp.Append(" the server");
      addChatLine("", tmp.Get());
    }
    g_need_disp_update = 1;
  }
}

//-----------------------------------------------------------------------------
PlugProcessor::PlugProcessor() {
  L_(ltrace) << "[PlugProcessor] Entering PlugProcessor::PlugProcessor";
  // register its editor class
  setControllerClass(abNinjamControllerUID);
  hostController = new HostController();
  ninjamClient = new NinjamClient();
  ninjamClient->gsNjClient()->ChatMessage_Callback = chatMessageCallback;
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
  if (numIns == 1 && numOuts == 1)
  {
      // the host wants Mono => Mono (or 1 channel -> 1 channel)
      if (SpeakerArr::getChannelCount (inputs[0]) == 1 &&
          SpeakerArr::getChannelCount (outputs[0]) == 1)
      {
          auto* bus = FCast<AudioBus> (audioInputs.at (0));
          if (bus)
          {
              // check if we are Mono => Mono, if not we need to recreate the busses
              if (bus->getArrangement () != inputs[0])
              {
                  getAudioInput (0)->setArrangement (inputs[0]);
                  getAudioInput (0)->setName (STR16 ("Mono In"));
                  getAudioOutput (0)->setArrangement (inputs[0]);
                  getAudioOutput (0)->setName (STR16 ("Mono Out"));
              }
              return kResultOk;
          }
      }
      // the host wants something else than Mono => Mono,
      // in this case we are always Stereo => Stereo
      else
      {
          auto* bus = FCast<AudioBus> (audioInputs.at (0));
          if (bus)
          {
              tresult result = kResultFalse;

              // the host wants 2->2 (could be LsRs -> LsRs)
              if (SpeakerArr::getChannelCount (inputs[0]) == 2 &&
                  SpeakerArr::getChannelCount (outputs[0]) == 2)
              {
                  getAudioInput (0)->setArrangement (inputs[0]);
                  getAudioInput (0)->setName (STR16 ("Stereo In"));
                  getAudioOutput (0)->setArrangement (outputs[0]);
                  getAudioOutput (0)->setName (STR16 ("Stereo Out"));
                  result = kResultTrue;
              }
              // the host want something different than 1->1 or 2->2 : in this case we want stereo
              else if (bus->getArrangement () != SpeakerArr::kStereo)
              {
                  getAudioInput (0)->setArrangement (SpeakerArr::kStereo);
                  getAudioInput (0)->setName (STR16 ("Stereo In"));
                  getAudioOutput (0)->setArrangement (SpeakerArr::kStereo);
                  getAudioOutput (0)->setName (STR16 ("Stereo Out"));
                  result = kResultFalse;
              }

              return result;
          }
      }
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
        case AbNinjamParams::kParamChannelVolumeId:
          L_(ltrace) << "AbNinjamParams::kParamChannelVolumeId has changed";
          if (paramQueue->getPoint(numPoints - 1, sampleOffset, value) ==
              kResultTrue) {
            monitorVolumeParam = value;
            L_(ltrace) << "monitorVolumeParam: " << monitorVolumeParam;
            ninjamClient->setLocalChannelVolume(0, monitorVolumeParam);
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

  // (simplification) we suppose that we have the same input
  // channel count as the output
  int32 numChannels = data.inputs[0].numChannels;

  //---get audio buffers----------------
  uint32 sampleFramesSize =
      getSampleFramesSizeInBytes(processSetup, data.numSamples);
  void **in = getChannelBuffersPointer(processSetup, data.inputs[0]);
  void **out = getChannelBuffersPointer(processSetup, data.outputs[0]);

  // This case will probably never happen because ninjam server metronome is
  // transmitted and output buffer is not silent
  //---check if silence---------------
  // normally we have to check each channel (simplification)
  //  if (data.inputs[0].silenceFlags != 0 && data.outputs[0].silenceFlags != 0)
  //  {
  //    L_(ldebug) << "[PlugProcessor] Silence detected";
  //    // the Plug-in has to be sure that if it sets the flags silence that the
  //    // output buffer are clear
  //    for (int32 i = 0; i < numChannels; i++) {
  //      // do not need to be cleared if the buffers are the same (in this case
  //      // input buffer are already cleared by the host)
  //      if (in[i] != out[i]) {
  //        memset(out[i], 0, sampleFramesSize);
  //      }
  //    }
  //    // nothing to do at this point
  //    return kResultOk;
  //  }
  // mark our outputs as not silent
  // data.outputs[0].silenceFlags = 0;

  if (data.numSamples > 0) {

    //---in bypass mode outputs should be like inputs-----
    if (mBypass) {
      L_(ldebug) << "[PlugProcessor] Bypassing";
      doBypass(numChannels, sampleFramesSize, in, out);
    } else {
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
                  L_(ltrace)
                      << "[PlugProcessor] oscSendStatus: " << oscSendStatus;
                  clearNotification = true;
                } else if (abs(hostBpm - previousHostBpm) >
                           numeric_limits<double>::epsilon()) {
                  // BPM was changed on host. Sync to remote
                  L_(ldebug) << "[PlugProcessor] BPM was changed on host. "
                                "Expecting to receive OSC message";
                  bpm = static_cast<int>(hostBpm);
                  ninjamClient->setBpm(bpm);
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
                this->sendNotification(notification);
                clearNotification = false;
                notificationCleared = false;
              }
            }

            previousNinjamBpm = ninjamClient->gsNjClient()->GetActualBPM();
            previousHostBpm = data.processContext->tempo;

          } else {
            //            ninjamClient->clearBuffers(data.outputs->channelBuffers32,
            //                                       data.outputs->numChannels,
            //                                       data.numSamples);
            doBypass(numChannels, sampleFramesSize, in, out);
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
          vector<Common::RemoteUser> remoteUsers =
              ninjamClient->getRemoteUsers();
          if (IPtr<IMessage> message = allocateMessage()) {
            message->setMessageID(binaryMessage);
            message->getAttributes()->setBinary(
                "remoteUsers", &remoteUsers,
                remoteUsers.capacity() * sizeof(Common::RemoteUser) +
                    sizeof(remoteUsers));
            sendMessage(message);
          }
        }

        if (g_need_disp_update > 0) {
          sendChatMessageUpdate(
              g_chat_buffers.Get(g_chat_buffers.GetSize() - 1));
          g_need_disp_update = 0;
        }

      } else {
        // Not connected
        //        ninjamClient->clearBuffers(data.outputs->channelBuffers32,
        //                                   data.outputs->numChannels,
        //                                   data.numSamples);
        doBypass(numChannels, sampleFramesSize, in, out);
        synced = false;
        clearNotification = true;
        previousNinjamBpm = 0.f;
      }
    }

    if (clearNotification && !notificationCleared) {
      this->sendNotification("");
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

  double savedMonitorVolumeParam = 0;
  if (streamer.readDouble(savedMonitorVolumeParam) == false)
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
  monitorVolumeParam = savedMonitorVolumeParam;
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
  double toSaveMonitorVolumeParam = monitorVolumeParam;
  int32 toSaveBypass = mBypass ? 1 : 0;
  int32 toSaveConnectParam = connectParam;
  int32 toSaveConnectionIndicatorParam = connectionIndicatorParam;

  IBStreamer streamer(state, kLittleEndian);
  streamer.writeDouble(toSaveMetronomeVolumeParam);
  streamer.writeDouble(toSaveMonitorVolumeParam);
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

  if (!strcmp(message->getMessageID(), textMessage)) {
    Steinberg::Vst::TChar string[256] = {0};
    L_(ltrace) << "[PlugProcessor] Received TextMessage";
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

  if (!strcmp(message->getMessageID(), binaryMessage)) {
    L_(ltrace) << "[PlugProcessor] Received BinaryMessage";
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

  if (!strcmp(message->getMessageID(), mixingTouchedMessage)) {
    L_(ltrace) << "[PlugProcessor] Received MixingTouchedMessage";
    int64 status;
    if (message->getAttributes()->getInt("manualMixingTouched", status) ==
        kResultOk) {
      if (status) {
        manualMixingTouched = true;
      }
    }
  }

  if (strcmp(message->getMessageID(), chatMessage) == 0) {
    String128 chatMessage;
    if (message->getAttributes()->getString("chatMessage", chatMessage,
                                            sizeof(chatMessage)) == kResultOk) {
      Steinberg::String str(chatMessage);
      str.toMultiByte(kCP_Utf8);
      const char8 *text = str.text8();
      // Received text message to send as chat message
      if (text && ninjamClient->connected) {
        ninjamClient->sendChatMessage(text);
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
    if (status == ok) {
      L_(ldebug) << "[PlugProcessor] Connection status is OK";
    }
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
      message->setMessageID(statusMessage);
      message->getAttributes()->setInt("ninjamClientStatus",
                                       ninjamClientStatus);
      sendMessage(message);
    }
  }
}

//------------------------------------------------------------------------
void PlugProcessor::sendNotification(std::string text) {
  L_(ltrace) << "[PlugProcessor] Entering PlugController::sendNotification";
  if (IPtr<IMessage> message = allocateMessage()) {
    message->setMessageID(notificationMessage);
    Steinberg::String str(text.c_str());
    message->getAttributes()->setString("notification", str.text16());
    sendMessage(message);
  }
}

//------------------------------------------------------------------------
void PlugProcessor::sendChatMessageUpdate(std::string text) {
  L_(ltrace)
      << "[PlugProcessor] Entering PlugController::sendChatMessageUpdate";
  if (IPtr<IMessage> message = allocateMessage()) {
    message->setMessageID(chatMessage);
    Steinberg::String str(text.c_str());
    message->getAttributes()->setString("chatUpdate", str.text16());
    sendMessage(message);
  }
}

void PlugProcessor::doBypass(int32 numChannels, uint32 sampleFramesSize,
                             void **in, void **out) {
  // L_(ltrace) << "[PlugProcessor] Entering PlugController::doBypass";
  // Note: assumption that input and output channel count is the same
  for (int32 i = 0; i < numChannels; i++) {
    // do not need to be copied if the buffers are the same
    if (in[i] != out[i]) {
      memcpy(out[i], in[i], sampleFramesSize);
    }
  }
}

//------------------------------------------------------------------------
