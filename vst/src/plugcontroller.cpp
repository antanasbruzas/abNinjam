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
    auto *metronomeVolumeParam = new VolumeParameter(
        ParameterInfo::kCanAutomate, AbNinjamParams::kParamMetronomeVolId,
        "Metronome Volume");
    parameters.addParameter(metronomeVolumeParam);
    //---Monitor volume parameter--
    auto *monitorVolumeParam = new VolumeParameter(
        ParameterInfo::kCanAutomate, AbNinjamParams::kParamChannelVolumeId,
        "Monitor Volume");
    parameters.addParameter(monitorVolumeParam);
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
    mixerScrollView = nullptr;

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
    L_(ltrace) << "[PlugController] Found MessageController";
    auto *controller = new UIMessageController(this);
    addUIMessageController(controller);
    return controller;
  }

  if (UTF8StringView(name) == "ChatController") {
    L_(ltrace) << "[PlugController] Found ChatController";
    auto *controller = new ChatController(this);
    chatController = controller;
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
    vst3Editor = view;
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

  double monitorVolumeState = 0;
  if (streamer.readDouble(monitorVolumeState) == false)
    return kResultFalse;
  setParamNormalized(AbNinjamParams::kParamChannelVolumeId, monitorVolumeState);

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
        message->setMessageID(textMessage);
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
tresult PLUGIN_API PlugController::notify(Vst::IMessage *message) {
  L_(ltrace) << "[PlugController] Entering PlugController::notify";
  if (!message)
    return kInvalidArgument;

  if (strcmp(message->getMessageID(), notificationMessage) == 0) {
    String128 notification;
    if (message->getAttributes()->getString(
            "notification", notification, sizeof(notification)) == kResultOk) {
      if (notificationLabel) {
        Steinberg::String str(notification);
        str.toMultiByte(kCP_Utf8);
        notificationLabel->setText(str.text8());
      }
    }
  }

  if (!strcmp(message->getMessageID(), statusMessage)) {
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
          notificationLabel->setText("");
#if !defined(_WIN32)
          menu->setValueNormalized(1.f);
#endif
          break;
        case disconnected:
          cleanNotConnected("");
          break;
        case serverNotProvided:
          cleanNotConnected("Server not provided!");
          break;
        case licenseNotAccepted:
          cleanNotConnected("License not accepted!");
          break;
        case connectionError:
          cleanNotConnected("Connection error!");
          break;
        }
#if !defined(_WIN32)
        menu->valueChanged();
#endif
      }
    }
  }

  if (!strcmp(message->getMessageID(), binaryMessage)) {
    const void *data;
    uint32 size;
    if (message->getAttributes()->getBinary("remoteUsers", data, size) ==
        kResultOk) {
      if (size > 0) {
        remoteUsers = *const_cast<std::vector<Common::RemoteUser> *>(
            static_cast<const std::vector<Common::RemoteUser> *>(data));
        if (menu && abs(menu->getValueNormalized() - 0.5f) <
                        std::numeric_limits<float>::epsilon()) {
          createMixer(vst3Editor);
        }
      } else {
        if (remoteUsers.size() > 0) {
          remoteUsers.clear();
        }
      }
    } else {
      if (remoteUsers.size() > 0) {
        remoteUsers.clear();
      }
    }
  }

  if (strcmp(message->getMessageID(), chatMessage) == 0) {
    String128 chatUpdate;
    if (message->getAttributes()->getString("chatUpdate", chatUpdate,
                                            sizeof(chatUpdate)) == kResultOk) {
      int historyLength = chatHistory.length();
      Steinberg::String str(chatUpdate);
      str.toMultiByte(kCP_Utf8);
      const char8 *text = str.text8();

      if (text && chatController) {
        if (chatHistory.length() > 0) {
          L_(ltrace) << "[PlugController] chatHistory: " << chatHistory;
          chatHistory = chatHistory + "\n" + text;
        } else {
          // received first message
          chatHistory = text;
        }
        if (menu && menu->getValueNormalized() > 0.6f) {
          chatController->displayChatText(chatHistory);
        } else {
          // rough estimate multi line message
          int lines = (chatHistory.length() - historyLength) / 13;
          L_(ltrace) << "[PlugController] chatHistory.length(): "
                     << chatHistory.length();
          L_(ltrace) << "[PlugController] historyLength: " << historyLength;
          L_(ltrace) << "[PlugController] lines: " << lines;
          int lineHeight = 12;
          lastChatTextHolderViewSize.bottom += (lines * lineHeight);
        }
      }
    }
  }

  return EditController::notify(message);
}

//------------------------------------------------------------------------
CView *PlugController::createCustomView(UTF8StringPtr name,
                                        const UIAttributes & /*attributes*/,
                                        const IUIDescription * /*description*/,
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
    //#if !defined(_WIN32)
    //    menu->setVisible(false);
    //#endif
    return menu;
  }

  if (name && strcmp(name, "MixerView") == 0) {

    CRect scrollViewSize(0, 0, 196, 74);
    CRect containerSize(0, 0, 185, 18);
    int scrollViewStyle = 10;
    mixerScrollView =
        new CScrollView(scrollViewSize, containerSize, scrollViewStyle);

    // Bitmaps
    sliderHandle = new CBitmap("slider_handle.png");
    sliderBackground = new CBitmap("slider_background.png");

    createMixer(editor);
    return mixerScrollView;
  }

  return nullptr;
}

