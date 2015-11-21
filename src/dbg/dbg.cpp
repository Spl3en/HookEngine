/**
*
*   ██████╗   ██╗ ███████╗ ███╗   ███╗ ██╗   ██╗
*   ██╔══██╗ ███║ ██╔════╝ ████╗ ████║ ██║   ██║
*   ██████╔╝ ╚██║ █████╗   ██╔████╔██║ ██║   ██║
*   ██╔══██╗  ██║ ██╔══╝   ██║╚██╔╝██║ ██║   ██║
*   ██║  ██║  ██║ ███████╗ ██║ ╚═╝ ██║ ╚██████╔╝
*   ╚═╝  ╚═╝  ╚═╝ ╚══════╝ ╚═╝     ╚═╝  ╚═════╝
*
* @license GNU GENERAL PUBLIC LICENSE - Version 2, June 1991
*          See LICENSE file for further information
*/
// ---------- Includes ------------
#include "dbg.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#ifdef WIN32
#include <windows.h>
#endif // WIN32

// ------ Structure declaration -------
FILE *_output = NULL;

// ------ Static declaration -------

// ------ Extern function implementation ------

HANDLE dbgMutex;
bool initialized = -1;

void get_now_buffer(char *buffer) {
	SYSTEMTIME time;
	GetSystemTime(&time);
	sprintf(buffer, "%02d:%02d:%02d", time.wHour, time.wMinute, time.wSecond);
}

char *str_dup_printf(const char *format, ...)
{
	char *str = NULL;

	if (format)
	{
		char t[1];

		va_list pa;
		size_t size = 0;

		va_start(pa, format);
		size = vsnprintf(t, 1, format, pa);
		size++;
		str = (char *) malloc(size + 1);
		memset(str, '\0', size + 1);

		if (str)
			vsnprintf(str, size, format, pa);
	}

	return str;
}


void
dbg_set_output (
    FILE *output
) {
    _output = output;
}


void
dbg_close (
    void
	) {
	if (!initialized) {
		dbgMutex = CreateMutex(NULL, FALSE, NULL);
		initialized = true;
	}
	WaitForSingleObject(dbgMutex, INFINITE);

    if (_output && _output != stdout && _output != stderr) {
        fclose (_output);
    }

	_output = stdout;
	ReleaseMutex(dbgMutex);
}

void _dbg (
    int level,
    char *format,
    ...
) {
	if (!initialized) {
		dbgMutex = CreateMutex(NULL, FALSE, NULL);
		initialized = true;
	}
	WaitForSingleObject(dbgMutex, INFINITE);

    va_list args;

    if (_output == NULL) {
        _output = stdout;
    }

    switch (level) {
        #ifdef WIN32
        case DBG_LEVEL_INFO:    SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x0A); break;
        case DBG_LEVEL_DEBUG: break;
        case DBG_LEVEL_WARNING: SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x0E); break;
        case DBG_LEVEL_ERROR:   SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x0C); break;
        case DBG_LEVEL_SPECIAL: SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x0B); break;
        #else
        case DBG_LEVEL_INFO: fprintf (_output, "\x1b[32m"); break;
        case DBG_LEVEL_DEBUG: fprintf (_output, "\x1b[37m"); break;
        case DBG_LEVEL_WARNING: fprintf (_output, "\x1b[33m"); break;
        case DBG_LEVEL_ERROR: fprintf (_output, "\x1b[31m"); break;
        case DBG_LEVEL_SPECIAL: fprintf (_output, "\x1b[35m"); break;
        #endif
    }

    va_start (args, format);
        vfprintf (_output, format, args);
    va_end (args);

    #ifdef WIN32
    SetConsoleTextAttribute (GetStdHandle (STD_OUTPUT_HANDLE), 0x07);
    #else
    fprintf (_output, "\033[0m");
    #endif

	fflush(_output);
	ReleaseMutex(dbgMutex);
}

/**
 * @brief Dump a buffer in the standard output
 * @param buffer An allocated buffer to dump
 * @param bufferSize The buffer size
 * @param prefix A string printed before each line of the dump
 * @return
 */
void
_buffer_print (
    void *buffer,
    int bufferSize,
    char *prefix
) {
	if (!initialized) {
		dbgMutex = CreateMutex(NULL, FALSE, NULL);
		initialized = true;
	}

	WaitForSingleObject(dbgMutex, INFINITE);

    int curPos = 0;

    fprintf (_output, "%s ===== buffer size = %d (0x%x) ================\n", prefix, bufferSize, bufferSize);
    while (curPos < bufferSize) {
        int offset;
        fprintf (_output, "%s", prefix);
        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            fprintf (_output, " %02X", ((unsigned char *) buffer)[curPos]);
        }
        if (offset != 16) {
            for (int j = 0; j < 16 - offset; j++) {
                fprintf (_output, "   ");
            }
        }

        fprintf (_output, " | ");
        curPos -= offset;

        for (offset = 0; offset < 16 && curPos < bufferSize; offset++, curPos++) {
            unsigned char c = ((unsigned char *) buffer)[curPos];
            fprintf (_output, "%c", isprint(c) ? c : '.');
        }

        fprintf (_output, "\n");
    }
    fprintf (_output, "%s=================================================\n", prefix);
	
	ReleaseMutex(dbgMutex);
}
