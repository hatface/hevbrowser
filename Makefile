# Makefile for hevbrowser

PP=cpp
CC=gcc
STRIP=strip
CCFLAGS=-O2 `pkg-config --cflags gtk+-3.0 webkitgtk-3.0 javascriptcoregtk-3.0`
LDFLAGS=`pkg-config --libs gtk+-3.0 webkitgtk-3.0 javascriptcoregtk-3.0`

SRCDIR=src
BINDIR=bin
BUILDDIR=build

TARGET=$(BINDIR)/hevbrowser
CCOBJSFILE=$(BUILDDIR)/ccobjs
-include $(CCOBJSFILE)
LDOBJS=$(patsubst $(SRCDIR)%.c,$(BUILDDIR)%.o,$(CCOBJS))

DEPEND=$(LDOBJS:.o=.dep)

.PHONY : all
all : $(BINDIR) $(BUILDDIR) $(CCOBJSFILE) $(TARGET)

.PHONY : clean
clean : 
	@echo -n "Clean ... " && $(RM) -r $(BINDIR) $(BUILDDIR)  && echo "OK"

$(BINDIR) : 
	@mkdir $@

$(BUILDDIR) :
	@mkdir $@
 
$(CCOBJSFILE) : $(SRCDIR) $(BUILDDIR)
	@echo CCOBJS=`ls $(SRCDIR)/*.c` > $(CCOBJSFILE)
 
$(TARGET) : $(LDOBJS)
	@echo -n "Linking $^ to $@ ... " && $(CC) -o $@ $^ $(LDFLAGS) && $(STRIP) $@ && echo "OK"
 
$(BUILDDIR)/%.dep : $(SRCDIR)/%.c
	@$(PP) $(CCFLAGS) -MM -MT $(@:.dep=.o) -o $@ $<
 
$(BUILDDIR)/%.o : $(SRCDIR)/%.c
	@echo -n "Building $< ... " && $(CC) $(CCFLAGS) -c -o $@ $< && echo "OK"
 
-include $(DEPEND)

