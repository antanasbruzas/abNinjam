#pragma once
#include "ninjamclient.h"
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/lib/controls/ctextedit.h"
#include "vstgui/lib/iviewlistener.h"
#include "vstgui/uidescription/icontroller.h"
#include <array>
//------------------------------------------------------------------------
using namespace Steinberg;
using namespace Vst;

namespace AbNinjam {

//------------------------------------------------------------------------
// AbUIMessageController
//------------------------------------------------------------------------
template <typename ControllerType>
class AbUIMessageController : public VSTGUI::IController,
                              public VSTGUI::ViewListenerAdapter {
public:
  enum Tags { kConnectTag = 1000 };

  NinjamClient *ninjamClient;

  AbUIMessageController(ControllerType *plugController)
      : plugController(plugController), textEdits() {
    ninjamClient = new NinjamClient();
  }
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
    String str(msgText);
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
  void controlEndEdit(CControl *pControl) override {
    if (pControl->getTag() == kConnectTag) {
      if (pControl->getValueNormalized() > 0.5f) {
        fprintf(stderr, "Connect initiated\n");

        for (auto &textEdit : textEdits) {
          if (textEdit) {
            plugController->sendTextMessage(textEdit->getText().data());
          }
        }

        int status = ninjamClient->connect();
        fprintf(stderr, "NinjamClient status: %d\n", status);

        // pControl->setValue(0.f);
        // pControl->invalid();

        //---send a binary message
        //        if (IPtr<IMessage> message =
        //        owned(plugController->allocateMessage())) {
        //          message->setMessageID("BinaryMessage");
        //          uint32 size = 100;
        //          uint32 data[100];
        //          memset(data, 0, size * sizeof(char));
        //          // fill my data with dummy stuff
        //          for (uint32 i = 0; i < size; i++)
        //            data[i] = i;
        //          message->getAttributes()->setBinary("MyData", data, size);
        //          plugController->sendMessage(message);
        //        }
      } else {
        fprintf(stderr, "Disconnect initiated\n");
        ninjamClient->disconnect();
      }
    }
  }
  //--- from IControlListener ----------------------
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
          String str(plugController->getMessageText(i));
          str.toMultiByte(kCP_Utf8);
          if (str.text8()) {
            textEdits[i]->setText(str.text8());
          }
          break;
        }
      }
      // initialize it content
      // String str("TODO");
      // str.toMultiByte(kCP_Utf8);
      // textEdits[0]->setText(str.text8());
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
          String str;
          str.fromUTF8(text.data());
          str.copyTo(messageText, 0, 128);
          plugController->setMessageText(messageText, i);
        }
      }
    }
    //    auto it = std::begin(textEdits);
    //    for (auto &textEdit : textEdits) {
    //      if (textEdit) {
    //        if (dynamic_cast<CTextEdit *>(view) == textEdit) {
    //          // save the last content of the text edit view
    //          const UTF8String &text = textEdit->getText();
    //          String128 messageText;
    //          String str;
    //          str.fromUTF8(text.data());
    //          str.copyTo(messageText, 0, 128);
    //          plugController->setMessageText(messageText, it);
    //        }
    //      }
    //      ++it;
    //    }
  }
  ControllerType *plugController;
  std::array<CTextEdit *, 4> textEdits;
  // CTextEdit *textEdits[4];
};

//------------------------------------------------------------------------
} // namespace AbNinjam