//------------------------------------------------------------------------
void PlugController::willClose(VST3Editor * /*editor*/) {
  L_(ltrace) << "[PlugController] Entering PlugController::willClose";
  notificationLabel = nullptr;
  menu = nullptr;
  mixerScrollView = nullptr;
  sliderHandle = nullptr;
  sliderBackground = nullptr;
}

VSTGUI::CTextLabel *PlugController::createLabel(std::string labelText,
                                                VSTGUI::CFontRef inFontID,
                                                VSTGUI::CRect labelPlacer,
                                                VSTGUI::CColor fontColor) {
  L_(ltrace) << "[PlugController] Entering PlugController::createLabel";
  CTextLabel *label = new CTextLabel(labelPlacer, labelText.c_str());
  label->setFontColor(fontColor);
  label->setFrameColor(CColor(0, 0, 0, 0));
  label->setBackColor(CColor(0, 0, 0, 0));
  label->setHoriAlign(kLeftText);
  label->setFont(inFontID);
  label->setTextTruncateMode(label->kTruncateTail);
  return label;
}

VSTGUI::CSlider *PlugController::createSlider(VSTGUI::CRect sliderPlacer,
                                              int controlTag, float value,
                                              VST3Editor *editor, int userId,
                                              int channelId) {
  L_(ltrace) << "[PlugController] Entering PlugController::createSlider";
  CPoint offsetHandle(0, 2);
  CSlider *slider = new CSlider(sliderPlacer, mixerScrollView, controlTag, 55,
                                163, sliderHandle, sliderBackground);
  slider->setMin(0.f);
  slider->setMax(1.f);

  L_(ltrace) << "[PlugController] userId: " << userId;
  L_(ltrace) << "[PlugController] channelId: " << channelId;
  if (userId >= 0) {
    slider->setAttribute(kCViewUserIdAttrID, userId);
  }
  if (channelId >= 0) {
    slider->setAttribute(kCViewChannelIdAttrID, channelId);
  }

  slider->setOffsetHandle(offsetHandle);
  slider->setValueNormalized(value);

  if (editor) {
    slider->registerControlListener(editor);
  }

  return slider;
}

void PlugController::createMixer(VST3Editor *editor) {
  L_(ltrace) << "[PlugController] Entering PlugController::createMixer";
  if (mixerScrollView) {
    L_(ltrace) << "[PlugController] scrollView available";

    std::vector<CTextLabel *> labels;
    if (mixerScrollView->getChildViewsOfType<CTextLabel>(labels, true) > 0) {
      for (auto label : labels) {
        if (strcmp(label->getText(), "MONITOR") != 0) {
          mixerScrollView->removeView(label);
        }
      }
    }

    std::vector<CSlider *> sliders;
    if (mixerScrollView->getChildViewsOfType<CSlider>(sliders, true) > 0) {
      for (auto slider : sliders) {
        if (slider->getTag() != kParamChannelVolumeId) {
          mixerScrollView->removeView(slider);
        }
      }
    }

    // Scroll to the top to get correct size calculation
    mixerScrollView->resetScrollOffset();

    CRect labelPlacer(5, 18, 54, 36);
    CRect sliderPlacer(55, 36, 185, 54);
    int totalRows = 1;

    // Local channel monitoring
    // Added by uidesc
    //    mixerScrollView->addView(createLabel(
    //        "MONITOR", kNormalFontSmaller, labelPlacer, CColor(120, 186, 52,
    //        255)));
    //    monitorSlider = createSlider(sliderPlacer, kParamChannelVolumeId,
    //                                 localChannelVolume, editor, -1, -1);
    //    mixerScrollView->addView(monitorSlider);

    for (auto remoteUser : remoteUsers) {
      L_(ltrace) << "[PlugController] user: " << remoteUser.name;

      mixerScrollView->addView(createLabel(remoteUser.name, kNormalFontSmall,
                                           labelPlacer,
                                           CColor(143, 140, 97, 255)));
      for (auto channel : remoteUser.channels) {
        L_(ltrace) << "[PlugController] channel: " << channel.name;
        if (!channel.name.empty()) {
          labelPlacer.top += 18;
          labelPlacer.bottom += 18;
          mixerScrollView->addView(createLabel(channel.name, kNormalFontSmaller,
                                               labelPlacer,
                                               CColor(143, 140, 97, 255)));
        }
        mixerScrollView->addView(createSlider(
            sliderPlacer,
            kParamChannelVolumeId + (remoteUser.id * 100) + channel.id + 1,
            channel.volume, editor, remoteUser.id, channel.id));
        sliderPlacer.top += 18;
        sliderPlacer.bottom += 18;
        totalRows++;
      }
      labelPlacer.top += 18;
      labelPlacer.bottom += 18;
      sliderPlacer.top += 18;
      sliderPlacer.bottom += 18;
      totalRows++;
    }
    CRect containerSize(0, 0, 185, totalRows * 18);
    mixerScrollView->setContainerSize(containerSize);
    mixerScrollView->setDirty();
  }
}

VSTGUI::UTF8String PlugController::getChatHistory() {
  L_(ltrace) << "[PlugController] Entering PlugController::getChatHistory";
  return chatHistory;
}

void PlugController::cleanNotConnected(std::string notification) {
  notificationLabel->setText(notification.c_str());
  chatHistory = nullptr;
  lastChatTextHolderViewSize.top = 0;
  lastChatTextHolderViewSize.bottom = 60;
  remoteUsers.clear();
#if !defined(_WIN32)
  menu->setValueNormalized(0.f);
#endif
}
