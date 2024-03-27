CC=gcc
CFLAGS=-Wall -g -Iinclude
LDFLAGS=
SOURCES=src/episodeA.c src/pathchange.c src/filecopy.c src/deletefile.c src/runprogram.c src/addcontent.c src/replacecontent.c src/printfile.c src/countlineswords.c src/main.c src/episodeB.c src/episodeC.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=AlonsShell

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.c.o:
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXECUTABLE)

.PHONY: clean
