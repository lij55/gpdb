#ifndef __S3_EXT_WRAPPER__
#define __S3_EXT_WRAPPER__

#include <string>

using std::string;

class PBExtBase {
   public:
    PBExtBase(string url);
    virtual ~PBExtBase();
    virtual bool Init(char* config, char* column) = 0;
    virtual bool TransferData(char* data, uint64_t& len) = 0;
    virtual bool Destroy() = 0;
    virtual bool ValidateURL();
};

class PBReader : public PBExtBase {
   public:
    PBReader(string url);
    virtual ~PBReader();
    virtual bool Init(char* config, char* column);
    virtual bool TransferData(char* data, uint64_t& len);
    virtual bool Destroy();

   protected:
};

class PBWriter : public PBExtBase {};

extern "C" PBExtBase* CreateExtWrapper(const char* url);

#endif
