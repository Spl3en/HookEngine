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
#pragma once

// ---------- Includes ------------
#include <stdio.h>
#include <stdlib.h>

#define __DBG_ACTIVATED__ TRUE

typedef enum {
    DBG_LEVEL_INFO,
    DBG_LEVEL_DEBUG,
    DBG_LEVEL_WARNING,
    DBG_LEVEL_ERROR,
    DBG_LEVEL_SPECIAL,
} DbgLevel;

void get_now_buffer(char *buffer);
char *str_dup_printf(const char *format, ...);

// ---------- Defines -------------
/** Get the name of the module object from the filename. __FILE__ differs between GCC on MinGW and Linux. */
#ifdef WIN32
#define __FILENAME__ (((strrchr(__FILE__, '\\')) != NULL) ? &(strrchr(__FILE__, '\\'))[1] : __FILE__)
#else
#define __FILENAME__ (((strrchr(__FILE__,  '/')) != NULL) ? &(strrchr(__FILE__,  '/'))[1] : __FILE__)
#endif

/** Debug line template */
#define dbg_ex(level, output, format, ...)                            \
    do {                                                              \
        char __time_buffer__[100] = {0};                              \
        get_now_buffer(__time_buffer__);                              \
        _dbg (level, "[%s][%s:%d in %s] " format,                     \
            __time_buffer__,                                          \
            __FILENAME__,                                             \
            __LINE__,                                                 \
            __FUNCTION__,                                             \
            ##__VA_ARGS__);                                           \
    } while (0)

/** Buffer dump template */
#define buffer_print_ex(buffer, size, prefix)                         \
    do {                                                              \
        char __time_buffer__[100] = {0};                              \
        get_now_buffer(__time_buffer__);                              \
        char *__prefix__ = str_dup_printf (                           \
            "[%s][%s:%d in %s] %s",                                   \
            __time_buffer__,                                          \
            __FILENAME__,                                             \
            __LINE__,                                                 \
            __FUNCTION__,                                             \
            (prefix) ? prefix : "");                                  \
            _buffer_print (buffer, size, __prefix__);                 \
            free (__prefix__);                                        \
    } while (0)


#if defined(__DBG_ACTIVATED__) && __DBG_ACTIVATED__ == TRUE

    // Declare debug functions here
    /** Basic level debug function. */
    #define dbg(format, ...)                                              \
        dbg_ex (DBG_LEVEL_DEBUG, stdout, format "\n", ##__VA_ARGS__)

    /** Warning level debug function. */
    #define warning(format, ...)                                          \
        dbg_ex (DBG_LEVEL_WARNING, stderr, "[WARNING] " format "\n", ##__VA_ARGS__)

    /** Error level debug function. */
    #define error(format, ...)                                            \
        dbg_ex (DBG_LEVEL_ERROR, stderr, "[ERROR] " format "\n", ##__VA_ARGS__)

    /** Special level debug function. */
    #define special(format, ...)                                            \
        dbg_ex (DBG_LEVEL_SPECIAL, stderr, "[SPECIAL] " format "\n", ##__VA_ARGS__)

    /** Fatal error level debug function. */
    #define die(format, ...)                                              \
        do {                                                              \
            dbg_ex (DBG_LEVEL_ERROR, stderr, "[FATAL ERROR] " format "\n", ##__VA_ARGS__); \
            getc(stdin); \
            exit (-1);                                                    \
        } while (0)

    /** Dump a buffer into the standard output.
     * See _buffer_print for the arguments documentation. */
    #define buffer_print(buffer, size, prefix) \
        buffer_print_ex (buffer, size, prefix)

#else

    // Don't output anything if __DBG_ACTIVATED__ is not enabled
	#define dbg(format, ...)
	#define warning(format, ...)
	#define error(format, ...)
	#define die(format, ...)
	#define buffer_print(format, ...)
	#define special(format, ...)

#endif


/** Info level debug function. Not a debug information */
#define info(format, ...)                                          \
    dbg_ex (DBG_LEVEL_INFO, stdout, format "\n", ##__VA_ARGS__)


// ----------- Functions ------------

/**
 * @brief Output a formated message to a chosen stream
 * @param level The debug level
 * @param output A destination stream
 * @param format the format of the message
 * @return
 */
void _dbg (
    int level,
    char *format,
    ...
);

/**
 * @brief Dump a buffer in the standard output
 * @param buffer An allocated buffer to dump
 * @param bufferSize The buffer size
 * @param prefix A string printed before each line of the dump (optional)
 * @return
 */
void
_buffer_print (
    void *buffer,
    int bufferSize,
    char *prefix
);


/**
 * @brief Redirect the default output of the debug messages to a chosen FILE
 * @param output The destination stream of the debug messages
 */
void
dbg_set_output (
    FILE *output
);


/**
 * @brief Close the custom debug file
 */
void
dbg_close (
    void
);
