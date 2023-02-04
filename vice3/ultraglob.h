#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Super globbing (wildcard handling) code.
// Author: Ondra Hosek.
#ifdef _WIN32
# include <windows.h>
#else
# include <glob.h>
#endif

#ifdef __cplusplus
extern "C"
{
#endif

char **epicGlob(char *glob, /* out */ size_t *count);

void freeEpicGlob(char **globs, size_t count);

#ifdef __cplusplus
}
#endif
