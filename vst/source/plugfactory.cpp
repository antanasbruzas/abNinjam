#include "public.sdk/source/main/pluginfactory.h"

#include "../include/fileutil.h"
#include "../include/plugcontroller.h" // for createInstance
#include "../include/plugids.h"        // for uids
#include "../include/plugprocessor.h"  // for createInstance
#include "../include/version.h"        // for version and naming

#define stringSubCategory                                                      \
  "Fx" // Subcategory for this Plug-in (to be changed if needed, see PlugType in
       // ivstaudioprocessor.h)

BEGIN_FACTORY_DEF(stringCompanyName, stringCompanyWeb, stringCompanyEmail)

DEF_CLASS2(INLINE_UID_FROM_FUID(abNinjam::abNinjamProcessorUID),
           PClassInfo::kManyInstances, // cardinality
           kVstAudioEffectClass, // the component category (do not changed this)
           stringPluginName,     // here the Plug-in name (to be changed)
           Vst::kDistributable,  // means that component and controller could be
                                 // distributed on different computers
           stringSubCategory,    // Subcategory for this Plug-in (to be changed)
           FULL_VERSION_STR,     // Plug-in version (to be changed)
           kVstVersionString, // the VST 3 SDK version (do not changed this, use
                              // always this define)
           abNinjam::PlugProcessor::createInstance) // function pointer called
                                                    // when this component
                                                    // should be instantiated

DEF_CLASS2(
    INLINE_UID_FROM_FUID(abNinjam::abNinjamControllerUID),
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
    abNinjam::PlugController::createInstance) // function pointer called when
                                              // this component should be
                                              // instantiated

END_FACTORY

//------------------------------------------------------------------------
//  Module init/exit
//------------------------------------------------------------------------

//------------------------------------------------------------------------
// called after library was loaded
bool InitModule() {
#ifndef NDEBUG
  path logPath = abNinjam::getHomePath();
  logPath /= "abNinjam";
  create_directory(logPath);
  if (exists(logPath)) {
    logPath /= "abNinjam.log";
    initLogger(logPath.string().c_str(), ldebug);
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
