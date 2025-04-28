#include "main.h"
#include <libloaderapi.h>
#include <processthreadsapi.h>

BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call,
                      LPVOID lpReserved)
{
  if (ul_reason_for_call == DLL_PROCESS_ATTACH)
  {
    DLL_HANDLE = hModule;
    DisableThreadLibraryCalls(hModule);
    CreateThread(NULL, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(InitThread),
                 (LPVOID)1, 0, NULL);
  }
  return TRUE;
}
