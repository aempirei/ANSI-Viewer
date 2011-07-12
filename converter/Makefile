CC = gcc
CFLAGS = -Wall -I.
CCFLAGS = -Wall -I.
PROGRAMS = defont ansitoprop
INSTALL_PATH = bin
BINS =  ansi.bin palette.bin font.bin
INSTALL_FILES = $(BINS) readme.txt

.PHONY: clean install wipe bins

install: bins
	mkdir -p $(INSTALL_PATH)
	cp -v $(INSTALL_FILES) $(INSTALL_PATH)

defont: defont.o
	$(CC) $(CCFLAGS) -lm -o $@ $<

ansitoprop: ansitoprop.o
	$(CC) $(CCFLAGS) -lm -o $@ $<

bins: $(PROGRAMS) ANSI.ANS
	./ansitocommands.pl < ANSI.ANS | ./ansitoprop > ansi.bin

clean:
	rm -f *.o *~ $(BINS)

wipe: clean
	rm -f $(PROGRAMS)
