#pragma once

#include "base/source/fobject.h"
#include "include/log.h"
#include "plugids.h"
#include "public.sdk/source/vst/vsteditcontroller.h"
#include "public.sdk/source/vst/vstparameters.h"
#include "vstgui/lib/vstguifwd.h"
#include "vstgui/uidescription/icontroller.h"
#include <functional>
#include <vector>

//------------------------------------------------------------------------
namespace AbNinjam {
namespace Vst3 {

using SizeFactors = std::vector<float>;
static const SizeFactors kSizeFactors = {1, 1.5, 2, 2.5, 3};

class EditorSizeController : public Steinberg::FObject,
                             public VSTGUI::IController {
public:
  //------------------------------------------------------------------------
  using SizeFunc = std::function<void(float)>;
  EditorSizeController(Steinberg::Vst::EditController *editController,
                       const SizeFunc &sizeFunc, double currentSizeFactor);
  virtual ~EditorSizeController();

  void PLUGIN_API update(FUnknown *changedUnknown, int32 message) override;
  VSTGUI::CView *verifyView(VSTGUI::CView *view,
                            const VSTGUI::UIAttributes &attributes,
                            const VSTGUI::IUIDescription *description) override;
  void valueChanged(VSTGUI::CControl *pControl) override;
  void controlBeginEdit(VSTGUI::CControl *pControl) override;
  void controlEndEdit(VSTGUI::CControl *pControl) override;

  void setSizeFactor(double factor);

  OBJ_METHODS(EditorSizeController, Steinberg::FObject)
  //------------------------------------------------------------------------
private:
  VSTGUI::CControl *sizeControl = nullptr;
  Steinberg::Vst::RangeParameter *sizeParameter = nullptr;
  SizeFunc sizeFunc;
};

//------------------------------------------------------------------------
} // namespace Vst3
} // namespace AbNinjam
