#include "../include/connectionproperties.h"
#include "../include/stringutil.h"
#include <fstream>
#include <string.h>

using namespace abNinjam;

ConnectionProperties::ConnectionProperties() {}

ConnectionProperties::~ConnectionProperties() {}

void ConnectionProperties::readFromFile(path path) {
  ifstream ifs(path);
  if (ifs.good()) {
    string line;
    while (getline(ifs, line)) {
      if (line.rfind("host", 0) == 0) {
        this->host = strdup(line.substr(5, line.length()).c_str());
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
    }
  }
}
