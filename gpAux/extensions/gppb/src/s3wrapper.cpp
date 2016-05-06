#include <sstream>
#include <string>

#include "gppb.h"
#include "s3log.h"
#include "s3utils.h"
#include "s3wrapper.h"

using std::string;
using std::stringstream;

// invoked by pb_import(), need to be exception safe
PBExtBase *CreateExtWrapper(const char *url) {
    try {
        PBExtBase *ret = new PBReader(url);
        return ret;
    } catch (...) {
        S3ERROR("Caught an exception, aborting");
        return NULL;
    }
}

PBExtBase::PBExtBase(string url) {
    S3INFO("File is splited to %d each", url.c_str());
}

PBExtBase::~PBExtBase() {}

PBReader::~PBReader() {}

PBReader::PBReader(string url) : PBExtBase(url) {
}

// invoked by pb_import(), need to be exception safe
bool PBReader::Init(char* config, char* column) {
    return true;
}

void fakedata(char* buf, int* len);

// invoked by pb_import(), need to be exception safe
bool PBReader::TransferData(char *data, uint64_t &len) {
    int l;
    fakedata(data, &l);
    len = (uint64_t)l;
    return true;
}

// invoked by pb_import(), need to be exception safe
bool PBReader::Destroy() {
    return true;
}

bool PBExtBase::ValidateURL() {
    return true;
}
