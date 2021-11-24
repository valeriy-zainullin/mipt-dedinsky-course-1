#pragma once

#include "support/macro_utils.h"

#define TTS_MAX_SAID_STRING_LENGTH_MACRO 1024
#define TTS_MAX_COMMAND_LENGTH_MACRO 2048

void tts_say(const char * format, ...);
