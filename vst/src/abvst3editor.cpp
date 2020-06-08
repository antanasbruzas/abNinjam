#include "../include/abvst3editor.h"

using namespace VSTGUI;
using namespace AbNinjam;
using namespace Vst3;
using namespace Common;

AbVST3Editor::AbVST3Editor(Steinberg::Vst::EditController *editController,
                           UTF8StringPtr templateName,
                           UTF8StringPtr xmlFileName)
    : VST3Editor(editController, templateName, xmlFileName) {
  L_(ltrace) << "[AbVST3Editor] Entering AbVST3Editor::AbVST3Editor";
}

void AbVST3Editor::valueChanged(CControl *pControl) {
  L_(ltrace) << "[AbVST3Editor] Entering AbVST3Editor::valueChanged";
  L_(ltrace) << "[AbVST3Editor] pControl->getTag(): " << pControl->getTag();
  // Remote channels
  if (pControl->getTag() > static_cast<signed int>(kParamChannelVolumeId)) {
    RemoteUserChannel *remoteUserChannel = new RemoteUserChannel();
    pControl->getAttribute(kCViewUserIdAttrID, remoteUserChannel->userId);
    pControl->getAttribute(kCViewChannelIdAttrID, remoteUserChannel->channelId);
    remoteUserChannel->volume = pControl->getValueNormalized();
    L_(ltrace) << "[AbVST3Editor] remoteUserChannel->userId: "
               << remoteUserChannel->userId;
    L_(ltrace) << "[AbVST3Editor] remoteUserChannel->channelId: "
               << remoteUserChannel->channelId;
    L_(ltrace) << "[AbVST3Editor] remoteUserChannel->volume: "
               << remoteUserChannel->volume;

    if (IPtr<IMessage> message = controller->allocateMessage()) {
      message->setMessageID(binaryMessage);
      message->getAttributes()->setBinary(
          "remoteUserChannel", remoteUserChannel, sizeof(RemoteUserChannel));
      controller->sendMessage(message);
    }
  }

  VST3Editor::valueChanged(pControl);
}

void AbVST3Editor::controlEndEdit(VSTGUI::CControl *pControl) {
  L_(ltrace) << "[AbVST3Editor] Entering AbVST3Editor::controlEndEdit";
  // Remote channels
  if (pControl->getTag() > static_cast<signed int>(kParamChannelVolumeId)) {
    // Notify processor through controller
    sendMixingTouched();
    // Update channel volumes in the controller
    int userId, channelId;
    pControl->getAttribute(kCViewUserIdAttrID, userId);
    pControl->getAttribute(kCViewChannelIdAttrID, channelId);

    auto it = find_if(
        getController()->remoteUsers.begin(),
        getController()->remoteUsers.end(),
        [&userId](const Common::RemoteUser &ru) { return ru.id == userId; });
    if (it != getController()->remoteUsers.end()) {
      auto index = std::distance(getController()->remoteUsers.begin(), it);
      auto iterator = find_if(
          getController()->remoteUsers.at(index).channels.begin(),
          getController()->remoteUsers.at(index).channels.end(),
          [&channelId](const RemoteChannel &rc) { return rc.id == channelId; });

      if (iterator != getController()->remoteUsers.at(index).channels.end()) {
        iterator->volume = pControl->getValueNormalized();
      }
    }
  }
  VST3Editor::controlEndEdit(pControl);
}

//------------------------------------------------------------------------
void AbVST3Editor::sendMixingTouched() {
  L_(ltrace) << "[PlugProcessor] Entering AbVST3Editor::sendMixingTouched";
  if (IPtr<IMessage> message = controller->allocateMessage()) {
    message->setMessageID(mixingTouchedMessage);
    message->getAttributes()->setInt("manualMixingTouched", 1);
    controller->sendMessage(message);
  }
}

//-----------------------------------------------------------------------------
Steinberg::tresult PLUGIN_API AbVST3Editor::canResize() { return kResultFalse; }
