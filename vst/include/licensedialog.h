#ifndef LICENSEDIALOG_H
#define LICENSEDIALOG_H

#pragma once

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
  void encodeLicenseText(std::string &text);
};

} // namespace abNinjam

#endif // LICENSEDIALOG_H
