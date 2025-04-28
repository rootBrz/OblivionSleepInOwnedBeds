#include "main.h"
#include "utils.h"
#include <cstdint>
#include <memoryapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>

typedef struct
{
  int enabled;
  short offset;
  size_t patchSize;
  uint8_t const patch[6];
} Patch;

DWORD WINAPI InitThread(LPVOID lpParam)
{
  // If OBSE initialized, do not run from PROCESS_ATTACH
  if (lpParam && OBSE_MESSAGE)
    return true;

  constexpr char pattern[]{"48 83 EC 40 49 8B ?? 49 8B C8 49 8B F8 48 8B F2 FF 90 48 03 ?? ??"};
  uintptr_t mainFuncAddr{FindPattern(pattern, sizeof(pattern) / 3)};
  SaveAddressToFile(mainFuncAddr);

  OWNED_BEDS = ReadIntIniSetting("SleepInOwnedBeds");
  HOSTILE_NEAR = ReadIntIniSetting("SleepHostileActorsNear");
  COMBAT = ReadIntIniSetting("SleepInCombat");
  TRESPASSING = ReadIntIniSetting("SleepTrespassing");

  LogToFile("ONWED_BEDS: %d\n", OWNED_BEDS);
  LogToFile("HOSTILE_NEAR: %d\n", HOSTILE_NEAR);
  LogToFile("COMBAT: %d\n", COMBAT);
  LogToFile("TRESPASSING: %d\n", TRESPASSING);

  Patch options[] =
      {{OWNED_BEDS, 0x153, 1, {0xEB}},
       {HOSTILE_NEAR, 0x130, 2, {0x90, 0x90}},
       {COMBAT, 0xFF, 6, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90}},
       {TRESPASSING, 0xE9, 6, {0x90, 0x90, 0x90, 0x90, 0x90, 0x90}}};

  for (const auto &option : options)
    if (option.enabled)
      WriteProcessMemory(GetCurrentProcess(), (LPVOID)(mainFuncAddr + option.offset), option.patch, option.patchSize, NULL);

  return true;
}

// OBSE
void MessageHandler(OBSEMessagingInterface::Message *msg)
{
  if (msg->type == OBSEMessagingInterface::kMessage_PostPostLoad)
    InitThread(nullptr);
}
extern "C"
{
  __declspec(dllexport) OBSEPluginVersionData OBSEPlugin_Version =
      {
          OBSEPluginVersionData::kVersion,

          10,
          "Sleep In Onwed Beds",
          "rootBrz",

          OBSEPluginVersionData::kAddressIndependence_Signatures,
          OBSEPluginVersionData::kStructureIndependence_NoStructs};

  __declspec(dllexport) bool OBSEPlugin_Load(const OBSEInterface *obse)
  {
    PLUGIN_HANDLE = obse->GetPluginHandle();
    OBSE_MESSAGE = (OBSEMessagingInterface *)obse->QueryInterface(kInterface_Messaging);
    OBSE_MESSAGE->RegisterListener(PLUGIN_HANDLE, "OBSE", MessageHandler);

    return true;
  }
};
