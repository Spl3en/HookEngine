// --- Author             : Moreau Cyril - Spl3en
#pragma once

// ---------- Includes ------------
#include "Utils/Utils.h"
#include "BbQueue/BbQueue.h"

// ---------- Defines -------------


// ------ Structure declaration -------
typedef struct _HookEngine
{
    BOOL (__cdecl *hook)(ULONG_PTR OriginalFunction, ULONG_PTR NewFunction);
    VOID (__cdecl *unhook)(ULONG_PTR Function);
    ULONG_PTR (__cdecl *get_original_function)(ULONG_PTR Hook);

    BbQueue hookedFunctions;

}   HookEngine;



// --------- Allocators ---------

/*
 * Description      : Allocate a new HookEngine structure.
 * char *enginePath : The path of the HookEngine
 * Return           : A pointer to an allocated HookEngine.
 */
HookEngine *
HookEngine_new (
    char *enginePath
);

// ----------- Functions ------------


/*
 * Description      : Initialize an allocated HookEngine structure.
 * HookEngine *this : An allocated HookEngine to initialize.
 * char *enginePath : The path of the HookEngine
 */
bool
HookEngine_init (
    HookEngine *this,
    char *enginePath
);

bool
HookEngine_hook (
    ULONG_PTR function,
    ULONG_PTR hookFunction
);

void
HookEngine_unhook (
    ULONG_PTR originalFunction
);

ULONG_PTR
HookEngine_get_original_function (
    ULONG_PTR hookFunction
);

void
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


