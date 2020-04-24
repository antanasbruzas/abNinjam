#include "../include/plugcontroller.h"
#include "../include/abuimessagecontroller.h"
#include "../include/abvst3editor.h"
#include "../include/plugids.h"

#include "base/source/fstring.h"

namespace AbNinjam {

//-----------------------------------------------------------------------------
tresult PLUGIN_API PlugController::initialize(FUnknown *context) {
  tresult result = EditController::initialize(context);
  if (result == kResultTrue) {
    //---Create Parameters------------
    parameters.addParameter(STR16("Bypass"), nullptr, 1, 0,
                            ParameterInfo::kCanAutomate |
                                ParameterInfo::kIsBypass,
                            AbNinjamParams::kBypassId);

    parameters.addParameter(STR16("Parameter 1"), STR16("dB"), 0, .5,
                            ParameterInfo::kIsReadOnly,
                            AbNinjamParams::kParamVolId, 0, STR16("Param1"));
    parameters.addParameter(STR16("Connect to server"), STR16("On/Off"), 1, 0,
                            ParameterInfo::kCanAutomate,
                            AbNinjamParams::kParamConnectId, 0,
                            STR16("Connect"));

    //---Custom state init------------
    Steinberg::String defaultPort("2049");
    defaultPort.copyTo16(messageTexts[3], 0, 127);
  }
  return kResultTrue;
}

//------------------------------------------------------------------------
IPlugView *PLUGIN_API PlugController::createView(const char *name) {
  // someone wants my editor
  if (name && strcmp(name, "editor") == 0) {
    auto *view = new AbVST3Editor(this, "view", "plug.uidesc");
    return view;
  }
  return nullptr;
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::setComponentState(IBStream *state) {
  // we receive the current state of the component (processor part)
  // we read our parameters and bypass value...
  if (!state)
    return kResultFalse;

  IBStreamer streamer(state, kLittleEndian);

  double savedParam1 = 0.0;
  if (streamer.readDouble(savedParam1) == false)
    return kResultFalse;
  setParamNormalized(AbNinjamParams::kParamVolId, savedParam1);

  int8 connectState = 0;
  if (streamer.readInt8(connectState) == false)
    return kResultFalse;
  setParamNormalized(AbNinjamParams::kParamConnectId, connectState);

  int8 connectionIndicatorState = 0;
  if (streamer.readInt8(connectionIndicatorState) == false)
    return kResultFalse;
  setParamNormalized(AbNinjamParams::kParamConnectionIndicatorId,
                     connectionIndicatorState);

  // read the bypass
  int32 bypassState;
  if (streamer.readInt32(bypassState) == false)
    return kResultFalse;
  setParamNormalized(kBypassId, bypassState ? 1 : 0);

  return kResultOk;
}

//------------------------------------------------------------------------
IController *
PlugController::createSubController(UTF8StringPtr name,
                                    const IUIDescription * /*description*/,
                                    VST3Editor * /*editor*/) {
  if (UTF8StringView(name) == "MessageController") {
    auto *controller = new UIMessageController(this);
    addUIMessageController(controller);
    return controller;
  }
  return nullptr;
}

//------------------------------------------------------------------------
void PlugController::addUIMessageController(UIMessageController *controller) {
  uiMessageControllers.push_back(controller);
}

//------------------------------------------------------------------------
void PlugController::removeUIMessageController(
    UIMessageController *controller) {
  UIMessageControllerList::const_iterator it = std::find(
      uiMessageControllers.begin(), uiMessageControllers.end(), controller);
  if (it != uiMessageControllers.end())
    uiMessageControllers.erase(it);
}

//------------------------------------------------------------------------
tresult PlugController::receiveText(const char *text) {
  // received from Component
  if (text) {
    fprintf(stderr, "[PlugController] received: ");
    fprintf(stderr, "%s", text);
    fprintf(stderr, "\n");
  }
  return kResultOk;
}

//------------------------------------------------------------------------
void PlugController::setMessageText(String128 text, unsigned long index) {
  Steinberg::String tmp(text);
  tmp.copyTo16(messageTexts[index], 0, 127);
}

//------------------------------------------------------------------------
TChar *PlugController::getMessageText(unsigned long index) {
  return messageTexts[index];
}

//------------------------------------------------------------------------
tresult PLUGIN_API PlugController::setState(IBStream *state) {
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

} // namespace AbNinjam
