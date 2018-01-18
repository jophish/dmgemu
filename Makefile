#Compiler and Linker
CC          := gcc

#The Target Binary Program
TARGET      := dmgemu

#The Directories, Source, Includes, Objects, Binary and Resources
SRCDIR      := src
INCDIR      := src/inc
BUILDDIR    := obj
TARGETDIR   := bin
RESDIR      := res
SRCEXT      := c
DEPEXT      := h
OBJEXT      := o

#Flags, Libraries and Includes
CFLAGS      := -fopenmp -Wall -Werror -O3 -D_OPENBSD_SOURCE
LIB         :=
INC         := -I$(INCDIR) -I/usr/local/include
INCDEP      := -I$(INCDIR)

#---------------------------------------------------------------------------------
#DO NOT EDIT BELOW THIS LINE
#---------------------------------------------------------------------------------
SOURCES     := $(shell find $(SRCDIR) -type f -name *.$(SRCEXT))
OBJECTS     := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(SOURCES:.$(SRCEXT)=.$(OBJEXT)))

#Defauilt Make
all: resources $(TARGET)

# Debug builds
debug: CFLAGS += -DDEBUG -g
debug: resources $(TARGET)

#Remake
remake: cleaner all

#Copy Resources from Resources Directory to Target Directory
resources: directories


#Make the Directories
directories:
	\mkdir -p $(TARGETDIR)
	\mkdir -p $(BUILDDIR)

#Clean only Objecst
clean:
	\$(RM) -rf $(BUILDDIR)

#Full Clean, Objects and Binaries
cleaner: clean
	\$(RM) -rf $(TARGETDIR)

#Pull in dependency info for *existing* .o files
-include $(OBJECTS:.$(OBJEXT)=.$(DEPEXT))

#Link
$(TARGET): $(OBJECTS)
	\$(CC) -o $(TARGETDIR)/$(TARGET) $^ $(LIB)

#Compile
$(BUILDDIR)/%.$(OBJEXT): $(SRCDIR)/%.$(SRCEXT)
	\mkdir -p $(dir $@)
	\$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	\$(CC) $(CFLAGS) $(INCDEP) -MM $(SRCDIR)/$*.$(SRCEXT) > $(BUILDDIR)/$*.$(DEPEXT)
	\cp -f $(BUILDDIR)/$*.$(DEPEXT) $(BUILDDIR)/$*.$(DEPEXT).tmp
	\sed -e 's|.*:|$(BUILDDIR)/$*.$(OBJEXT):|' < $(BUILDDIR)/$*.$(DEPEXT).tmp > $(BUILDDIR)/$*.$(DEPEXT)
	\sed -e 's/.*://' -e 's/\\$$//' < $(BUILDDIR)/$*.$(DEPEXT).tmp | fmt -1 | sed -e 's/^ *//' -e 's/$$/:/' >> $(BUILDDIR)/$*.$(DEPEXT)
	\rm -f $(BUILDDIR)/$*.$(DEPEXT).tmp

#Non-File Targets
.PHONY: all remake clean cleaner resources debug
