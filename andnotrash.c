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

bool isNameNumOnly(const char* name);
void spec_rm1(const char *path);
int rec_rm_dir(const char *path);
bool isDirectoryEmpty(char *dirname);
bool isFileExists(const char *path);
bool isDirectoryExists(const char *path);
int delit(const char *path ,TYPE type);

struct trash {
	char name[64];
	TYPE type;
} trash;

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
	//shitty umeng
	{"Android/data/.um",Dir},
	{"Android/.system_android_l2",File},
	{"Android/obj/.um",Dir},
	{"Documents/environment.pub",File},
	{"Pictures/jpush",Dir},
	{"Pictures/.sss",File},
	{"Music/.qqq",File},
	{"Download/.tim",File},
	{"baidu/tempdata",Dir},
	{"app/.d10",File},
	//Seriously?
	{"backups",Dir},
	};
char spec[][256] = {"/sdcard/Documents/"};

bool isNameNumOnly(const char* name) {
	bool n = true;
	for(int tmp = 0;tmp < strlen(name);name++) {
		if(!(name[tmp] >= '0' && name[tmp] <= '9'))
			n = false;
	}
	return n;
}


void spec_rm1(const char *path) {
	DIR *d = opendir(path);
	if (d) {
		struct dirent *p;
		while((p=readdir(d))) {
			if (!strcmp(p->d_name, ".") || !strcmp(p->d_name, ".."))
				continue;
			if(isNameNumOnly(p->d_name)) {
				char thispath[256] = {0};
				char fullpath1[256] = {0};
				char fullpath2[256] = {0};
				strcpy(thispath,path);
				strcat(thispath,p->d_name);
				strcpy(fullpath1,path);
				strcpy(fullpath2,path);
				strcat(fullpath1,p->d_name);
				strcat(fullpath2,p->d_name);
				strcat(fullpath1,"/environment.priv");
				strcat(fullpath2,"/environment.pub");
				if(isFileExists(fullpath1))
					remove(fullpath1);
				if(isFileExists(fullpath2))
					remove(fullpath2);
				if(isDirectoryEmpty(thispath))
					remove(thispath);
			}
		}
	}
	closedir(d);
	return;
}

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

bool isDirectoryEmpty(char *dirname) {
  int n = 0;
  struct dirent *d;
  DIR *dir = opendir(dirname);
  if (dir == NULL)
    return true;
  while ((d = readdir(dir)) != NULL) {
    if(++n > 2)
      break;
  }
  closedir(dir);
  if (n <= 2)
    return true;
  else
    return false;
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
	setpriority(PRIO_PROCESS,0,10);
	struct trash* tptr = trashes;
	struct trash* endptr = tptr + sizeof(trashes)/sizeof(trash);
	while(1) {
		while(tptr < endptr) {
			delit(tptr->name,tptr->type);
			tptr++;
		}
		spec_rm1(spec[0]);
		tptr = trashes;
		sleep(12);
	}
}
