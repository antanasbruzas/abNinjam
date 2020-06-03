#include "../include/volumeparameter.h"

using namespace AbNinjam;
using namespace Vst3;

//------------------------------------------------------------------------
// VolumeParameter Implementation
//------------------------------------------------------------------------
VolumeParameter::VolumeParameter(int32 flags, ParamID id, std::string title) {
  L_(ltrace) << "[VolumeParameter] Entering "
                "VolumeParameter::VolumeParameter";
  Steinberg::UString(info.title, USTRINGSIZE(info.title))
      .assign(USTRING(title.c_str()));
  Steinberg::UString(info.units, USTRINGSIZE(info.units)).assign(USTRING("dB"));

  info.flags = flags;
  info.id = id;
  info.stepCount = 0;
  info.defaultNormalizedValue = 0.5;
  info.unitId = kRootUnitId;

  setNormalized(1.0);
}

//------------------------------------------------------------------------
void VolumeParameter::toString(ParamValue normValue, String128 string) const {
  L_(ltrace) << "[VolumeParameter] Entering "
                "VolumeParameter::toString";
  char text[32];
  if (normValue > 0.0001) {
    sprintf(text, "%.2f", 20 * log10(normValue));
  } else {
    strcpy(text, "-oo");
  }

  Steinberg::UString(string, 128).fromAscii(text);
}

//------------------------------------------------------------------------
bool VolumeParameter::fromString(const TChar *string,
                                 ParamValue &normValue) const {
  L_(ltrace) << "[VolumeParameter] Entering "
                "VolumeParameter::fromString";
  Steinberg::String wrapper(
      const_cast<TChar *>(string)); // don't know buffer size here!
  double tmp = 0.0;
  if (wrapper.scanFloat(tmp)) {
    // allow only values between -oo and 0dB
    if (tmp > 0.0) {
      tmp = -tmp;
    }
    normValue = exp(log(10) * tmp / 20);
    return true;
  }
  return false;
}
