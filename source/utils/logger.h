#ifndef UTILS_LOGGER_H_
#define UTILS_LOGGER_H_

#include "fsl_debug_console.h"

#define LOGGER_PROJECT_LOGO "  _      ______ _____  ____     _____ _____ _________     __\r\n \
| |    |  ____/ ____|/ __ \\   / ____|_   _|__   __\\ \\   / /\r\n \
| |    | |__ | |  __| |  | | | |      | |    | |   \\ \\_/ / \r\n \
| |    |  __|| | |_ | |  | | | |      | |    | |    \\   /  \r\n \
| |____| |___| |__| | |__| | | |____ _| |_   | |     | |   \r\n \
|______|______\\_____|\\____/   \\_____|_____|  |_|     |_|   \r\n"

#define LOGGER_WRITENL()	do {PRINTF("\r\n");} while(0)
#define LOGGER_WRITELN(x)	do {PRINTF x;PRINTF("\r\n");} while(0)
#define LOGGER_WRITE(x)		do {PRINTF x;} while(0)

#endif /* UTILS_LOGGER_H_ */
