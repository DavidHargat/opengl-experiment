
OBJS=main.o

LIBS=-lGLEW -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm -lXxf86vm -lXinerama -lXcursor -lSOIL
FLAGS=-Wall -I.
DEPS=readfile.h vertex.glsl fragment.glsl dg.h matrix.h vec3.h maths.h

all: app
.PHONY: all

run: app 	
	./app
.PHONY: run

clean:
	rm *.o
	rm app
.PHONY: clean

app: $(OBJS) $(DEPS)
	gcc $(FLAGS) -o $@ $< $(LIBS)

%.o: %.c
	gcc $(FLAGS) -c -o $@ $^

