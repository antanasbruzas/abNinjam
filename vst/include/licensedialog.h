#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H

#pragma once

#include "fileutil.h"
#include "log.h"
#include <stdio.h>
#include <string>

namespace abNinjam {

class LicenseDialog {
public:
  LicenseDialog();
  ~LicenseDialog();
  int showDialog(const char *licensetext);

private:
  void closeProcess();
  int startProcess(const char *command);
  FILE *pipe{nullptr};
  std::string getCommandLocation();
};

} // namespace abNinjam

#endif // LICENSEDIALOG_H
