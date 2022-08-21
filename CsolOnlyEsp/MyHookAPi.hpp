#pragma once
#include <Windows.h>
#include <detours.h>
#include <unordered_map>
using namespace std;
namespace HookMessage {
	unordered_map<PVOID, PVOID> OringMap;
	template<typename Fn>
	void Install(Fn HookDir, Fn NewFuns) {
		Begin(HookDir, NewFuns);
		OringMap[reinterpret_cast<PVOID>(NewFuns)] = reinterpret_cast<PVOID>(HookDir);
	}
	template<typename Fn>
	void Begin(Fn& Funs, Fn NewFuns) {
		DetourTransactionBegin();
		DetourUpdateThread(GetCurrentThread());
		DetourAttach(&(PVOID&)Funs, NewFuns);
		DetourTransactionCommit();
	}
	template<typename Fn>
	_NODISCARD Fn GetOring(Fn Funs) noexcept{
		if (!OringMap.count(reinterpret_cast<PVOID>(Funs))) {
			printf("Shit Api Can't Find Old");
		}
		return (Fn)(OringMap[reinterpret_cast<PVOID>(Funs)]);
	}
}