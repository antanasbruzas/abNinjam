#ifndef REMOTEUSER_H
#define REMOTEUSER_H

#pragma once

#include "remotechannel.h"
#include <string>
#include <vector>

namespace AbNinjam {
namespace Common {

class RemoteUser {
public:
  RemoteUser();

  // private:
  int id;
  std::string name;
  std::vector<RemoteChannel> channels;
};

} // namespace Common
} // namespace AbNinjam

#endif // REMOTEUSER_H
