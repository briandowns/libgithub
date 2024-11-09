cc = cc

CFLAGS  = -g -std=c99 -O3 -Wall 
LDFLAGS = -lcurl 

NAME    = libgithub

UNAME_S = $(shell uname -s)

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

.PHONY: install
install: 
	cp gh_client.h $(INCDIR)
ifeq ($(UNAME_S),Linux)
	cp gh_client.h $(INCDIR)
	cp $(NAME).so $(LIBDIR)
endif
ifeq ($(UNAME_S),Darwin)
	cp gh_client.h $(INCDIR)
	cp $(NAME).dylib $(LIBDIR)
endif

uninstall:
	rm -f $(INCDIR)/gh_client.h
ifeq ($(UNAME_S),Linux)
	rm -f $(INCDIR)/$(NAME).so
endif
ifeq ($(UNAME_S),Darwin)
	rm -f $(INCDIR)/$(NAME).dylib
endif

.PHONY: test
test: clean
	$(CC) -o tests/tests gh_client.c tests/tests.c tests/unity/unity.c $(LDFLAGS)
	tests/tests
	rm -f tests/tests

.PHONY: clean
clean:
	rm -f $(NAME).dylib
	rm -f $(NAME).so
	rm -f example
	rm -f tests/tests

.PHONY: example
example: clean
	$(CC) -o $@ gh_client.c example.c $(CFLAGS) $(LDFLAGS)
