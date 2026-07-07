BIN     = $(EXE)
BINDIR  = bin
CFLAGS  = -I$(INCDIR) -Wall $(shell pkg-config gtk+-3.0 --cflags)
DNDEBUG = -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS
EXE     = $(BINDIR)/$(NAME)
GTK     = $(wildcard gtk/*.ui)
ICONS   = $(wildcard icons/scalable/actions/*.svg)
INCDIR  = include
INC     = $(INCDIR)/viewer.h
LIBS    = $(shell pkg-config gtk+-3.0 --libs)
OBJ     = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJDIR  = obj
NAME    = viewer
SRC     = $(addprefix $(SRCDIR)/, about.c app.c document.c main.c resource.c)
SRCDIR  = src
.PHONY: all build clean
all:
	$(MAKE) build "CFLAGS = $(CFLAGS) $(DNDEBUG)"
build: $(EXE)
clean:
	$(RM) $(BIN) $(OBJ) $(SRCDIR)/resource.c
$(EXE): $(OBJ)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(INC)
	mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c -o $@ $<
$(SRCDIR)/resource.c: $(INCDIR)/viewer.gresource.xml $(GTK) $(ICONS)
	glib-compile-resources --generate-source --target $@ $<
