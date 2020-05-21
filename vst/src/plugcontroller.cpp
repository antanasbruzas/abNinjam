#include "../include/plugcontroller.h"
#include "../include/abuimessagecontroller.h"
#include "../include/abvst3editor.h"

using namespace AbNinjam;
using namespace Vst3;
using namespace Common;

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugController::initialize(FUnknown *context) {
  L_(ltrace) << "[PlugController] Entering PlugController::initialize";
  tresult result = EditController::initialize(context);
  if (result == kResultTrue) {
    //---Create Parameters------------
    //---Metronome volume parameter--
    auto *metronomeVolumeParam = new MetronomeVolumeParameter(
        ParameterInfo::kCanAutomate, AbNinjamParams::kParamMetronomeVolId);
    parameters.addParameter(metronomeVolumeParam);
    parameters.addParameter(STR16("Bypass"), nullptr, 1, 0,
                            ParameterInfo::kCanAutomate |
                                ParameterInfo::kIsBypass,
                            AbNinjamParams::kBypassId);
    parameters.addParameter(STR16("Connect to server"), STR16("On/Off"), 1, 0,
                            ParameterInfo::kCanAutomate,
                            AbNinjamParams::kParamConnectId, 0,
                            STR16("Connect"));
    parameters.addParameter(
        STR16("Connection indicator"), STR16("Connected/Disconnected"), 1, 0,
        ParameterInfo::kIsReadOnly, AbNinjamParams::kParamConnectionIndicatorId,
        0, STR16("Connection"));

    notificationLabel = nullptr;

    //---Custom state init------------
    // Steinberg::String defaultPort("2049");
    // defaultPort.copyTo16(messageTexts[3], 0, 127);
  }
  return kResultTrue;
}

//------------------------------------------------------------------------
IController *
PlugController::createSubController(UTF8StringPtr name,
                                    const IUIDescription * /*description*/,
                                    VST3Editor * /*editor*/) {
  L_(ltrace) << "[PlugController] Entering PlugController::createSubController";
  if (UTF8StringView(name) == "MessageController") {
    auto *controller = new UIMessageController(this);
    addUIMessageController(controller);
    return controller;
  }

  return nullptr;
}

