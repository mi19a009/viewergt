# !/usr/bin/make
APP      = $(HOME)/.local/share/applications/$(APPID).desktop
APPID    = com.github.mi19a009.PictureViewer
C        = about alert app document file main resource
CFLAGS   = -Iinclude -Wall $(shell pkg-config gtk+-3.0 --cflags)
DEBUG    = -O0 -g -rdynamic
DNDEBUG  = -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS
EXEC     = $(abspath $(OUT))
GTK      = $(wildcard gtk/*.ui)
ICON     = $(abspath icons/scalable/actions/viewer.svg)
ICONS    = $(wildcard icons/scalable/actions/*.svg)
LIBS     = $(shell pkg-config gtk+-3.0 --libs)
LINGUAS  = ja
NATIVE   = -march=native -mtune=native
OBJ      = $(C:%=obj/%.o)
OUT      = bin/viewer
LOCALE   = $(LINGUAS:%=bin/%/LC_MESSAGES/viewer.mo)
SCHEMA   = $(HOME)/.local/share/glib-2.0/schemas/$(APPID).gschema.xml
SRC      = $(C:%=src/%.c)
SRCRES   = src/resource.c
VALID    = include/viewer.gschema.valid
.PHONY: all build clean debug install native uninstall
all:
	$(MAKE) build "CFLAGS = $(CFLAGS) $(DNDEBUG)"
build: $(OUT) $(LOCALE)
clean:
	$(RM) $(OUT) $(OBJ) $(LOCALE) $(SRCRES) $(VALID)
debug:
	$(MAKE) build "CFLAGS = $(CFLAGS) $(DEBUG)"
install: $(APP) $(SCHEMA)
native:
	$(MAKE) build "CFLAGS = $(CFLAGS) $(DNDEBUG) $(NATIVE)"
uninstall:
	$(RM) $(APP) $(SCHEMA)
	glib-compile-schemas $(dir $(SCHEMA))
$(OUT): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJ): obj/%.o: src/%.c include/viewer.h
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
$(SRCRES): include/viewer.gresource.xml $(GTK) $(ICONS)
	glib-compile-resources --generate-source --target $@ $<
$(LOCALE): bin/%/LC_MESSAGES/viewer.mo: text/%.po
	mkdir -p $(dir $@)
	msgfmt -o $@ $<
$(APP): include/viewer.desktop
	mkdir -p $(dir $@)
	sed -e "s#Exec=#Exec=$(EXEC)#" -e "s#Icon=#Icon=$(ICON)#" $< > $@
$(SCHEMA): include/viewer.gschema.xml $(VALID)
	mkdir -p $(dir $@)
	cp $< $@
	glib-compile-schemas $(dir $@)
$(VALID): include/viewer.gschema.xml
	glib-compile-schemas --strict --dry-run --schema-file=$< && touch $@
