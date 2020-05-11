#pragma once

#include "connectionproperties.h"
#include "hostcontroller.h"
#include "log.h"
#include "ninjamclient.h"
#include "plugids.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "public.sdk/source/vst/vstaudioeffect.h"

#include <array>

using namespace Steinberg;

namespace abNinjam {

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

  /** We want to receive message. */
  tresult PLUGIN_API notify(Vst::IMessage *message) SMTG_OVERRIDE;

  static FUnknown *createInstance(void *) {
    return static_cast<Vst::IAudioProcessor *>(new PlugProcessor());
  }

protected:
  int16 connectParam = 0;
  int16 connectionIndicatorParam = 0;
  Vst::ParamValue metronomeVolumeParam = 0.5;
  bool mBypass = false;

private:
  void connectToServer(int16 value, ConnectionProperties connectionProperties);
  char *tCharToCharPtr(Steinberg::Vst::TChar *tChar);
  NinjamClient *ninjamClient;

  std::array<char *, 3> messageTexts = {strdup(""), strdup(""), strdup("")};
  bool connectedOld;
  String128 hostProductString;
  HostController *hostController;
  bool synced = false;
  bool bpmNotification = false;
  bool clearNotification = true;
  bool notificationCleared = false;
  NinjamClientStatus ninjamClientStatus = disconnected;
};

//------------------------------------------------------------------------
} // namespace abNinjam