//------------------------------------------------------------------------
IPlugView *PLUGIN_API PlugController::createView(const char *name) {
  L_(ltrace) << "[PlugController] Entering PlugController::createView";
  // someone wants my editor
#ifndef WITHOUT_GUI
  if (name && strcmp(name, "editor") == 0) {
    auto *view = new AbVST3Editor(this, "view", "plug.uidesc");
    return view;
  }
#endif
  return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::setComponentState(IBStream *state) {
  L_(ltrace) << "[PlugController] Entering PlugController::setComponentState";
  // we receive the current state of the component (processor part)
  // we read our parameters and bypass value...
  if (!state)
    return kResultFalse;

  IBStreamer streamer(state, kLittleEndian);

  double metronomeVolumeState = 0;
  if (streamer.readDouble(metronomeVolumeState) == false)
    return kResultFalse;
  setParamNormalized(AbNinjamParams::kParamMetronomeVolId,
                     metronomeVolumeState);
  // read the bypass
  int32 bypassState;
  if (streamer.readInt32(bypassState) == false)
    return kResultFalse;
  setParamNormalized(kBypassId, bypassState ? 1 : 0);

  int8 connectState = 0;
  if (streamer.readInt8(connectState) == false)
    return kResultFalse;
  setParamNormalized(AbNinjamParams::kParamConnectId, connectState);

  int8 connectionIndicatorState = 0;
  if (streamer.readInt8(connectionIndicatorState) == false)
    return kResultFalse;
  setParamNormalized(AbNinjamParams::kParamConnectionIndicatorId,
                     connectionIndicatorState);

  return kResultOk;
}

//------------------------------------------------------------------------
void PlugController::addUIMessageController(UIMessageController *controller) {
  L_(ltrace)
      << "[PlugController] Entering PlugController::addUIMessageController";
  uiMessageControllers.push_back(controller);
}

//------------------------------------------------------------------------
void PlugController::removeUIMessageController(
    UIMessageController *controller) {
  L_(ltrace)
      << "[PlugController] Entering PlugController::removeUIMessageController";
  UIMessageControllerList::const_iterator it = std::find(
      uiMessageControllers.begin(), uiMessageControllers.end(), controller);
  if (it != uiMessageControllers.end())
    uiMessageControllers.erase(it);
}

//------------------------------------------------------------------------
void PlugController::setMessageText(String128 text, unsigned long index) {
  L_(ltrace) << "[PlugController] Entering PlugController::setMessageText";
  if (messageTexts.size() > index) {
    Steinberg::String tmp(text);
    tmp.copyTo16(messageTexts[index], 0, 127);

    const char *id;
    switch (index) {
    case 0:
      id = "host";
      break;
    case 1:
      id = "user";
      break;
    case 2:
      id = "pass";
      break;
    default:
      id = nullptr;
      break;
    }
    if (id != nullptr) {
      //---send a message
      if (IPtr<IMessage> message = allocateMessage()) {
        message->setMessageID("TextMessage");
        message->getAttributes()->setString(id, text);
        sendMessage(message);
      }
    }
  }
}

//------------------------------------------------------------------------
TChar *PlugController::getMessageText(unsigned long index) {
  L_(ltrace) << "[PlugController] Entering PlugController::getMessageText";
  if (messageTexts.size() > index) {
    return messageTexts.at(index);
  }
  return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::setState(IBStream *state) {
  L_(ltrace) << "[PlugController] Entering PlugController::setState";
  IBStreamer streamer(state, kLittleEndian);

  int8 byteOrder;
  if (streamer.readInt8(byteOrder) == false)
    return kResultFalse;
  for (unsigned short j = 0; j < messageTexts.size(); j++) {
    if (streamer.readRaw(messageTexts[j], 128 * sizeof(TChar)) == false)
      return kResultFalse;

    // if the byteorder doesn't match, byte swap the text array ...
    if (byteOrder != BYTEORDER) {
      for (int32 i = 0; i < 128; i++) {
        SWAP_16(messageTexts[j][i])
      }
    }

    // update our editors
    for (auto &uiMessageController : uiMessageControllers)
      uiMessageController->setMessageText(messageTexts[j], j);
  }
  return kResultTrue;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::getState(IBStream *state) {
  L_(ltrace) << "[PlugController] Entering PlugController::getState";
  // here we can save UI settings for example

  // as we save a Unicode string, we must know the byteorder when setState is
  // called

  IBStreamer streamer(state, kLittleEndian);

  int8 byteOrder = BYTEORDER;
  if (streamer.writeInt8(byteOrder) == false)
    return kResultFalse;

  for (auto &messageText : messageTexts) {
    if (streamer.writeRaw(messageText, 128 * sizeof(TChar)) == false)
      return kResultFalse;
  }

  return kResultTrue;
}

//------------------------------------------------------------------------
tresult PlugController::receiveText(const char *text) {
  L_(ltrace) << "[PlugController] Entering PlugController::receiveText";
  // received from Component
  if (text) {
    L_(ldebug) << "[PlugController] received: " << text;
  }
  if (notificationLabel) {
    notificationLabel->setText(text);
  }
  return kResultOk;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::notify(Vst::IMessage *message) {
  L_(ltrace) << "[PlugController] Entering PlugController::notify";
  if (!message)
    return kInvalidArgument;

  if (!strcmp(message->getMessageID(), "StatusMessage")) {
    int64 ninjamClientStatus;
    if (message->getAttributes()->getInt("ninjamClientStatus",
                                         ninjamClientStatus) == kResultOk) {
      auto status = static_cast<NinjamClientStatus>(ninjamClientStatus);
      assert(status == ok || status == serverNotProvided ||
             status == licenseNotAccepted || status == connectionError ||
             status == disconnected);

      L_(ldebug) << "[PlugController] status: " << status;
      if (notificationLabel && menu) {
        switch (status) {
        case ok:
          menu->setValueNormalized(1);
          break;
        case disconnected:
          notificationLabel->setText("");
          menu->setValueNormalized(0);
          break;
        case serverNotProvided:
          notificationLabel->setText("Server not provided!");
          menu->setValueNormalized(0);
          break;
        case licenseNotAccepted:
          notificationLabel->setText("License not accepted!");
          menu->setValueNormalized(0);
          break;
        case connectionError:
          notificationLabel->setText("Connection error!");
          menu->setValueNormalized(0);
          break;
        }
        menu->valueChanged();
      }
    }
  }

  return EditController::notify(message);
}

//------------------------------------------------------------------------
CView *PlugController::createCustomView(UTF8StringPtr name,
                                        const UIAttributes &attributes,
                                        const IUIDescription *description,
                                        VST3Editor *editor) {
  L_(ltrace) << "[PlugController] Entering PlugController::createCustomView";
  if (name && strcmp(name, "NotificationText") == 0) {
    CRect size;
    notificationLabel = new CTextLabel(size);
    return notificationLabel;
  }

  if (name && strcmp(name, "Menu") == 0) {
    CRect size;
    menu = new CSegmentButton(size);
    menu->setVisible(false);
    return menu;
  }
  return nullptr;
}

//------------------------------------------------------------------------
void PlugController::willClose(VST3Editor *editor) {
  L_(ltrace) << "[PlugController] Entering PlugController::willClose";
  notificationLabel = nullptr;
  menu = nullptr;
}
