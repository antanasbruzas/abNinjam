#ifndef CHATCONTROLLER_H
#define CHATCONTROLLER_H

#pragma once
#include "common.h"
#include "messagetypes.h"
#include "plugids.h"
#include "vstgui/lib/vstkeycode.h"
#include "vstgui/uidescription/detail/uiviewcreatorattributes.h"
#include "vstgui/uidescription/icontroller.h"
#include "vstgui/uidescription/uiattributes.h"

namespace AbNinjam {
namespace Vst3 {

class ChatController : public VSTGUI::IController,
                       public VSTGUI::ViewListenerAdapter {
public:
  explicit ChatController(Steinberg::Vst::EditController *plugController);
  ~ChatController() override;
  VSTGUI::CView *
  verifyView(VSTGUI::CView *view, const VSTGUI::UIAttributes &attributes,
             const VSTGUI::IUIDescription * /*description*/) override;
  //--- from IControlListener ----------------------
  void valueChanged(VSTGUI::CControl * /*pControl*/) override;
  void controlEndEdit(VSTGUI::CControl *pControl) override;

  //--- from IViewListenerAdapter ----------------------
  void viewLostFocus(VSTGUI::CView *view) override;
  void viewWillDelete(VSTGUI::CView *view) override;

  void displayChatText(VSTGUI::UTF8String text);
  void sendChatMessage(std::string text);

private:
  VSTGUI::CViewContainer *chatViewContainer;
  VSTGUI::CScrollView *chatScrollView;
  VSTGUI::CTextEdit *chatTextInput;
  VSTGUI::CTextButton *sendButton;
  VSTGUI::CMultiLineTextLabel *chatTextHolder;
  Steinberg::Vst::EditController *plugController;
};

} // namespace Vst3
} // namespace AbNinjam
#endif // CHATCONTROLLER_H
