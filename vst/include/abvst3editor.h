#ifndef ABVST3EDITOR_H
#define ABVST3EDITOR_H

#pragma once

#include "plugcontroller.h"
#include "plugids.h"

#include "vstgui/plugin-bindings/vst3editor.h"
#if !defined(__APPLE__) && !defined(_WIN32)
namespace VSTGUI {
class RunLoop;
}
#endif

using namespace VSTGUI;

namespace abNinjam {

class AbVST3Editor : public VST3Editor {
public:
  AbVST3Editor(Steinberg::Vst::EditController *controller,
               UTF8StringPtr templateName, UTF8StringPtr xmlFile);
  CMessageResult notify(CBaseObject *sender, IdStringPtr message) override;
};

} // namespace abNinjam

#endif // ABVST3EDITOR_H
