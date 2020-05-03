#pragma once

#include "base/source/fstreamer.h"
#include "public.sdk/source/vst/vstguieditor.h"

using namespace Steinberg;

namespace abNinjam {

// HERE are defined the parameter Ids which are exported to the host
enum AbNinjamParams : Vst::ParamID {
  kBypassId = 100,
  kParamConnectId = 1000,
  kParamConnectionIndicatorId = 1001,
  kParamMetronomeVolId = 1002
};

// HERE you have to define new unique class ids: for processor and for
// controller you can use GUID creator tools like https://www.guidgenerator.com/
static const FUID abNinjamProcessorUID(0x24F26E6C, 0xAA204EB0, 0x8188B679,
                                       0x0CABA3AA);
static const FUID abNinjamControllerUID(0x38BA0928, 0x13AE46A0, 0x8419DACB,
                                        0xE8151301);

//------------------------------------------------------------------------
} // namespace abNinjam
