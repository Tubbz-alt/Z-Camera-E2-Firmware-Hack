CC := gcc
CFL := -std=gnu99 -ggdb -fPIC
TOOLCHAIN := aarch64-linux-gnu-
OUT_INJECTOR_NAME := injector
OUT_INJECTILE_NAME := injectile.so
ARCH := -march=armv8-a
LIBS := -L. -lQtCore -lQtGui -lQtNetwork

all:
	$(TOOLCHAIN)$(CC) $(CFL) $(ARCH) hooker.c events.c log.c rec_notificator.c $(LIBS) -shared -o $(OUT_INJECTILE_NAME)

clear:
	rm -f *.o
	rm -f $(OUT_INJECTOR_NAME)
	rm -f $(OUT_INJECTILE_NAME)
