#ifndef REMOTECHANNEL_H
#define REMOTECHANNEL_H

#pragma once

#include <string>

namespace AbNinjam {
namespace Common {

class RemoteChannel {
public:
  RemoteChannel();

  // private:
  int id;
  std::string name;
  float volume;
};

} // namespace Common
} // namespace AbNinjam

#endif // REMOTECHANNEL_H
