#define PGDLLIMPORT "C"

#include <cstdlib>

#include <pthread.h>
#include <signal.h>

#include "postgres.h"

#include "funcapi.h"

#include "access/extprotocol.h"
#include "catalog/pg_proc.h"
#include "fmgr.h"
#include "utils/array.h"
#include "utils/builtins.h"
#include "utils/memutils.h"


#include "gppb.h"
#include "s3log.h"
#include "s3utils.h"
#include "s3wrapper.h"

/* Do the module magic dance */

PG_MODULE_MAGIC;
PG_FUNCTION_INFO_V1(pb_export);
PG_FUNCTION_INFO_V1(pb_import);
PG_FUNCTION_INFO_V1(pb_validate_urls);

extern "C" {
Datum pb_export(PG_FUNCTION_ARGS);
Datum pb_import(PG_FUNCTION_ARGS);
Datum pb_validate_urls(PG_FUNCTION_ARGS);
}
Datum pb_import(PG_FUNCTION_ARGS) {
    PBExtBase *myData;
    char *data;
    int data_len;
    size_t nread = 0;

    /* Must be called via the external table format manager */
    if (!CALLED_AS_EXTPROTOCOL(fcinfo))
        elog(ERROR,
             "extprotocol_import: not called by external protocol manager");

    /* Get our internal description of the protocol */
    myData = (PBExtBase *)EXTPROTOCOL_GET_USER_CTX(fcinfo);

    if (EXTPROTOCOL_IS_LAST_CALL(fcinfo)) {
        if (myData) {
            if (!myData->Destroy()) {
                ereport(ERROR, (0, errmsg("Failed to cleanup PB extention")));
            }
            delete myData;
        }

        PG_RETURN_INT32(0);
    }

    if (myData == NULL) {
        /* first call. do any desired init */

        const char *p_name = "pb";
        char *url_with_options = EXTPROTOCOL_GET_URL(fcinfo);
        char *url = truncate_options(url_with_options);

        char *config_path = get_opt(url_with_options, "config");
        if (!config_path) {
            elog(ERROR, "config is required in url");
        }
        char* column = get_opt(url_with_options, "column");
        if (!column) {
            elog(ERROR, "column is required in url");
        }

        myData = CreateExtWrapper(url);
        // need column, config, url
        if (!myData ||
            !myData->Init(config_path, column)) {
            if (myData) delete myData;
            ereport(ERROR, (0, errmsg("Failed to init protobuf extension")));
        }
        EXTPROTOCOL_SET_USER_CTX(fcinfo, myData);
        free(url);
    }

    /* =======================================================================
     *                            DO THE IMPORT
     * =======================================================================
     */

    data = EXTPROTOCOL_GET_DATABUF(fcinfo);
    data_len = EXTPROTOCOL_GET_DATALEN(fcinfo);
    uint64_t readlen = 0;
    if (data_len > 0) {
        readlen = data_len;
        if (!myData->TransferData(data, readlen))
            ereport(ERROR, (0, errmsg("pb_import: could not read data")));
        nread = (size_t)readlen;
    }

    PG_RETURN_INT32((int)nread);
}

/*
 * Export data out of GPDB.
 * invoked by GPDB, be careful with C++ exceptions.
 */
Datum pb_export(PG_FUNCTION_ARGS) { PG_RETURN_INT32(0); }

Datum pb_validate_urls(PG_FUNCTION_ARGS) {
    int nurls;
    int i;
    ValidatorDirection direction;
    PG_RETURN_VOID();
}
