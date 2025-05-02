#include "main.h"
#include "utils.h"
#include <cstdint>
#include <cstdio>
#include <memoryapi.h>
#include <minwindef.h>
#include <processthreadsapi.h>
#include <windows.h>

typedef struct
{
  int enabled;
  short offset;
  size_t patchSize;
  uint8_t const patch[6];
} Patch;

DWORD WINAPI InitThread(LPVOID lpParam)
{
  Sleep(5000);

  FILE *log = fopen(LOG_NAME, "w");

  OWNED_BEDS = ReadIntIniSetting("SleepInOwnedBeds");
  HOSTILE_NEAR = ReadIntIniSetting("SleepHostileActorsNear");
  COMBAT = ReadIntIniSetting("SleepInCombat");
  TRESPASSING = ReadIntIniSetting("SleepTrespassing");

  uintptr_t mainFuncAddr{FindPattern("48 83 EC 40 49 8B ?? 49 8B C8 49 8B F8 48 8B F2 FF 90 48 03")};

  fprintf(log, "Found address: 0x%p\n", (void *)mainFuncAddr);
  fprintf(log, "ONWED_BEDS: %d\n", OWNED_BEDS);
  fprintf(log, "HOSTILE_NEAR: %d\n", HOSTILE_NEAR);
  fprintf(log, "COMBAT: %d\n", COMBAT);
  fprintf(log, "TRESPASSING: %d\n", TRESPASSING);

  fclose(log);

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
extern "C"
{
  OBSEPluginVersionData OBSEPlugin_Version =
      {
          OBSEPluginVersionData::kVersion,

          11,
          "Sleep In Onwed Beds",
          "rootBrz",

          OBSEPluginVersionData::kAddressIndependence_Signatures,
          OBSEPluginVersionData::kStructureIndependence_NoStructs};

  bool OBSEPlugin_Load(const OBSEInterface *obse)
  {
    PLUGIN_HANDLE = obse->GetPluginHandle();
    OBSE_MESSAGE = (OBSEMessagingInterface *)obse->QueryInterface(kInterface_Messaging);

    return true;
  }
};
