#include "../include/licensedialog.h"

#include <iostream>

using namespace abNinjam;

static constexpr auto zenitypath = "/usr/bin/zenity";

LicenseDialog::LicenseDialog() {}

LicenseDialog::~LicenseDialog() { closeProcess(); }

int LicenseDialog::showDialog(const char *licensetext) {
  //  std::string license = licensetext;
  //  encodeLicenseText(license);
  std::string command = zenitypath;
  command +=
      " --title=\"You must agree to this license to connect to this server\"";
  command += " --ok-label=Agree";
  command += " --cancel-label=Disagree";
  command += " --question";
  command += " --no-markup";
  command += " --ellipsize";
  command += " --no-wrap";
  command += " --text=\"";
  command += licensetext;
  command += "\"";

  return startProcess(command.data());
}

int LicenseDialog::startProcess(const char *command) {

  pipe = popen(command, "r");
  if (!pipe) {
    return 256;
  }
  int stat = pclose(pipe);
  return stat;
}

void LicenseDialog::closeProcess() {
  if (pipe)
    pclose(pipe);
  pipe = nullptr;
}

void LicenseDialog::encodeLicenseText(std::string &data) {
  std::string buffer;
  buffer.reserve(data.size());
  for (size_t pos = 0; pos != data.size(); ++pos) {
    switch (data[pos]) {
    case '&':
      buffer.append("&amp;");
      break;
    case '\"':
      buffer.append("&quot;");
      break;
    case '\'':
      buffer.append("&apos;");
      break;
    case '<':
      buffer.append("&lt;");
      break;
    case '>':
      buffer.append("&gt;");
      break;
    default:
      buffer.append(&data[pos], 1);
      break;
    }
  }
  data.swap(buffer);
}
