#LIBGSL = -lgsl -lgslcblas # to be commented out if no GSL are installed
#LIBLAPACK = -llapack -lblas # to be commented out if no LAPACK are installed

#CDEFINE = -DLAPACK -DGSL

SHELL=/bin/tcsh

SYSTEM  = $(shell uname)

BINDIR  = ../bin
OBJDIR  = ../objs

OUT     =  $(BINDIR)/Pallette.$(SYSTEM)

ifeq "$(SYSTEM)" "Linux"
CC 	= gcc
LFLAGS	= -lm $(LIBGSL) $(LIBLAPACK)
endif

# Remember: -g with -O not allowed 
CFLAGS  = -g

CSOURCES = $(wildcard *.c)
SOURCES  = $(CSOURCES)
ALLH     = $(wildcard *.h)

COBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(CSOURCES))

OBJS = $(COBJS)

all: $(OUT)

$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@;
	etags *.[ch];
	./Store

$(OBJS): $(ALLH) Makefile

# compiling procedures
$(COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CDEFINE) $< -o $@

.PHONY: clean

clean:
	rm $(OBJS) $(OUT)
