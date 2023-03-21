SOURCEDIR = ./sources
all:
	./configure
	( cd $(SOURCEDIR) && $(MAKE) )
