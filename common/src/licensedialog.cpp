#include "../include/licensedialog.h"

using namespace AbNinjam::Common;

static constexpr auto zenitypath = "/usr/bin/zenity";

LicenseDialog::LicenseDialog() {
  L_(ltrace) << "[LicenseDialog] Entering LicenseDialog::LicenseDialog";
}

LicenseDialog::~LicenseDialog() {
  L_(ltrace) << "[LicenseDialog] Entering LicenseDialog::~LicenseDialog";
  closeProcess();
}

int LicenseDialog::showDialog(const char *licensetext) {
  L_(ltrace) << "[LicenseDialog] Entering LicenseDialog::showDialog";
  //  std::string license = licensetext;
  //  encodeLicenseText(license);
  std::string command = getCommandLocation();
  command +=
      " --title=\"You must agree to this license to connect to this server\"";
  command += " --ok-label=Agree";
  command += " --cancel-label=Disagree";
  command += " --question";
  command += " --no-markup";
#ifdef unix
  command += " --ellipsize";
#endif
  command += " --no-wrap";
  command += " --text=\"";
  command += licensetext;
  command += "\"";

  return startProcess(command.data());
}

int LicenseDialog::startProcess(const char *command) {
  L_(ltrace) << "[LicenseDialog] Entering LicenseDialog::startProcess";
#ifdef _WIN32
  pipe = _popen(command, "r");
#else
  pipe = popen(command, "r");
#endif
  if (!pipe) {
    return 256;
  }
#ifdef _WIN32
  int stat = _pclose(pipe);
#else
  int stat = pclose(pipe);
#endif
  return stat;
}

void LicenseDialog::closeProcess() {
  L_(ltrace) << "[LicenseDialog] Entering LicenseDialog::closeProcess";
  if (pipe) {
#ifdef _WIN32
    _pclose(pipe);
#else
    pclose(pipe);
#endif
  }
  pipe = nullptr;
}

std::string LicenseDialog::getCommandLocation() {
  L_(ltrace) << "[LicenseDialog] Entering LicenseDialog::getCommandLocation";
#ifdef _WIN32
  path commandPath = getHomePath();
  std::ostringstream oss;
  oss << "abNinjam" << separator() << "zenity.exe";
  commandPath /= oss.str();
  if (exists(commandPath)) {
    return commandPath.string();
  }
#elif defined(__APPLE__)
  return "/usr/local/bin/zenity";
#endif
  return zenitypath;
}
