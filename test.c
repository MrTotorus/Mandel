#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <ftw.h>

char newest[PATH_MAX];
time_t mtime = 0;

int checkifnewer(const char *path, const struct stat *sb, int typeflag)
{
    if (typeflag == FTW_F && sb->st_mtime > mtime) {
        mtime = sb->st_mtime;
        strncpy(newest, path, PATH_MAX);
    }
    return 0;
}

int main(void)
{
    ftw("data", checkifnewer, 1); 
    printf("%s\n", newest);
}