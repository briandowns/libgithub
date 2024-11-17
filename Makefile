cc = cc

NAME = libgithub

UNAME_S = $(shell uname -s)

CFLAGS  = -std=c99 -O3 -Wall -fPIC
ifeq ($(UNAME_S),FreeBSD)
	CFLAGS += $(shell pkg-config --cflags --libs libcurl)
endif
LDFLAGS = -lcurl

# respect traditional UNIX paths
INCDIR  = /usr/local/include
LIBDIR  = /usr/local/lib

ifeq ($(UNAME_S),Darwin)
$(NAME).dylib: clean
	$(CC) -c -dynamiclib -o $@ $(CFLAGS) $(LDFLAGS)
endif
ifeq ($(UNAME_S),Linux)
$(NAME).so: clean
	$(CC) -shared -o $@ $(CFLAGS) $(LDFLAGS)
endif

.PHONY: test
test: clean
	$(CC) -o tests/tests tests/github_test.c github.c $(CFLAGS) $(LDFLAGS)
	tests/tests
	rm -f tests/tests

.PHONY: valgrind
valgrind: example
	valgrind ./example

.PHONY: install
install: 
	cp github.h $(INCDIR)
ifeq ($(UNAME_S),Linux)
	cp github.h $(INCDIR)
	cp $(NAME).so $(LIBDIR)
endif
ifeq ($(UNAME_S),Darwin)
	cp github.h $(INCDIR)
	cp $(NAME).dylib $(LIBDIR)
endif

uninstall:
	rm -f $(INCDIR)/github.h
ifeq ($(UNAME_S),Linux)
	rm -f $(INCDIR)/$(NAME).so
endif
ifeq ($(UNAME_S),Darwin)
	rm -f $(INCDIR)/$(NAME).dylib
endif

.PHONY: clean
clean:
	rm -f $(NAME).dylib
	rm -f $(NAME).so
	rm -f example
	rm -f tests/tests

.PHONY: example
example: clean
	$(CC) -o $@ github.c example.c $(CFLAGS) $(LDFLAGS)
