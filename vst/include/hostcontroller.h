#ifndef HOSTCONTROLLER_H
#define HOSTCONTROLLER_H

#pragma once

#include "log.h"
#include "pluginterfaces/vst/ivstprocesscontext.h"

using namespace Steinberg;
using namespace Vst;

namespace abNinjam {
class HostController {
public:
  HostController();
  double getStartPositionForHostSync(ProcessContext *processContext) const;
  bool hostPlayingInLastBuffer;
  bool hostIsPlaying(ProcessContext *processContext) const;
};

} // namespace abNinjam

#endif // HOSTCONTROLLER_H
