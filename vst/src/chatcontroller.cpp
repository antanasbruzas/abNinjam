#include "../include/chatcontroller.h"
#include "../include/plugcontroller.h"

using namespace AbNinjam;
using namespace Vst3;
using namespace Common;
using namespace Steinberg::Vst;
using namespace VSTGUI;

ChatController::ChatController(EditController *plugController)
    : plugController(plugController) {}

ChatController::~ChatController() {
  L_(ltrace) << "[ChatController] Entering ChatController::~ChatController";
  if (plugController) {
    if (PlugController *controller =
            dynamic_cast<PlugController *>(plugController)) {
      L_(ltrace) << "[ChatController] plugController available";
      if (chatTextHolder) {
        L_(ltrace) << "[ChatController] chatTextHolder available";
        controller->gsLastChatTextHolderViewSize() =
            chatTextHolder->getViewSize();
        chatTextHolder = nullptr;
      }
    }
  }

  if (chatTextInput) {
    L_(ltrace) << "[ChatController] chatTextInput available";
    viewWillDelete(chatTextInput);
  }
}

CView *ChatController::verifyView(CView *view, const UIAttributes &attributes,
                                  const IUIDescription * /*description*/) {
  L_(ltrace) << "[ChatController] Entering ChatController::verifyView";

  if (CViewContainer *viewContainer = dynamic_cast<CViewContainer *>(view)) {
    if (const auto *attrValue =
            attributes.getAttributeValue(UIViewCreator::kAttrUIDescLabel)) {
      if (attrValue->compare("chat-controller") == 0) {
        L_(ldebug) << "[ChatController] Found chat view container";
        chatViewContainer = viewContainer;
      }
    }
  }

  if (CScrollView *scrollView = dynamic_cast<CScrollView *>(view)) {
    L_(ldebug) << "[ChatController] Found chat scroll view container";
    chatScrollView = scrollView;

    if (PlugController *controller =
            dynamic_cast<PlugController *>(plugController)) {
      auto lastSize = controller->gsLastChatTextHolderViewSize();
      if (lastSize.getHeight() > 0) {
        chatScrollView->setContainerSize(lastSize, true);
      }
    }
  }

  if (CTextEdit *textEdit = dynamic_cast<CTextEdit *>(view)) {
    L_(ldebug) << "[ChatController] Found chat text input";
    chatTextInput = textEdit;
    chatTextInput->registerViewListener(this);
    chatTextInput->takeFocus();
  }

  if (CTextButton *textButton = dynamic_cast<CTextButton *>(view)) {
    L_(ldebug) << "[ChatController] Found send button";
    sendButton = textButton;
    // Do not display send button as hitting return should work fine on it's own
    sendButton->setVisible(false);
  }

  if (CMultiLineTextLabel *multiLineTextLabel =
          dynamic_cast<CMultiLineTextLabel *>(view)) {
    L_(ldebug) << "[ChatController] Found chat text holder";
    chatTextHolder = multiLineTextLabel;

    // initialize content
    if (PlugController *controller =
            dynamic_cast<PlugController *>(plugController)) {
      VSTGUI::UTF8String chatHistory = controller->getChatHistory();
      if (chatHistory.length() > 0) {
        chatTextHolder->setText(chatHistory);

        auto lastSize = controller->gsLastChatTextHolderViewSize();
        if (lastSize.getHeight() > 0) {
          chatTextHolder->setViewSize(lastSize, true);
        }
      } else {
        // TODO: remove scroller
      }
    }
  }

  return view;
}

void ChatController::valueChanged(CControl * /*pControl*/) {
  L_(ltrace) << "[ChatController] Entering ChatController::valueChanged";
}

void ChatController::controlEndEdit(VSTGUI::CControl *pControl) {
  L_(ltrace) << "[ChatController] Entering ChatController::controlEndEdit";
  if (pControl->getTag() == kParamSendChatMessageId) {
    L_(ltrace) << "[ChatController] kParamSendChatMessageId";
    if (pControl->getValueNormalized() > 0.5f) {
      pControl->setValue(0.f);
      pControl->invalid();
      sendChatMessage(chatTextInput->getText().data());
      chatTextInput->setText("");
    }
  }
}

//--- is called when the view is loosing the focus -----------------
void ChatController::viewLostFocus(CView *view) {
  L_(ltrace) << "[ChatController] Entering viewLostFocus";
  if (chatTextInput) {
    if (dynamic_cast<CTextEdit *>(view) == chatTextInput) {
      // Send text message if Return key was pressed
      if (chatTextInput->bWasReturnPressed) {
        sendChatMessage(chatTextInput->getText().data());
        chatTextInput->setText("");
      }
    }
  }
}

//--- is called when a view will be deleted: the editor is closed -----
void ChatController::viewWillDelete(CView *view) {
  L_(ltrace) << "[ChatController] Entering viewWillDelete";
  if (chatTextInput) {
    L_(ltrace) << "[ChatController] chatTextInput available";
    if (dynamic_cast<CTextEdit *>(view) == chatTextInput) {
      chatTextInput->unregisterViewListener(this);
      chatTextInput = nullptr;
    }
  }
}

void ChatController::displayChatText(VSTGUI::UTF8String text) {
  L_(ltrace) << "[ChatController] Entering ChatController::displayChatText";
  L_(ltrace) << "[ChatController] text: " << text;
  if (chatTextHolder) {
    L_(ltrace) << "[ChatController] chatTextHolder is available";
    chatTextHolder->setText(text);
    if (chatScrollView) {
      chatScrollView->setDirty();
    }
#if !defined(_WIN32)
    // FIXME: Somehow Windows hangs everything when trying to take focus
    if (chatTextInput) {
      chatTextInput->takeFocus();
    }
#endif
  }
}

//------------------------------------------------------------------------
void ChatController::sendChatMessage(std::string text) {
  L_(ltrace)
      << "[PlugProcessor] Entering PlugController::sendChatMessageUpdate";
  if (IPtr<IMessage> message = plugController->allocateMessage()) {
    message->setMessageID(chatMessage);
    Steinberg::String str(text.c_str());
    message->getAttributes()->setString("chatMessage", str.text16());
    plugController->sendMessage(message);
  }
}
