#ifndef ABVST3EDITOR_H
#define ABVST3EDITOR_H

#pragma once

#include "include/remoteuserchannel.h"
#include "messagetypes.h"
#include "plugcontroller.h"
#include "plugids.h"

#include "vstgui/plugin-bindings/vst3editor.h"
#if !defined(__APPLE__) && !defined(_WIN32)
namespace VSTGUI {
class RunLoop;
}
#endif

using namespace VSTGUI;

namespace AbNinjam {
namespace Vst3 {

class AbVST3Editor : public VST3Editor {
public:
  AbVST3Editor(Steinberg::Vst::EditController *controller,
               UTF8StringPtr templateName, UTF8StringPtr xmlFile);
  CMessageResult notify(CBaseObject *sender, IdStringPtr message) override;
  void valueChanged(CControl *pControl) override;
  void controlEndEdit(VSTGUI::CControl *pControl) override;

private:
  static constexpr VSTGUI::CViewAttributeID kCViewUserIdAttrID = 'uidx';
  static constexpr VSTGUI::CViewAttributeID kCViewChannelIdAttrID = 'cidx';
  PlugController *getController() const {
    return static_cast<PlugController *>(Vst::VSTGUIEditor::getController());
  }
  void sendMixingTouched();
};

} // namespace Vst3
} // namespace AbNinjam

#endif // ABVST3EDITOR_H
