#include <stddef.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sched.h>
#include <sys/resource.h>

typedef enum {Dir,File} TYPE;

struct trash {
	char name[64];
	TYPE type;
} trash ;

struct trash trashes[] = {
	{".7934039a",Dir},
	{".a.dat",File},
	{".com.taobao.dp",Dir},
	{".DataStorage",Dir},
	{".gs_file",Dir},
	{".gs_fs0",Dir},
	{".gs_fs3",Dir},
	{".gs_fs6",Dir},
	{".iFly",Dir},
	{".tbs",Dir},
	{".turing.dat",File},
	{".turingdebug",Dir},
	{".uxx",Dir},
	{".vivo",Dir},
	{".zzz",File},
	{"Catfish",Dir},
	{"data/.push_udid",File},
	{"tbs",Dir},
	{"qqstory",Dir},
	{"msc",Dir},
	{"geetest",Dir},
	{".um",Dir},
	{".UTSystemConfig",Dir},
	{"widgetone",Dir},
	{"IntsigLog",Dir},
	//Seriously?
	{"backups",Dir},
	};

int rec_rm_dir(const char *path) {
	DIR *d = opendir(path);
	size_t path_len = strlen(path);
	int r = 1;
	if (d) {
		struct dirent *p;
		r = 0;
		while (!r && (p=readdir(d))) {
			int r2 = 1;
			char *buf;
			size_t len;
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;
			len = path_len + strlen(p->d_name) + 2;
			buf = malloc(len);
			if (buf) {
				struct stat statbuf;
				snprintf(buf, len, "%s/%s", path, p->d_name);
				if (!stat(buf, &statbuf)) {
					if (S_ISDIR(statbuf.st_mode))
						r2 = rec_rm_dir(buf);
					else
						r2 = remove(buf);
				}
				free(buf);
			}
			r = r2;
		}
		closedir(d);
	}
	if (!r)
		r = remove(path);
	return r;
}

bool isFileExists(const char *path)
{
    FILE *fptr = fopen(path, "r");

    if (fptr == NULL)
        return false;

    fclose(fptr);

    return true;
}

bool isDirectoryExists(const char *path)
{
    struct stat stats;

    stat(path, &stats);

    if (S_ISDIR(stats.st_mode))
        return true;

    return false;
}

int delit(const char *path ,TYPE type) {
	char fullpath[256] = {0};
	strcpy(fullpath,"/sdcard/");
	strcat(fullpath,path);
	if(type == File) {
		if(isFileExists(fullpath))
			return remove(fullpath);
	}else if (type == Dir) {
		if(isDirectoryExists(fullpath))
			return rec_rm_dir(fullpath);
	}
	return 1;
}


int main() {
	sched_setscheduler(0,SCHED_BATCH,NULL);
	setpriority(PRIO_PROCESS,0,15);
	struct trash* tptr = trashes;
	struct trash* endptr = tptr + sizeof(trashes)/sizeof(trash);
	while(1) {
		while(tptr < endptr) {
			delit(tptr->name,tptr->type);
			tptr++;
		}
		tptr = trashes;
		sleep(5);
	}
}
