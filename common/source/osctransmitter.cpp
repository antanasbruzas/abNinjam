#include "../include/osctransmitter.h"

using namespace AbNinjam::Common;

#ifdef ABNINJAM_USE_LIBLO
OscTransmitter::OscTransmitter() : oscc("localhost", "8000") {}
#else
OscTransmitter::OscTransmitter() {}
#endif

int OscTransmitter::sendInt(string path, int value) {
  L_(ltrace) << "[OscTransmitter] Entering OscTransmitter::send";
#ifdef ABNINJAM_USE_LIBLO
  L_(ldebug) << "[OscTransmitter] Sending OSC message";
  return oscc.send(path, "i", value);
#else
  L_(ldebug) << "[OscTransmitter] Sending OSC message is not implemented!";
  // TODO: implement alternatives
  return -1;
#endif
}
