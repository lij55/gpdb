#ifndef _UTILFUNCTIONS_
#define _UTILFUNCTIONS_

#include <cstdio>
#include <cstdlib>
// #include <cstdint>
#include <stdint.h>
#include <sys/types.h>
#include <cstring>
#include <string>

#include "s3log.h"

using std::string;
bool gethttpnow(char datebuf[65]);

bool lowercase(char* out, const char* in);
void _tolower(char* buf);

bool trim(char* out, const char* in, const char* trimed = " \t\r\n");

size_t find_Nth(const string& str,  // where to work
                unsigned N,         // N'th ocurrence
                const string& find  // what to 'find'
                );
bool to_bool(std::string str);

void find_replace(string& str, const string& find, const string& replace);

char* wholefile(char* path);
char *get_opt(const char *url, const char *key);
char *truncate_options(const char *url_with_options);
#endif  // _UTILFUNCTIONS_
