BIN     = $(EXE)
BINDIR  = bin
CFLAGS  = -I$(INCDIR) -Wall $(shell pkg-config gtk+-3.0 --cflags)
DNDEBUG = -O2 -DNDEBUG -DG_DISABLE_ASSERT -DG_DISABLE_CAST_CHECKS
EXE     = $(BINDIR)/$(NAME)
INCDIR  = include
INC     = $(INCDIR)/viewer.h
LIBS    = $(shell pkg-config gtk+-3.0 --libs)
OBJ     = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJDIR  = obj
NAME    = viewer
SRC     = $(addprefix $(SRCDIR)/, app.c document.c main.c)
SRCDIR  = src
.PHONY: all build clean
all:
	@$(MAKE) build "CFLAGS = $(CFLAGS) $(DNDEBUG)"
build: $(EXE)
clean:
	$(RM) $(BIN) $(OBJ)
$(EXE): $(OBJ)
	@echo $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(INC)
	@echo $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
