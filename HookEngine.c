#include "HookEngine.h"
#include <stdlib.h>

// ---------- Debugging -------------
#include "dbg/dbg.h"
HookEngine *engine = NULL;

static char *get_module_path(char *module)
{
    // Get current module path
    char path[MAX_PATH] = { 0 };
    if (GetModuleFileNameA(GetModuleHandleA(module), path, sizeof(path)) == 0) {
        return NULL;
    }

    char * lastSlash = strrchr(path, '\\');
    char * dllName = (lastSlash != NULL) ? &lastSlash[0] : path;
    dllName[0] = '\0';

    if (!strlen(path)) {
        return NULL;
    }

    return strdup(path);
}

/*
 * Description      : Allocate a new HookEngine structure.
 * char *engineName : The path of the HookEngine
 * Return           : A pointer to an allocated HookEngine.
 */
HookEngine *
HookEngine_new (
    char *dllNameInjected,
    char *engineName
) {
    HookEngine *this;

    if ((this = calloc (1, sizeof(HookEngine))) == NULL)
        return NULL;

    if (!HookEngine_init (this, dllNameInjected, engineName)) {
        HookEngine_free (this);
        return NULL;
    }

    engine = this;
    dbg ("HookEngine initialized correctly.");

    return this;
}


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
) {
    char fullEnginePath[MAX_PATH] = {0};
    char *modulePath = get_module_path(dllNameInjected);
    if (!modulePath) {
        error ("Cannot find modulePath '%s'", dllNameInjected);
        return false;
    }
    sprintf(fullEnginePath, "%s/%s", modulePath, engineName);

    HMODULE hEngine = LoadLibrary (fullEnginePath);
    if (!hEngine) {
        error ("Cannot load HookEngine <%s>", fullEnginePath);
        return false;
    }

    info ("%s loaded into process.", fullEnginePath);

    if (!(this->hook = (typeof(this->hook)) GetProcAddress (hEngine, "HookFunction"))) {
        error ("Cannot find 'HookFunction'.");
        return false;
    }

    if (!(this->unhook = (typeof(this->unhook)) GetProcAddress (hEngine, "UnhookFunction"))) {
        error ("Cannot find 'UnhookFunction'.");
        return false;
    }

    if (!(this->get_original_function = (typeof(this->get_original_function)) GetProcAddress (hEngine, "GetOriginalFunction"))) {
        error ("Cannot find GetOriginalFunction.");
        return false;
    }

    bb_queue_init (&this->hookedFunctions);

    return true;
}

bool
HookEngine_hook (
    PVOID *ppSystemFunction,
    PVOID pHookFunction
) {
    if (!engine) {
        MessageBoxA(NULL, "Initialize HookEngine first.", "Error.", 0);
        return false;
    }

    bb_queue_add (&engine->hookedFunctions, ppSystemFunction);

    if (!engine->hook (ppSystemFunction, pHookFunction)) {
        error ("Error when hooking function %p", ppSystemFunction);
        return false;
    }

    return true;
}

bool
HookEngine_unhook (
    PVOID *ppHookedFunction
) {
    if (!engine) {
        MessageBoxA(NULL, "Initialize HookEngine first.", "Error.", 0);
        return false;
    }

    return engine->unhook (ppHookedFunction);
}

PVOID *
HookEngine_get_original_function (
    PVOID pHookFunction
) {
    PVOID **pFunc;

    if (!engine) {
        MessageBoxA(NULL, "Initialize HookEngine first.", "Error.", 0);
        return NULL;
    }

    if (!(pFunc = engine->get_original_function (pHookFunction))) {
        error ("Cannot get original function of %p", pHookFunction);
        return 0;
    }

    return *pFunc;
}

bool
HookEngine_unhook_all (
    void
) {
    if (!engine) {
        MessageBoxA(NULL, "Initialize HookEngine first.", "Error.", 0);
        return false;
    }

    while (bb_queue_get_length (&engine->hookedFunctions)) {
        void* originalFunction = bb_queue_pop (&engine->hookedFunctions);
        if (!(HookEngine_unhook (originalFunction))) {
            error ("Error when hooking function %p", originalFunction);
            return false;
        }
    }

    return true;
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
