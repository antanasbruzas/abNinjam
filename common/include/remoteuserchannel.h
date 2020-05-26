#ifndef REMOTEUSERCHANNEL_H
#define REMOTEUSERCHANNEL_H

#pragma once

namespace AbNinjam {
namespace Common {

class RemoteUserChannel {
public:
  RemoteUserChannel();
  int userId;
  int channelId;
  float volume;
};

} // namespace Common
} // namespace AbNinjam

#endif // REMOTEUSERCHANNEL_H
