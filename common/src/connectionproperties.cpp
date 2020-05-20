#include "../include/connectionproperties.h"
#include "../include/stringutil.h"
#include <cstring>
#include <fstream>

using namespace AbNinjam::Common;
using namespace std;

ConnectionProperties::ConnectionProperties() {
  L_(ltrace) << "[ConnectionProperties] Entering "
                "ConnectionProperties::ConnectionProperties";
  host = nullptr;
  username = nullptr;
  password = nullptr;
  autoLicenseAgree = false;
  autoRemoteVolume = true;
  autoSyncBpm = true;
}

ConnectionProperties::~ConnectionProperties() {
  L_(ltrace) << "[ConnectionProperties] Entering "
                "ConnectionProperties::~ConnectionProperties";
}

void ConnectionProperties::readFromFile(path path) {
  L_(ltrace)
      << "[ConnectionProperties] Entering ConnectionProperties::readFromFile";
  ifstream ifs(path);
  if (ifs.good()) {
    string line;
    while (getline(ifs, line)) {
      if (line.rfind("host", 0) == 0) {
        if (isEmpty(this->host)) {
          this->host = strdup(line.substr(5, line.length()).c_str());
        }
      }
      if (line.rfind("user", 0) == 0) {
        if (isEmpty(this->username)) {
          this->username = strdup(line.substr(5, line.length()).c_str());
        }
      }
      if (line.rfind("pass", 0) == 0) {
        if (isEmpty(this->password)) {
          this->password = strdup(line.substr(5, line.length()).c_str());
        }
      }
      if (line.rfind("autoLicenseAgree", 0) == 0) {
        istringstream(line.substr(17, line.length())) >> boolalpha >>
            this->autoLicenseAgree;
      }
      if (line.rfind("autoRemoteVolume", 0) == 0) {
        istringstream(line.substr(17, line.length())) >> boolalpha >>
            this->autoRemoteVolume;
      }
      if (line.rfind("autoSyncBpm", 0) == 0) {
        istringstream(line.substr(12, line.length())) >> boolalpha >>
            this->autoSyncBpm;
      }
    }
  }
}
