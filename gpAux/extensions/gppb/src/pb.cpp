#include <cstring>


#define MAXNUM 10
#define data  "aaa,123,456,789\n"


void fakedata(char* buf, int* len) {
    static int i = 0;
    if(i++ < MAXNUM) {
        strcpy(buf, data);
        *len = strlen(data);
    } else {
        *len = 0;
        i = 0;
    }
    
}
