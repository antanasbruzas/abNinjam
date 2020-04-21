#pragma once

#include "public.sdk/source/vst/vsteditcontroller.h"
#include "vstgui/plugin-bindings/vst3editor.h"

#include <vector>

using namespace Steinberg;
using namespace Vst;

namespace AbNinjam {

//-----------------------------------------------------------------------------
class PlugController : public EditController,
                       public VSTGUI::VST3EditorDelegate {
public:
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
  tresult PLUGIN_API setComponentState(IBStream *state) SMTG_OVERRIDE;
};

//------------------------------------------------------------------------
} // namespace AbNinjam
