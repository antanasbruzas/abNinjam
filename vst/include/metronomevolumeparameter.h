#ifndef METRONOMEVOLUMEPARAMETER_H
#define METRONOMEVOLUMEPARAMETER_H

#pragma once

#include "base/source/fstring.h"
#include "log.h"
#include "pluginterfaces/base/ustring.h"
#include "public.sdk/source/vst/vstparameters.h"

#include <math.h>

using namespace Steinberg;
using namespace Vst;

namespace abNinjam {

class MetronomeVolumeParameter : public Parameter {
public:
  MetronomeVolumeParameter(int32 flags, ParamID id);

  void toString(ParamValue normValue, String128 string) const SMTG_OVERRIDE;
  bool fromString(const TChar *string,
                  ParamValue &normValue) const SMTG_OVERRIDE;
};

} // namespace abNinjam

#endif // METRONOMEVOLUMEPARAMETER_H
