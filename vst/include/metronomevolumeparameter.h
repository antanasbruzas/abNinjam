#ifndef METRONOMEVOLUMEPARAMETER_H
#define METRONOMEVOLUMEPARAMETER_H

#pragma once

#include "common.h"
#ifdef stricmp
#undef stricmp
#endif
#ifdef strnicmp
#undef strnicmp
#endif
#include "base/source/fstring.h"
#include "pluginterfaces/base/ustring.h"
#include "public.sdk/source/vst/vstparameters.h"

#include <math.h>

using namespace Steinberg;
using namespace Vst;

namespace AbNinjam {
namespace Vst3 {

class MetronomeVolumeParameter : public Parameter {
public:
  MetronomeVolumeParameter(int32 flags, ParamID id);

  void toString(ParamValue normValue, String128 string) const SMTG_OVERRIDE;
  bool fromString(const TChar *string,
                  ParamValue &normValue) const SMTG_OVERRIDE;
};

} // namespace Vst3
} // namespace AbNinjam

#endif // METRONOMEVOLUMEPARAMETER_H
