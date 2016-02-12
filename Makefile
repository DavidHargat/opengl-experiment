
OBJS=main.o

LIBS=-lGLEW -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm -lXxf86vm -lXinerama -lXcursor
FLAGS=-Wall -I.
DEPS=readfile.h

all: app
.PHONY: all

run: app $(DEPS)
	./app
.PHONY: run

clean:
	rm *.o
	rm app
.PHONY: clean

app: $(OBJS)
	gcc $(FLAGS) -o $@ $^ $(LIBS)

%.o: %.c
	gcc $(FLAGS) -c -o $@ $^

