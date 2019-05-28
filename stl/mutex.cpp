/*######   Copyright (c) 2013-2015 Ufasoft  http://ufasoft.com  mailto:support@ufasoft.com,  Sergey Pavlov  mailto:dev@ufasoft.com ####
#                                                                                                                                     #
# 		See LICENSE for licensing information                                                                                         #
#####################################################################################################################################*/

#include <el/ext.h>

#if UCFG_WIN32
#	include <windows.h>
#endif


#pragma warning(disable: 4073)
#pragma init_seg(lib)


namespace ExtSTL  {
using namespace Ext;

#ifdef EXT_ONCE_BASE_DEFINED

static mutex s_mtxCallOnce;

#if UCFG_WIN32

typedef BOOL (WINAPI *PFN_InitOnceExecuteOnce)(PINIT_ONCE, PINIT_ONCE_FN, PVOID, LPVOID *);
    
static PFN_InitOnceExecuteOnce s_pfnInitOnceExecuteOnce;

static BOOL WINAPI InitOnceFun(PINIT_ONCE InitOnce, PVOID Parameter, PVOID *Context) {
	OnceBase& onceBase = *(OnceBase*)Parameter;
	onceBase.Call();
	return TRUE;
}

static void CallOnceFun(once_flag& onceFlag, OnceBase& onceBase) {
	EXT_LOCK (s_mtxCallOnce) {
		if (!onceFlag.m_flag) {
			onceBase.Call();
			onceFlag.m_flag = 1;
		}
	}
}

static void WinCallOnceFun(once_flag& onceFlag, OnceBase& onceBase) {
	Win32Check(s_pfnInitOnceExecuteOnce(&onceFlag.m_initOnce, &InitOnceFun, &onceBase, 0));
}

static void (*s_pfnCallOnceFun)(once_flag& onceFlag, OnceBase& onceBase);

#endif  // UCFG_WIN32_FULL

once_flag::once_flag() {
	ZeroStruct(*this);		// INIT_ONCE_STATIC_INIT iz zero
}

void AFXAPI CallOnceImp(once_flag& onceFlag, OnceBase& onceBase) {
#if UCFG_WIN32_FULL
	if (!s_pfnCallOnceFun) {
		HMODULE hModule = GetModuleHandleW(L"kernel32.dll");
		s_pfnCallOnceFun = (!hModule || !Ext::GetProcAddress(s_pfnInitOnceExecuteOnce, hModule, "InitOnceExecuteOnce")) ? &CallOnceFun : &WinCallOnceFun;
	}
	s_pfnCallOnceFun(onceFlag, onceBase);
#else
	EXT_LOCK (s_mtxCallOnce) {
		if (!onceFlag.m_flag) {
			onceBase.Call();
			flag = 1;
		}
	}
#endif
}

#endif // EXT_ONCE_BASE_DEFINED

}  // ExtSTL::




