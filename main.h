#pragma once

#include "OBSE/obse64/PluginAPI.h"
#include <minwindef.h>

inline int OWNED_BEDS = 1;
inline int HOSTILE_NEAR = 0;
inline int COMBAT = 0;
inline int TRESPASSING = 0;
inline constexpr const char *LOG_NAME = "sleepinownedbeds.log";
inline constexpr const char *INI_NAME = "sleepinownedbeds.ini";
inline OBSEMessagingInterface *OBSE_MESSAGE = nullptr;
inline PluginHandle PLUGIN_HANDLE = kPluginHandle_Invalid;
inline HMODULE DLL_HANDLE = nullptr;

unsigned int __stdcall InitThread(void *param);