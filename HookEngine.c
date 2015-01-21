#include "HookEngine.h"
#include <stdlib.h>

// ---------- Debugging -------------
#define __DEBUG_OBJECT__ "HookEngine"
#include "dbg/dbg.h"

HookEngine *engine = NULL;

/*
 * Description      : Allocate a new HookEngine structure.
 * char *enginePath : The path of the HookEngine
 * Return           : A pointer to an allocated HookEngine.
 */
HookEngine *
HookEngine_new (
    char *enginePath
) {
    HookEngine *this;

    if ((this = calloc (1, sizeof(HookEngine))) == NULL)
        return NULL;

    if (!HookEngine_init (this, enginePath)) {
        HookEngine_free (this);
        return NULL;
    }

    engine = this;

    return this;
}


/*
 * Description      : Initialize an allocated HookEngine structure.
 * HookEngine *this : An allocated HookEngine to initialize.
 * char *enginePath : The path of the HookEngine
 */
bool
HookEngine_init (
    HookEngine *this,
    char *enginePath
) {
    HMODULE hEngine = LoadLibrary (enginePath);
    if (!hEngine) {
        dbg ("Cannot load HookEngine <%s>", enginePath);
        return false;
    }

    this->hook = (typeof(this->hook)) GetProcAddress (hEngine, "HookFunction");
    this->unhook = (typeof(this->unhook)) GetProcAddress (hEngine, "UnhookFunction");
    this->get_original_function = (typeof(this->get_original_function)) GetProcAddress (hEngine, "GetOriginalFunction");

    bb_queue_init (&this->hookedFunctions);

    return true;
}

bool
HookEngine_hook (
    ULONG_PTR function,
    ULONG_PTR hookFunction
) {
    bb_queue_add (&engine->hookedFunctions, (void *) function);
    return engine->hook (function, hookFunction);
}

void
HookEngine_unhook (
    ULONG_PTR originalFunction
) {
    engine->unhook (originalFunction);
}

ULONG_PTR
HookEngine_get_original_function (
    ULONG_PTR hookFunction
) {
    return engine->get_original_function (hookFunction);
}

void
HookEngine_unhook_all (
    void
) {
    while (bb_queue_get_length (&engine->hookedFunctions)) {
        ULONG_PTR originalFunction = (int) bb_queue_pop (&engine->hookedFunctions);
        HookEngine_unhook (originalFunction);
    }
}

/*
 * Description      : Free an allocated HookEngine structure.
 * HookEngine *this : An allocated HookEngine to free.
 */
void
HookEngine_free (
    HookEngine *this
) {
    free (this);
}
