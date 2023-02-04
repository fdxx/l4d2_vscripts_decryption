#include "ultraglob.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Super globbing (wildcard handling) code.
// Author: Ondra Hosek.
#ifdef _WIN32
char **epicGlob(char *glob, /* out */ size_t *count)
{
	WIN32_FIND_DATA fdata;
	HANDLE findh = INVALID_HANDLE_VALUE;
	char *fullpath = (char*)malloc(MAX_PATH);
	char *dirpath = (char*)malloc(strlen(glob)+1);
	char **globs = (char**)malloc(sizeof(char*));
	size_t c = 0;
	char *lbs;

	// in case we have to bail out
	(*count) = c;

	strncpy(dirpath, glob, strlen(glob)+1);
	// NUL the last backslash
	lbs = strrchr(dirpath, '\\');
	if (lbs)
		lbs[0] = 0;
	else
	{
		realloc(dirpath, 2);
		strncpy(dirpath, ".", 2);
	}

	findh = FindFirstFile(glob, &fdata);
	if (findh == INVALID_HANDLE_VALUE)
	{
		fprintf(stderr, "ERROR while enumerating files of pattern '%s'.\n", glob);
		return 0;
	}
	_snprintf(fullpath, strlen(dirpath)+strlen(fdata.cFileName)+2, "%s\\%s", dirpath, fdata.cFileName);

	// we mustn't do this on directories. so, for each entry, check if it's
	// a directory.
	if ((GetFileAttributes(fullpath) & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) // not a directory
	{
		globs[c] = (char*)malloc(strlen(fullpath)+1);
		strncpy(globs[c], fullpath, strlen(fullpath)+1);
		++c;
		globs = (char**)realloc(globs, (c+1)*sizeof(char*));
		if (!globs)
		{
			fprintf(stderr, "ERROR while allocating memory.\n");
			return 0;
		}
	}

	while (FindNextFile(findh, &fdata) != 0)
	{
		_snprintf(fullpath, strlen(dirpath)+strlen(fdata.cFileName)+2, "%s\\%s", dirpath, fdata.cFileName);

		if ((GetFileAttributes(fullpath) & FILE_ATTRIBUTE_DIRECTORY) != FILE_ATTRIBUTE_DIRECTORY) // not a directory
		{
			globs[c] = (char*)malloc(strlen(fullpath)+1);
			strncpy(globs[c], fullpath, strlen(fullpath)+1);
			++c;
			globs = (char**)realloc(globs, (c+1)*sizeof(char*));
			if (!globs)
			{
				fprintf(stderr, "ERROR while allocating memory.\n");
				return 0;
			}
		}
	}

	free(dirpath);
	free(fullpath);

	(*count) = c;
	return globs;
}
#else // hope for POSIX
char **epicGlob(char *globPat, /* out */ size_t *count)
{
	char **globs = (char**)malloc(sizeof(char*));
	const char *fullpath;
	struct stat st;
	int i;
	size_t c = 0;
	glob_t glb;

	c = (*count) = 0;

	if (glob(globPat, 0, 0, &glb) != 0)
	{
		fprintf(stderr, "ERROR while enumerating files of pattern '%s'.\n", globPat);
		return 0;
	}

	for (i = 0; i < glb.gl_pathc; ++i)
	{
		fullpath = glb.gl_pathv[i];
		if (stat(fullpath, &st) == -1)
		{
			fprintf(stderr, "ERROR while getting attributes of file '%s'.\n", fullpath);
			return 0;
		}

		if (!S_ISDIR(st.st_mode)) // not a directory
		{
			globs[c] = (char*)malloc(strlen(fullpath)+1);
			strncpy(globs[c], fullpath, strlen(fullpath)+1);
			globs = (char**)realloc(globs, (++c)*sizeof(char*));
			if (!globs)
			{
				fprintf(stderr, "ERROR while allocating memory.\n");
				return 0;
			}
		}
	}

	globfree(&glb);
	(*count) = c;

	return globs;
}
#endif

void freeEpicGlob(char **globs, size_t count)
{
	size_t i = 0;
	for (i = 0; i < count; ++i)
	{
		free(globs[i]);
	}
	free(globs);
}

#ifdef ULTRAGLOB_TEST
int main(int argc, char **argv)
{
	size_t i = 0, len;
	char **globs;

	if (argc != 2)
		return 1;
	globs = epicGlob(argv[1], &len);
	while (i < len)
	{
		printf("%s\n", globs[i++]);
	}
	freeEpicGlob(globs, len);
}
#endif


#ifdef __cplusplus
}
#endif
