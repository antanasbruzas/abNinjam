#ifndef HOSTCONTROLLER_H
#define HOSTCONTROLLER_H

#pragma once

#include "common.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"

using namespace Steinberg;
using namespace Vst;

namespace AbNinjam {
namespace Vst3 {

class HostController {
public:
  HostController();
  double getStartPositionForHostSync(ProcessContext *processContext) const;
  bool hostPlayingInLastBuffer;
  bool hostIsPlaying(ProcessContext *processContext) const;
};

} // namespace Vst3
} // namespace AbNinjam

#endif // HOSTCONTROLLER_H
