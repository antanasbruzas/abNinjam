//-----------------------------------------------------------------------------

#include "../include/editorsizecontroller.h"
#include "vstgui/lib/controls/ccontrol.h"

//------------------------------------------------------------------------
using namespace AbNinjam;
using namespace Vst3;

//------------------------------------------------------------------------
EditorSizeController::EditorSizeController(
    Steinberg::Vst::EditController * /*editController*/,
    const SizeFunc &sizeFunc, double currentSizeFactor)
    : sizeFunc(sizeFunc) {
  L_(ltrace) << "[EditorSizeController] Entering "
                "EditorSizeController::EditorSizeController";
  const auto kMaxValue =
      static_cast<Steinberg::Vst::ParamValue>(kSizeFactors.size() - 1);
  L_(ltrace) << "[EditorSizeController] kMaxValue: " << kMaxValue;
  sizeParameter =
      new Steinberg::Vst::RangeParameter(STR("EditorSize"), kParamZoomFactorId,
                                         nullptr, 0, kMaxValue, 0, kMaxValue);
  sizeParameter->setMin(0);
  L_(ltrace) << "[EditorSizeController] kSizeFactors.size() - 1: "
             << kSizeFactors.size() - 1;
  sizeParameter->setMax(kSizeFactors.size() - 1);

  setSizeFactor(currentSizeFactor);

  sizeParameter->addDependent(this);
}

//------------------------------------------------------------------------
EditorSizeController::~EditorSizeController() {
  L_(ltrace) << "[EditorSizeController] Entering "
                "EditorSizeController::~EditorSizeController";
  if (sizeParameter)
    sizeParameter->removeDependent(this);
}

//------------------------------------------------------------------------
void PLUGIN_API EditorSizeController::update(FUnknown *changedUnknown,
                                             int32 /*message*/) {
  L_(ltrace) << "[EditorSizeController] Entering "
                "EditorSizeController::update";
  auto *param = FCast<Steinberg::Vst::Parameter>(changedUnknown);
  L_(ltrace) << "[EditorSizeController] param->getInfo().id: "
             << param->getInfo().id;
  if (param && param->getInfo().id == kParamZoomFactorId) {
    size_t index = static_cast<size_t>(param->toPlain(param->getNormalized()));
    L_(ltrace) << "[EditorSizeController] param->getNormalized():"
               << param->getNormalized();
    L_(ltrace)
        << "[EditorSizeController] param->toPlain(param->getNormalized()):"
        << param->toPlain(param->getNormalized());
    L_(ltrace) << "[EditorSizeController] index:" << index;
    if (sizeFunc)
      sizeFunc(kSizeFactors.at(index));
  }
}

//------------------------------------------------------------------------
VSTGUI::CView *EditorSizeController::verifyView(
    VSTGUI::CView *view, const VSTGUI::UIAttributes & /*attributes*/,
    const VSTGUI::IUIDescription * /*description*/) {
  L_(ltrace) << "[EditorSizeController] Entering "
                "EditorSizeController::verifyView";
  auto *optionMenu = dynamic_cast<VSTGUI::COptionMenu *>(view);
  if (optionMenu) {
    optionMenu->addEntry("100%");
    optionMenu->addEntry("150%");
    optionMenu->addEntry("200%");
    optionMenu->addEntry("250%");
    optionMenu->addEntry("300%");
    sizeControl = optionMenu;
    sizeControl->setValueNormalized(sizeParameter->getNormalized());
    sizeControl->setListener(this);
    sizeParameter->deferUpdate();
  }
  return view;
}

//------------------------------------------------------------------------
void EditorSizeController::valueChanged(VSTGUI::CControl *pControl) {
  L_(ltrace) << "[EditorSizeController] Entering "
                "EditorSizeController::valueChanged";
  if (!pControl)
    return;

  L_(ltrace) << "[EditorSizeController] pControl->getValue():"
             << pControl->getValue();

  auto normValue =
      static_cast<Steinberg::Vst::ParamValue>(pControl->getValue()) /
      sizeParameter->getMax();
  L_(ltrace) << "[EditorSizeController] normValue:" << normValue;
  sizeParameter->setNormalized(normValue);
  L_(ltrace) << "[EditorSizeController] sizeParameter->getNormalized():"
             << sizeParameter->getNormalized();
}

//------------------------------------------------------------------------
void EditorSizeController::controlBeginEdit(VSTGUI::CControl *pControl) {
  L_(ltrace) << "[EditorSizeController] Entering "
                "EditorSizeController::controlBeginEdit";
  if (!pControl)
    return;
}

//------------------------------------------------------------------------
void EditorSizeController::controlEndEdit(VSTGUI::CControl *pControl) {
  L_(ltrace) << "[EditorSizeController] Entering "
                "EditorSizeController::controlEndEdit";
  if (!pControl)
    return;
}

//------------------------------------------------------------------------
void EditorSizeController::setSizeFactor(double factor) {
  L_(ltrace) << "[EditorSizeController] Entering "
                "EditorSizeController::setSizeFactor";
  if (!sizeParameter)
    return;
  auto iter = std::find(kSizeFactors.begin(), kSizeFactors.end(), factor);
  if (iter != kSizeFactors.end()) {
    sizeParameter->setNormalized(
        sizeParameter->toNormalized(iter - kSizeFactors.begin()));
    if (sizeControl) {
      L_(ltrace) << "[EditorSizeController] sizeParameter->getNormalized():"
                 << sizeParameter->getNormalized();
      sizeControl->setValueNormalized(sizeParameter->getNormalized());
    }
  }
}

//------------------------------------------------------------------------
