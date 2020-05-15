#ifndef OSCTRANSMITTER_H
#define OSCTRANSMITTER_H

#pragma once

#include "log.h"
#ifdef ABNINJAM_USE_LIBLO
#include <lo/lo.h>
#include <lo/lo_cpp.h>
#endif

using namespace std;

namespace AbNinjam {
namespace Common {

class OscTransmitter {
public:
  OscTransmitter();
  int sendInt(string path, int value);

private:
#ifdef ABNINJAM_USE_LIBLO
  lo::Address oscc;
#endif
};

} // namespace Common
} // namespace AbNinjam

#endif // OSCTRANSMITTER_H
