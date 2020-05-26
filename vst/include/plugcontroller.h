#pragma once

#include "common.h"
#include "include/remoteuser.h"
#include "metronomevolumeparameter.h"
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/lib/cbitmap.h"
#include "vstgui/lib/controls/csegmentbutton.h"
#include "vstgui/lib/controls/cslider.h"
#include "vstgui/lib/controls/ctextlabel.h"
#include "vstgui/lib/cscrollview.h"
#include "vstgui/lib/vstguifwd.h"
#include "vstgui/plugin-bindings/vst3editor.h"

#include <array>
#include <vector>

using namespace Steinberg;
using namespace Vst;

namespace AbNinjam {
namespace Vst3 {

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
  using CTextLabel = VSTGUI::CTextLabel;
  using CView = VSTGUI::CView;
  using UIAttributes = VSTGUI::UIAttributes;
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

  //---from ComponentBase-----
  tresult receiveText(const char *text) SMTG_OVERRIDE;

  /** We want to receive status message. */
  tresult PLUGIN_API notify(Vst::IMessage *message) SMTG_OVERRIDE;

  //---from VST3EditorDelegate-----------
  IController *createSubController(UTF8StringPtr name,
                                   const IUIDescription *description,
                                   VST3Editor *editor) SMTG_OVERRIDE;
  CView *createCustomView(UTF8StringPtr name, const UIAttributes &attributes,
                          const IUIDescription *description,
                          VST3Editor *editor) SMTG_OVERRIDE;
  void willClose(VST3Editor *editor) SMTG_OVERRIDE;

  //---Internal functions-------
  void addUIMessageController(UIMessageController *controller);
  void removeUIMessageController(UIMessageController *controller);

  void setMessageText(String128 text, unsigned long index);
  TChar *getMessageText(unsigned long index);
  std::vector<Common::RemoteUser> remoteUsers;

private:
  using UIMessageControllerList = std::vector<UIMessageController *>;
  UIMessageControllerList uiMessageControllers;
  std::array<String128, 3> messageTexts;
  CTextLabel *notificationLabel;
  VSTGUI::CSegmentButton *menu;
  VSTGUI::CScrollView *scrollView;
  VSTGUI::CBitmap *sliderHandle;
  VSTGUI::CBitmap *sliderBackground;
  static constexpr VSTGUI::CViewAttributeID kCViewUserIdAttrID = 'uidx';
  static constexpr VSTGUI::CViewAttributeID kCViewChannelIdAttrID = 'cidx';
  VSTGUI::CTextLabel *createLabel(std::string labelText,
                                  VSTGUI::CFontRef inFontID,
                                  VSTGUI::CRect labelPlacer);
  VSTGUI::CSlider *createSlider(VSTGUI::CRect sliderPlacer, int controlTag,
                                float value, VST3Editor *editor, int userId,
                                int channelId);
  void createMixer(VST3Editor *editor);
  VST3Editor *vst3Editor;
};

//------------------------------------------------------------------------
} // namespace Vst3
} // namespace AbNinjam
