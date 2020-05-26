#include "../include/abvst3editor.h"

#if !defined(__APPLE__) && !defined(_WIN32)
#include "../include/X11RunLoop.h"
#endif

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

CMessageResult AbVST3Editor::notify(CBaseObject *sender, IdStringPtr message) {
  // L_(ltrace) << "[AbVST3Editor] Entering AbVST3Editor::notify";
  if (message == CVSTGUITimer::kMsgTimer) {
    if (doCreateView)
      recreateView();
  }
  CMessageResult result = VSTGUIEditor::notify(sender, message);

  if (result != kMessageNotified)
    return result;

#if !defined(__APPLE__) && !defined(_WIN32)
  if (message == CVSTGUITimer::kMsgTimer) {
    SharedPointer<RunLoop> runLoop = RunLoop::get();
    if (runLoop) {
      // note(jpc) I don't find a reliable way to check if the host
      //   notifier of X11 events is working. If there is, remove this and
      //   avoid polluting Linux hosts which implement the loop correctly.
      runLoop->processSomeEvents();

      runLoop->cleanupDeadHandlers();
    }
  }
#endif

  return result;
}

void AbVST3Editor::valueChanged(CControl *pControl) {
  L_(ltrace) << "[AbVST3Editor] Entering AbVST3Editor::valueChanged";
  L_(ltrace) << "[AbVST3Editor] pControl->getTag(): " << pControl->getTag();
  if (pControl->getTag() >= static_cast<signed int>(kParamChannelVolumeId)) {
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
      message->setMessageID("BinaryMessage");
      message->getAttributes()->setBinary(
          "remoteUserChannel", remoteUserChannel, sizeof(RemoteUserChannel));
      controller->sendMessage(message);
    }
  }

  VST3Editor::valueChanged(pControl);
}

void AbVST3Editor::controlEndEdit(VSTGUI::CControl *pControl) {
  L_(ltrace) << "[AbVST3Editor] Entering AbVST3Editor::controlEndEdit";
  if (pControl->getTag() >= static_cast<signed int>(kParamChannelVolumeId)) {
    controller->sendTextMessage("manualMixingTouched");
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
