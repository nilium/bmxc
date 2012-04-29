CC?=clang
CFLAGS+= -g

SOURCES=\
  src/lexer.c \
  src/lib/object.cc \
  src/main.c \
  src/scope.cc

OBJECTS=\
src/lexer.o \
  src/lib/object.o \
  src/main.o \
  src/scope.o

BIN=bin/bmxlex

.PHONY: all clean

all: $(BIN)

src/lexer.o: src/lexer.c src/lexer.h
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

src/lib/object.o: src/lib/object.cc 
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

src/main.o: src/main.c src/lexer.h
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

src/scope.o: src/scope.cc 
	$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@

$(BIN): $(OBJECTS)
	$(CC) -o $@ $(OBJECTS)

clean:
	$(RM) $(BIN) $(OBJECTS)
