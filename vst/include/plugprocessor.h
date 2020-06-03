#pragma once

#include "common.h"
#include "hostcontroller.h"
#include "include/osctransmitter.h"
#include "include/remoteuser.h"
#include "include/remoteuserchannel.h"
#include "messagetypes.h"
#include "plugids.h"
#include "pluginterfaces/base/fstrdefs.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "pluginterfaces/vst/vsttypes.h"
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "public.sdk/source/vst/vstaudioprocessoralgo.h"

#include <array>
#include <vector>

using namespace Steinberg;
using namespace AbNinjam::Common;

namespace AbNinjam {
namespace Vst3 {

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
  Vst::ParamValue monitorVolumeParam = 1;
  bool mBypass = false;

private:
  void connectToServer(int16 value, ConnectionProperties *connectionProperties);
  char *tCharToCharPtr(Steinberg::Vst::TChar *tChar);
  void sendNotification(std::string text);
  void sendChatMessageUpdate(std::string text);
  void doBypass(int32 numChannels, uint32 sampleFramesSize, void **in,
                void **out);
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
  OscTransmitter *oscTransmitter;
  double hostBpm, previousHostBpm;
  float ninjamBpm, previousNinjamBpm;
  ConnectionProperties connectionProperties;
  bool manualMixingTouched = false;
};

//------------------------------------------------------------------------
} // namespace Vst3
} // namespace AbNinjam
