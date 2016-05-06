#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <iomanip>
#include <sstream>
#include <string>

#include <arpa/inet.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#include "s3utils.h"

using std::string;
extern "C" {
void write_log(const char *fmt, ...) __attribute__((format(printf, 1, 2)));
}

bool gethttpnow(char datebuf[65]) {  //('D, d M Y H:i:s T')
    struct tm tm_info;
    time_t t;
    if (!datebuf) {
        return false;
    }
    time(&t);
    localtime_r(&t, &tm_info);
    strftime(datebuf, 65, "%a, %d %b %Y %H:%M:%S %z", &tm_info);
    return true;
}

void _tolower(char *buf) {
    do {
        if (*buf >= 'A' && *buf <= 'Z') *buf |= 0x60;
    } while (*buf++);
    return;
}

bool trim(char *out, const char *in, const char *trimed) {
    int targetlen;

    if (!out || !in) {  // invalid string params
        return false;
    }

    targetlen = strlen(in);

    while (targetlen > 0) {
        if (strchr(trimed, in[targetlen - 1]) ==
            NULL)  // can't find stripped char
            break;
        else
            targetlen--;
    }

    while (targetlen > 0) {
        if (strchr(trimed, *in) == NULL)  // normal string
            break;
        else {
            in++;
            targetlen--;
        }
    }

    memcpy(out, in, targetlen);
    out[targetlen] = 0;
    return true;
}

/*
 * It would be more efficient to use a variation of KMP to
 * benefit from the failure function.
 * - Algorithm inspired by James Kanze.
 * - http://stackoverflow.com/questions/20406744/
 */
size_t find_Nth(const string &str,  // where to work
                unsigned N,         // N'th ocurrence
                const string &find  // what to 'find'
                ) {
    if (0 == N) {
        return string::npos;
    }
    size_t pos, from = 0;
    unsigned i = 0;
    while (i < N) {
        pos = str.find(find, from);
        if (string::npos == pos) {
            break;
        }
        from = pos + 1;  // from = pos + find.size();
        ++i;
    }
    return pos;
}
void find_replace(string &str, const string &find, const string &replace) {
    if (find.empty()) return;

    size_t pos = 0;

    while ((pos = str.find(find, pos)) != string::npos) {
        str.replace(pos, find.length(), replace);
        pos += replace.length();
    }
}

char* wholefile(char* path) {
    FILE *f = fopen(path, "rb");
    if(!f) 
        return NULL;
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  //same as rewind(f);

    char *s = (char*)malloc(fsize + 1);
    fread(s, fsize, 1, f);
    fclose(f);

    s[fsize] = 0;
    return s;
}


// invoked by s3_import(), need to be exception safe
char *get_opt(const char *url, const char *key) {
    const char *key_start = NULL;
    const char *key_tailing = NULL;
    char *value = NULL;
    int value_len = 0;

    if (!url || !key) {
        return NULL;
    }

    // construct the key to search " key="
    char *key2search = (char *)malloc(strlen(key) + 3);
    if (!key2search) {
        S3ERROR("Can't allocate memory for string");
        return NULL;
    }

    int key_len = strlen(key);

    key2search[0] = ' ';
    memcpy(key2search + 1, key, key_len);
    key2search[key_len + 1] = '=';
    key2search[key_len + 2] = 0;

    // get the whole options string (strip "url ")
    const char *delimiter = " ";
    const char *options = strstr(url, delimiter);
    if (!options) {
        goto FAIL;
    }

    // get the string " key=blah1 key2=blah2 ..."
    key_start = strstr(options, key2search);
    if (key_start == NULL) {
        goto FAIL;
    }

    // get the string "blah1 key2=blah2 ..."
    key_start += strlen(key2search);
    if (*key_start == ' ') {
        goto FAIL;
    }

    // get the string "key2=blah2 ..."
    key_tailing = strstr(key_start, delimiter);
    // get the length of "blah1"
    if (key_tailing) {
        value_len = strlen(key_start) - strlen(key_tailing);
    } else {
        value_len = strlen(key_start);
    }

    // get the string "blah1"
    value = (char *)malloc(value_len + 1);
    if (!value) {
        goto FAIL;
    }

    memcpy(value, key_start, value_len);
    value[value_len] = 0;

    free(key2search);

    return value;
FAIL:
    if (key2search) {
        free(key2search);
    }
    return NULL;
}

// returned memory needs to be freed
// invoked by s3_import(), need to be exception safe
char *truncate_options(const char *url_with_options) {
    const char *delimiter = " ";
    char *options = strstr((char *)url_with_options, delimiter);
    int url_len = strlen(url_with_options);

    if (options) {
        url_len = strlen(url_with_options) - strlen(options);
    }

    char *url = (char *)malloc(url_len + 1);
    if (url) {
        memcpy(url, url_with_options, url_len);
        url[url_len] = 0;
    }

    return url;
}
