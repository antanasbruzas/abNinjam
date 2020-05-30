#ifndef MESSAGETYPES_H
#define MESSAGETYPES_H

#pragma once

#include "pluginterfaces/base/ftypes.h"

namespace AbNinjam {
namespace Vst3 {

const Steinberg::FIDString textMessage = "TextMessage";
const Steinberg::FIDString statusMessage = "StatusMessage";
const Steinberg::FIDString notificationMessage = "NotificationMessage";
const Steinberg::FIDString binaryMessage = "BinaryMessage";
const Steinberg::FIDString chatMessage = "ChatMessage";
const Steinberg::FIDString mixingTouchedMessage = "MixingTouchedMessage";

} // namespace Vst3
} // namespace AbNinjam

#endif // MESSAGETYPES_H
