void  EraseHeaders(HINSTANCE hModule1)
{

	PIMAGE_DOS_HEADER pDoH; 
	PIMAGE_NT_HEADERS pNtH;
	DWORD i, ersize, protect;

	if (!hModule1) return;

	pDoH = (PIMAGE_DOS_HEADER)(hModule1);

	pNtH = (PIMAGE_NT_HEADERS)((LONG)hModule1 + ((PIMAGE_DOS_HEADER)hModule1)->e_lfanew);

	ersize = sizeof(IMAGE_DOS_HEADER);
	if ( VirtualProtect(pDoH, ersize, PAGE_READWRITE, &protect) )
	{
		for ( i=0; i < ersize; i++ )
				*(BYTE*)((BYTE*)pDoH + i) = 0;
	}

	ersize = sizeof(IMAGE_NT_HEADERS);
	if ( pNtH && VirtualProtect(pNtH, ersize, PAGE_READWRITE, &protect) )
	{
		for ( i=0; i < ersize; i++ )
				*(BYTE*)((BYTE*)pNtH + i) = 0;
	}
	return;
}

typedef struct offsets_ 
{
	std::ptrdiff_t bone_component = 0x548;
	std::ptrdiff_t mesh_component = 0x410;
	std::ptrdiff_t root_component = 0x210;
	std::ptrdiff_t persistent_level = 0x38;
	std::ptrdiff_t owning_game_instance = 0x1a8;
	std::ptrdiff_t player_state = 0x3d0;
	std::ptrdiff_t team_component = 0x580;
	std::ptrdiff_t team = 0xf8;
	std::ptrdiff_t damage_handler = 0x978;
	std::ptrdiff_t cached_life = 0x1b0;
	std::ptrdiff_t relative_location = 0x164;
	std::ptrdiff_t relative_rotation = 0x170;
	std::ptrdiff_t player_controller = 0x38;
	std::ptrdiff_t acknowledged_pawn = 0x440;
	std::ptrdiff_t player_camera_manager = 0x458;
	std::ptrdiff_t camera_cache_private = 0x1f80;
	std::ptrdiff_t control_rotation = 0x420;
}

typedef struct _UNICODE_STRING 
{
	USHORT Length;
	USHORT MaximumLength;
	PWSTR Buffer;
} UNICODE_STRING, *PUNICODE_STRING;

typedef struct _ModuleInfoNode
{
	LIST_ENTRY LoadOrder;
	LIST_ENTRY InitOrder;
	LIST_ENTRY MemoryOrder;
	HMODULE baseAddress;
	unsigned long entryPoint;
	unsigned int size;
	UNICODE_STRING fullPath;
	UNICODE_STRING name;
	unsigned long flags;
	unsigned short LoadCount;
	unsigned short TlsIndex;
	LIST_ENTRY HashTable;
	unsigned long timestamp;
} ModuleInfoNode, *pModuleInfoNode;

typedef struct _ProcessModuleInfo
{
	unsigned int size;
	unsigned int initialized;
	HANDLE SsHandle;
	LIST_ENTRY LoadOrder;
	LIST_ENTRY InitOrder;
	LIST_ENTRY MemoryOrder;
} ProcessModuleInfo, *pProcessModuleInfo;

#define UNLINK(x) (x).Blink->Flink = (x).Flink; \
	(x).Flink->Blink = (x).Blink;

int HideModule( HMODULE hMod )
{
	ProcessModuleInfo *pmInfo;
	ModuleInfoNode *module;

	_asm
	{
		mov eax, fs:[18h]
		mov eax, [eax + 30h]
		mov eax, [eax + 0Ch] 
		mov pmInfo, eax
	}

	module = (ModuleInfoNode *)(pmInfo->LoadOrder.Flink);

	while(module->baseAddress && module->baseAddress != hMod){
		module = (ModuleInfoNode *)(module->LoadOrder.Flink);}

	if(!module->baseAddress){
		return 0;}

	UNLINK(module->LoadOrder);
	UNLINK(module->InitOrder);
	UNLINK(module->MemoryOrder);
	UNLINK(module->HashTable);
	memset(module->fullPath.Buffer, 0, module->fullPath.Length);
	memset(module, 0, sizeof(ModuleInfoNode));

	return 1;
}
