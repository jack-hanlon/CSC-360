.PHONY all:
all: diskinfo disklist diskget

diskinfo: diskinfo.c
	gcc -o diskinfo diskinfo.c

disklist: disklist.c
	gcc -o disklist disklist.c

diskget: diskget.c
	gcc -o diskget diskget.c

.PHONY clean:
clean:
	-rm diskinfo disklist diskget 
