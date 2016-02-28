
OBJS=main.o

LIBS=-lGLEW -lglfw3 -lGL -lX11 -lpthread -lXrandr -lXi -ldl -lm -lXxf86vm -lXinerama -lXcursor -lSOIL
FLAGS=-Wall -I.
DEPS=readfile.h vertex.glsl fragment.glsl dlib.h

all: app
.PHONY: all

run: app 	
	./app
.PHONY: run

matrix:
	echo "updating matrix library"
	curl -s "https://raw.githubusercontent.com/DavidHargat/matrix-fun/master/matrix.h" -o matrix.h

clean:
	rm *.o
	rm app
.PHONY: clean

app: $(OBJS) $(DEPS)
	gcc $(FLAGS) -o $@ $< $(LIBS)

%.o: %.c
	gcc $(FLAGS) -c -o $@ $^

