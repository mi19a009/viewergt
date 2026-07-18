#!/usr/bin/make
APP      = $(USR)/share/applications/$(APPID).desktop
APPID    = com.github.mi19a009.PictureViewer
BIN      = bin/viewer
CFLAGS   = -Iinclude -Wall $(shell pkg-config --cflags gtk4)
CR       = glib-compile-resources
CS       = glib-compile-schemas
DDEBUG   = -O0 -g -rdynamic
DNATIVE  = -march=native -mtune=native
DNDEBUG  = -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS
EXEC     = $(abspath $(BIN))
GTK      = $(wildcard gtk/*.ui)
ICON     = $(abspath icons/scalable/actions/viewer.svg)
ICONS    = $(wildcard icons/scalable/actions/*.svg)
LIBS     = $(shell pkg-config --libs gtk4)
LINGUAS  = ja
LOCALE   = $(LINGUAS:%=bin/%/LC_MESSAGES/viewer.mo)
MODULE   = about alert app document iface locale main
OBJ      = $(addprefix obj/, $(addsuffix .o, $(MODULE)))
RESOURCE = obj/resource.o
SCHEMA   = $(USR)/share/glib-2.0/schemas/$(APPID).gschema.xml
SRC      = src/resource.c
USR      = $(HOME)/.local
VALID    = include/viewer.gschema.valid
.PHONY: build clean debug install native release uninstall
release:
	$(MAKE) build "CFLAGS = $(CFLAGS) $(DNDEBUG)"
native:
	$(MAKE) build "CFLAGS = $(CFLAGS) $(DNDEBUG) $(DNATIVE)"
debug:
	$(MAKE) build "CFLAGS = $(CFLAGS) $(DDEBUG)"
clean:
	$(RM) $(BIN) $(LOCALE) $(OBJ) $(SRC) $(VALID)
uninstall:
	$(RM) $(APP) $(SCHEMA)
	$(CS) $(dir $(SCHEMA))
build: $(BIN) $(LOCALE)
install: $(APP) $(SCHEMA)
$(BIN): $(OBJ) $(RESOURCE)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(RESOURCE) $(LIBS)
$(OBJ): obj/%.o: src/%.c include/viewer.h
	$(CC) $(CFLAGS) -c -o $@ $<
$(RESOURCE): $(SRC)
	$(CC) $(CFLAGS) -c -o $@ $<
$(SRC): include/viewer.gresource.xml $(GTK) $(ICONS)
	$(CR) --generate-source --target $@ $<
$(LOCALE): bin/%/LC_MESSAGES/viewer.mo: text/%.po
	mkdir -p $(dir $@)
	msgfmt -o $@ $<
$(APP): include/viewer.desktop
	mkdir -p $(dir $@)
	sed -e "s#Exec=#Exec=$(EXEC)#" -e "s#Icon=#Icon=$(ICON)#" $< > $@
$(SCHEMA): include/viewer.gschema.xml $(VALID)
	mkdir -p $(dir $@)
	cp $< $@
	$(CS) $(dir $@)
$(VALID): include/viewer.gschema.xml
	$(CS) --strict --dry-run --schema-file=$< && touch $@
