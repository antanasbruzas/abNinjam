#include "public.sdk/source/main/pluginfactory.h"

#include "../include/plugcontroller.h" // for createInstance
#include "../include/plugids.h"        // for uids
#include "../include/plugprocessor.h"  // for createInstance
#include "../include/version.h"        // for version and naming
#include "common.h"

using namespace AbNinjam;
using namespace Vst3;

#define stringSubCategory                                                      \
  "Fx" // Subcategory for this Plug-in (to be changed if needed, see PlugType in
       // ivstaudioprocessor.h)

BEGIN_FACTORY_DEF(stringCompanyName, stringCompanyWeb, stringCompanyEmail)

DEF_CLASS2(INLINE_UID_FROM_FUID(abNinjamProcessorUID),
           PClassInfo::kManyInstances, // cardinality
           kVstAudioEffectClass, // the component category (do not changed this)
           stringPluginName,     // here the Plug-in name (to be changed)
           Vst::kDistributable,  // means that component and controller could be
                                 // distributed on different computers
           stringSubCategory,    // Subcategory for this Plug-in (to be changed)
           FULL_VERSION_STR,     // Plug-in version (to be changed)
           kVstVersionString, // the VST 3 SDK version (do not changed this, use
                              // always this define)
           PlugProcessor::createInstance) // function pointer called
                                          // when this component
                                          // should be instantiated

DEF_CLASS2(
    INLINE_UID_FROM_FUID(abNinjamControllerUID),
    PClassInfo::kManyInstances,   // cardinality
    kVstComponentControllerClass, // the Controller category (do not changed
                                  // this)
    stringPluginName
    "Controller",     // controller name (could be the same than component name)
    0,                // not used here
    "",               // not used here
    FULL_VERSION_STR, // Plug-in version (to be changed)
    kVstVersionString, // the VST 3 SDK version (do not changed this, use always
                       // this define)
    PlugController::createInstance) // function pointer called when
                                    // this component should be
                                    // instantiated

END_FACTORY

//------------------------------------------------------------------------
//  Module init/exit
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// called after library was loaded
bool InitModule() {
#ifdef LOG_FILE
  TLogLevel logLevel = linfo;
#ifndef NDEBUG
  logLevel = ldebug;
#endif
  path logPath = AbNinjam::Common::getHomePath();
  logPath /= "abNinjam";
  create_directory(logPath);
  if (exists(logPath)) {
    logPath /= "abNinjam.log";
    initLogger(logPath.string().c_str(), logLevel);
    L_(ltrace) << "InitModule";
  } else {
    fprintf(stderr, "Cannot open log file\n");
  }
#endif
  return true;
}

//------------------------------------------------------------------------
// called after library is unloaded
bool DeinitModule() {
  L_(ltrace) << "Entering DeinitModule";
  endLogger();
  return true;
}
