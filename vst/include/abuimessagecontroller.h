#pragma once
#include "plugids.h"
#include "vstgui/uidescription/icontroller.h"
#include <array>

//------------------------------------------------------------------------
using namespace Steinberg;
using namespace Vst;

namespace AbNinjam {
namespace Vst3 {

//------------------------------------------------------------------------
// AbUIMessageController
//------------------------------------------------------------------------
template <typename ControllerType>
class AbUIMessageController : public VSTGUI::IController,
                              public VSTGUI::ViewListenerAdapter {
public:
  explicit AbUIMessageController(ControllerType *plugController)
      : plugController(plugController), textEdits() {}
  ~AbUIMessageController() override {

    for (auto &textEdit : textEdits) {
      if (textEdit) {
        viewWillDelete(textEdit);
      }
    }
    plugController->removeUIMessageController(this);
  }

  void setMessageText(String128 msgText, unsigned long index) {
    if (!textEdits[index])
      return;
    Steinberg::String str(msgText);
    str.toMultiByte(kCP_Utf8);
    textEdits[index]->setText(str.text8());
  }

private:
  using CControl = VSTGUI::CControl;
  using CView = VSTGUI::CView;
  using CTextEdit = VSTGUI::CTextEdit;
  using UTF8String = VSTGUI::UTF8String;
  using UIAttributes = VSTGUI::UIAttributes;
  using IUIDescription = VSTGUI::IUIDescription;

  //--- from IControlListener ----------------------
  void valueChanged(CControl * /*pControl*/) override {}
  void controlBeginEdit(CControl * /*pControl*/) override {}
  void controlEndEdit(VSTGUI::CControl * /*pControl*/) override {}
  //--- is called when a view is created -----
  CView *verifyView(CView *view, const UIAttributes & /*attributes*/,
                    const IUIDescription * /*description*/) override {

    if (CTextEdit *te = dynamic_cast<CTextEdit *>(view)) {
      // this allows us to keep a pointer of the text edit view
      for (unsigned short i = 0; i < textEdits.size(); i++) {
        if (!textEdits[i]) {

          // pointer is not a valid pointer yet.";
          textEdits[i] = te;
          // add this as listener in order to get viewWillDelete and
          // viewLostFocus calls
          textEdits[i]->registerViewListener(this);

          // initialize content
          Steinberg::String str(plugController->getMessageText(i));
          str.toMultiByte(kCP_Utf8);
          if (str.text8()) {
            textEdits[i]->setText(str.text8());
          }
          break;
        }
      }
    }
    return view;
  }

  //--- from IViewListenerAdapter ----------------------
  //--- is called when a view will be deleted: the editor is closed -----
  void viewWillDelete(CView *view) override {
    for (auto &textEdit : textEdits) {
      if (textEdit) {
        if (dynamic_cast<CTextEdit *>(view) == textEdit) {
          textEdit->unregisterViewListener(this);
          textEdit = nullptr;
        }
      }
    }
  }
  //--- is called when the view is loosing the focus -----------------
  void viewLostFocus(CView *view) override {

    for (unsigned short i = 0; i < textEdits.size(); i++) {
      if (textEdits[i]) {
        if (dynamic_cast<CTextEdit *>(view) == textEdits[i]) {
          // save the last content of the text edit view
          const UTF8String &text = textEdits[i]->getText();
          String128 messageText;
          Steinberg::String str;
          str.fromUTF8(text.data());
          str.copyTo(messageText, 0, 128);
          plugController->setMessageText(messageText, i);
        }
      }
    }
  }
  ControllerType *plugController;
  std::array<CTextEdit *, 3> textEdits;
};

//------------------------------------------------------------------------
} // namespace Vst3
} // namespace AbNinjam
