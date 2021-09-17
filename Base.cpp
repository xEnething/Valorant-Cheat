//=============================================//
#include <stdio.h>
#include <Windows.h>
#include <DbgHelp.h>
#include <dwmapi.h>
#pragma comment(lib, "dwmapi")


#include "xorstr.h"
#include "defs.h"
#include "Menu.h"
#include "Color.h"

#define D3DparamX		, UINT paramx
#define D3DparamvalX	, paramx
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

cFun Fun;
bool Create=true;

auto WINAPI DllMain(const HINSTANCE instance, const DWORD reason, LPVOID reserved) -> bool
{
    //DisableThreadLibraryCalls(hModule);
    if (reason == DLL_PROCESS_ATTACH) {
        if (instance)
            DisableThreadLibraryCalls(instance);
       // AllocConsole();   
       // freopen(xorstr_("CONOUT$"), xorstr_("w"), stdout);
        UWorld = ReadWorld();
        
        /*ULevel = read<uintptr_t>(UWorld + Offsets::oLevel);//page guard

        GameInstance = read<uintptr_t>(UWorld + Offsets::oGameInstance);
        LocalPlayer = read<uintptr_t>(read<uintptr_t>(GameInstance + Offsets::oLocalPlayers));
        PlayerController = read<uintptr_t>(LocalPlayer + Offsets::oPlayerController);

        CameraCache = read<uint64_t>(PlayerController + Offsets::oCameraManager);
        AcknowledgedPawn = read<uintptr_t>(PlayerController + Offsets::oLocalPawn);
        uintptr_t DmgHandlerLocal = read<uintptr_t>(AcknowledgedPawn + Offsets::DamageHandler);
        LocalPlayerState = read<uintptr_t>(AcknowledgedPawn + Offsets::PlayerState);
        auto ActorArray = read<uintptr_t>(ULevel + 0xA0);

        printf("UWorld %p\n", UWorld);
        printf("ULevel %p\n", ULevel);
        printf("GameInstance %p\n", GameInstance);
        printf("LocalPlayer %p\n", LocalPlayer);
        printf("PlayerController %p\n", PlayerController);
        printf("CameraCache %p\n", CameraCache);
        printf("AcknowledgedPawn %p\n", AcknowledgedPawn);
        printf("ActorArray %p\n", ActorArray);
        printf("DmgHandlerLocal %p\n", DmgHandlerLocal);
        printf("LocalPlayerState %p\n", LocalPlayerStat);*/
        SetupDX();
    }
    return 1;
}
