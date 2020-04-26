#pragma once

#include "pluginterfaces/base/fplatform.h"

#define MAJOR_VERSION_STR "0"
#define MAJOR_VERSION_INT 0

#define SUB_VERSION_STR "0"
#define SUB_VERSION_INT 0

#define RELEASE_NUMBER_STR "1"
#define RELEASE_NUMBER_INT 1

#define BUILD_NUMBER_STR "1"
#define BUILD_NUMBER_INT 1

// Version with build number (example "1.0.3.342")
#define FULL_VERSION_STR                                                       \
  MAJOR_VERSION_STR "." SUB_VERSION_STR "." RELEASE_NUMBER_STR                 \
                    "." BUILD_NUMBER_STR

// Version without build number (example "1.0.3")
#define VERSION_STR MAJOR_VERSION_STR "." SUB_VERSION_STR "." RELEASE_NUMBER_STR

// HERE you have to define your plug-in, company name, email and web
#define stringPluginName "NINJAM client"

#define stringOriginalFilename "abNinjam.vst3"
#if SMTG_PLATFORM_64
#define stringFileDescription stringPluginName " VST3 (64Bit)"
#else
#define stringFileDescription stringPluginName " VST3"
#endif
#define stringCompanyName "Antanas Bruzas"
#define stringCompanyWeb "http://hippie.lt"
#define stringCompanyEmail "mailto:antanas@hippie.lt"

#define stringLegalCopyright "(c) 2020 Antanas Bruzas"
#define stringLegalTrademarks                                                  \
  "VST is a trademark of Steinberg Media Technologies GmbH"
