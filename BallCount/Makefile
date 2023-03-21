CDEFINE = -DGSL # to be commented out if no GSL are installed

LIBGSL = -lgsl -lgslcblas # to be commented out if no GSL are installed

SHELL=/bin/tcsh

SYSTEM  = $(shell uname)

BINDIR  = .
OBJDIR  = .

OUT     =  $(BINDIR)/BallCount.$(SYSTEM)

ifeq "$(SYSTEM)" "Linux"
CC 	= gcc
#FC	= g77
LFLAGS	= -lm $(LIBGSL) $(LIBLAPACK)
endif

# Remember: -g with -O not allowed 
CFLAGS  = -O
#FFLAGS  = -O

CSOURCES = $(wildcard *.c)
SOURCES  = $(CSOURCES)
ALLH     = $(wildcard *.h)

COBJS = $(patsubst %.c,$(OBJDIR)/%.o,$(CSOURCES))

OBJS = $(COBJS)

all: $(OUT)

# use $(FC) instead of $(CC) because of lapack anomaly 
$(OUT): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) $(LFLAGS) -o $@;
	etags *.[ch];
#	./Store

$(OBJS): $(ALLH) Makefile

# compiling procedures
$(COBJS): $(OBJDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(CDEFINE) $< -o $@

.PHONY: clean

clean:
	rm $(OBJS) $(OUT)
