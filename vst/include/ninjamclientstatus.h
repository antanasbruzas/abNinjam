#ifndef NINJAMCLIENTSTATUS_H
#define NINJAMCLIENTSTATUS_H

namespace abNinjam {

enum NinjamClientStatus {
  ok = 0,
  disconnected = 220,
  serverNotProvided = 221,
  licenseNotAccepted = 222,
  connectionError = 223
};

}

#endif // NINJAMCLIENTSTATUS_H
