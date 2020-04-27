#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/plugin-bindings/vst3editor.h"

#include <array>
#include <vector>

using namespace Steinberg;
using namespace Vst;

namespace abNinjam {

template <typename T> class AbUIMessageController;

//-----------------------------------------------------------------------------
class PlugController : public EditController,
                       public VSTGUI::VST3EditorDelegate {
public:
  using UIMessageController = AbUIMessageController<PlugController>;
  using UTF8StringPtr = VSTGUI::UTF8StringPtr;
  using IUIDescription = VSTGUI::IUIDescription;
  using IController = VSTGUI::IController;
  using VST3Editor = VSTGUI::VST3Editor;
  //------------------------------------------------------------------------
  // create function required for Plug-in factory,
  // it will be called to create new instances of this controller
  //------------------------------------------------------------------------
  static FUnknown *createInstance(void *) {
    return static_cast<IEditController *>(new PlugController());
  }

  //---from IPluginBase--------
  tresult PLUGIN_API initialize(FUnknown *context) SMTG_OVERRIDE;

  //---from EditController-----
  IPlugView *PLUGIN_API createView(const char *name) SMTG_OVERRIDE;
  tresult PLUGIN_API setState(IBStream *state) SMTG_OVERRIDE;
  tresult PLUGIN_API getState(IBStream *state) SMTG_OVERRIDE;
  tresult PLUGIN_API setComponentState(IBStream *state) SMTG_OVERRIDE;

  //---from VST3EditorDelegate-----------
  IController *createSubController(UTF8StringPtr name,
                                   const IUIDescription *description,
                                   VST3Editor *editor) SMTG_OVERRIDE;

  //---Internal functions-------
  void addUIMessageController(UIMessageController *controller);
  void removeUIMessageController(UIMessageController *controller);

  //---from ComponentBase-----
  tresult receiveText(const char *text) SMTG_OVERRIDE;

  void setMessageText(String128 text, unsigned long index);
  TChar *getMessageText(unsigned long index);

private:
  using UIMessageControllerList = std::vector<UIMessageController *>;
  UIMessageControllerList uiMessageControllers;
  std::array<String128, 3> messageTexts;
};

//------------------------------------------------------------------------
} // namespace abNinjam
