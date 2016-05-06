#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstdarg>
#include <cstdio>
#include <cstring>
#include <string>

#include "s3log.h"

unsigned int s3ext_loglevel = EXT_INFO;

extern "C" {
    void write_log(const char* fmt, ...) __attribute__((format(printf, 1, 2)));
}

#include "s3log.h"
#include "s3utils.h"

void _LogMessage(const char* fmt, va_list args) {
    char buf[1024];
    int len = vsnprintf(buf, sizeof(buf), fmt, args);
    if (len >= 1024) len = 1023;
    buf[len] = 0;
    write_log("%s", buf);
}

void LogMessage(LOGLEVEL loglevel, const char* fmt, ...) {
    if (loglevel > s3ext_loglevel) return;
    va_list args;
    va_start(args, fmt);
    _LogMessage(fmt, args);
    va_end(args);
}
