#ifndef COMMON_H
#define COMMON_H

#pragma once

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#endif

#include "filesystem.hpp"
#include "include/connectionproperties.h"
#include "include/fileutil.h"
#include "include/log.h"
#include "include/ninjamclient.h"
#include "include/ninjamclientstatus.h"

#endif // COMMON_H
