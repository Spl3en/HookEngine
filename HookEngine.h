// --- Author             : Moreau Cyril - Spl3en
#pragma once

// ---------- Includes ------------
#include "Utils/Utils.h"
#include "BbQueue/BbQueue.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef struct _HookEngine
{
    BOOL (__cdecl *hook)(PVOID *ppSystemFunction, PVOID pHookFunction);
    BOOL (__cdecl *unhook)(PVOID *ppHookedFunction);
    PVOID (__cdecl *get_original_function)(PVOID *ppHookedFunction);
    BbQueue hookedFunctions;
}   HookEngine;

// --------- Allocators ---------

/*
 * Description      : Allocate a new HookEngine structure.
 * char *engineName : The name of the HookEngine DLL
 * Return           : A pointer to an allocated HookEngine.
 */
HookEngine *
HookEngine_new (
    char *dllNameInjected,
    char *engineName
);

// ----------- Functions ------------


/*
 * Description      : Initialize an allocated HookEngine structure.
 * HookEngine *this : An allocated HookEngine to initialize.
 * char *engineName : The name of the HookEngine DLL
 */
bool
HookEngine_init (
    HookEngine *this,
    char *dllNameInjected,
    char *engineName
);

bool
HookEngine_hook (
    PVOID *ppSystemFunction,
    PVOID pHookFunction
);

bool
HookEngine_unhook (
    PVOID *ppHookedFunction
);

PVOID *
HookEngine_get_original_function (
    PVOID pHookFunction
);

bool
HookEngine_unhook_all (
    void
);


// --------- Destructors ----------

/*
 * Description            : Free an allocated HookEngine structure.
 * HookEngine *this    : An allocated HookEngine to free.
 */
void
HookEngine_free (
    HookEngine *this
);


