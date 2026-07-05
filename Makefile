BIN     = $(EXE)
BINDIR  = bin
CFLAGS  =
EXE     = $(BINDIR)/$(NAME)
INCLUDE =
LIBS    =
OBJ     = $(SRC:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
OBJDIR  = obj
NAME    = viewer
SRC     = $(addprefix $(SRCDIR)/, main.c)
SRCDIR  = src
.PHONY: all clean
all: $(EXE)
clean:
	$(RM) $(BIN) $(OBJ)
$(EXE): $(OBJ)
	@echo $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -o $@ $(OBJ) $(LIBS)
$(OBJ): $(OBJDIR)/%.o: $(SRCDIR)/%.c $(INCLUDE)
	@echo $@
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c -o $@ $<
