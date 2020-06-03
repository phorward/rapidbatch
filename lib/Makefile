LIBPHORWARD = libphorward.a
PREGEX = pregex
PLEX = plex

CFLAGS += -g -DDEBUG -DUNICODE -DUTF8 -I. -Wall
LDLIBS += libphorward.a

SRC = \
	$(PREFIX)phorward.c \
	$(PREFIX)plex.c \
	$(PREFIX)pregex.c \
	$(PREFIX)util.c \


OBJ = $(patsubst %.c,%.o,$(SRC))

all: $(LIBPHORWARD) $(PREGEX) $(PLEX)

$(LIBPHORWARD): phorward.h $(OBJ)
	$(AR) rv $@ $(OBJ)

clean:
	-rm -f $(OBJ)
	-rm -f $(LIBPHORWARD) $(PREGEX) $(PLEX)

