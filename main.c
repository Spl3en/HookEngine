#include <windows.h>
#include <stdio.h>
#include "dbg/dbg.h"
#include "HookEngine.h"

int WINAPI HookMessageBoxA (HWND hWnd, LPCTSTR lpText, LPCTSTR lpCaption, UINT uType) {
	typeof(HookMessageBoxA) *hooked = (typeof(HookMessageBoxA) *) HookEngine_get_original_function(HookMessageBoxA);
	return hooked (hWnd, "hooked", "hooked", uType);
}

int main (int argc, char **argv) {
	
	HookEngine *hookEngine = NULL;
	PVOID msgbox;

	if (!(hookEngine = HookEngine_new (argv[1], "HookEngine.dll"))) {
		error ("Cannot initialize hook engine correctly.");
		return 0;
	}

 	if (!(msgbox = GetProcAddress(LoadLibrary("user32.dll"), "MessageBoxA"))) {
		error ("Cannot find user32!MessageBoxA.");
		return 0;
	}

	HookEngine_hook(&msgbox, HookMessageBoxA);
	MessageBoxA(NULL, "test", "test", 0);

	HookEngine_unhook(&msgbox);
	MessageBoxA(NULL, "test", "test", 0);

	return 0;
}