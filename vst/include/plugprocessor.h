#pragma once

#include "public.sdk/source/vst/vstaudioeffect.h"

using namespace Steinberg;

namespace AbNinjam {

//-----------------------------------------------------------------------------
class PlugProcessor : public Vst::AudioEffect {
public:
  PlugProcessor();

  tresult PLUGIN_API initialize(FUnknown *context) SMTG_OVERRIDE;
  tresult PLUGIN_API setBusArrangements(Vst::SpeakerArrangement *inputs,
                                        int32 numIns,
                                        Vst::SpeakerArrangement *outputs,
                                        int32 numOuts) SMTG_OVERRIDE;

  tresult PLUGIN_API setupProcessing(Vst::ProcessSetup &setup) SMTG_OVERRIDE;
  tresult PLUGIN_API setActive(TBool state) SMTG_OVERRIDE;
  tresult PLUGIN_API process(Vst::ProcessData &data) SMTG_OVERRIDE;

  //------------------------------------------------------------------------
  tresult PLUGIN_API setState(IBStream *state) SMTG_OVERRIDE;
  tresult PLUGIN_API getState(IBStream *state) SMTG_OVERRIDE;

  /** Test of a communication channel between controller and component */
  tresult receiveText(const char *text) SMTG_OVERRIDE;

  static FUnknown *createInstance(void *) {
    return static_cast<Vst::IAudioProcessor *>(new PlugProcessor());
  }

protected:
  Vst::ParamValue mParam1 = 0;
  int16 connectParam = 0;
  int16 connectionIndicatorParam = 0;
  bool mBypass = false;
};

//------------------------------------------------------------------------
} // namespace AbNinjam
