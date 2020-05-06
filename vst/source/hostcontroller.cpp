#include "../include/hostcontroller.h"

using namespace abNinjam;

HostController::HostController() { hostPlayingInLastBuffer = false; }

double HostController::getStartPositionForHostSync(
    ProcessContext *processContext) const {
  L_(ltrace) << "[HostController] Entering "
                "HostController::getStartPositionForHostSync";
  double startPosition = 0;
  double samplesPerBeat =
      (60.0 * processContext->sampleRate) / processContext->tempo;
  if (processContext->projectTimeMusic > 0) {
    double cursorPositionInMeasure =
        processContext->projectTimeMusic - processContext->barPositionMusic;
    if (cursorPositionInMeasure > 0.00000001) {
      // shift the interval start to align with host
      double samplesUntilNextMeasure =
          (processContext->timeSigNumerator - cursorPositionInMeasure) *
          samplesPerBeat;
      startPosition = -samplesUntilNextMeasure;
    }
  } else {
    // sometimes host is returning negative values
    startPosition = processContext->projectTimeMusic * samplesPerBeat;
  }

  return startPosition;
}

bool HostController::hostIsPlaying(ProcessContext *processContext) const {
  if (processContext)
    return (processContext->state & processContext->kPlaying) != 0;
  return false;
}
