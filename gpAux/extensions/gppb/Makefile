# Options

# Flags
SHLIB_LINK = -lstdc++ -lpb
PG_CPPFLAGS = -O2 -g -std=c++11 -fPIC -I$(libpq_srcdir) -Isrc  -I$(libpq_srcdir)/postgresql/server/utils

# Targets
MODULE_big = gppb
OBJS = src/gppb.o src/s3utils.o src/s3log.o src/s3wrapper.o

# Launch
PGXS := $(shell pg_config --pgxs)
include $(PGXS)
