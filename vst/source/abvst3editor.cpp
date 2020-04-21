#include "../include/abvst3editor.h"
#if !defined(__APPLE__) && !defined(_WIN32)
#include "../include/X11RunLoop.h"
#endif

using namespace VSTGUI;

namespace AbNinjam {

AbVST3Editor::AbVST3Editor(Steinberg::Vst::EditController *editController,
                           UTF8StringPtr templateName,
                           UTF8StringPtr xmlFileName)
    : VST3Editor(editController, templateName, xmlFileName) {}

CMessageResult AbVST3Editor::notify(CBaseObject *sender, IdStringPtr message) {
  if (message == CVSTGUITimer::kMsgTimer) {
    if (doCreateView)
      recreateView();
  }
  CMessageResult result = VSTGUIEditor::notify(sender, message);

  if (result != kMessageNotified)
    return result;

#if !defined(__APPLE__) && !defined(_WIN32)
  if (message == CVSTGUITimer::kMsgTimer) {
    SharedPointer<VSTGUI::RunLoop> runLoop = RunLoop::get();
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

} // namespace AbNinjam
