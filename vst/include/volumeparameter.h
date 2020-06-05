#ifndef VOLUMEPARAMETER_H
#define VOLUMEPARAMETER_H

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

#include <cmath>

using namespace Steinberg;
using namespace Vst;

namespace AbNinjam {
namespace Vst3 {

class VolumeParameter : public Parameter {
public:
  VolumeParameter(int32 flags, ParamID id, std::string title);

  void toString(ParamValue normValue, String128 string) const SMTG_OVERRIDE;
  bool fromString(const TChar *string,
                  ParamValue &normValue) const SMTG_OVERRIDE;
};

} // namespace Vst3
} // namespace AbNinjam

#endif // VOLUMEPARAMETER_H
