
OBJS=main.o
LIBS=-lGLEW -lglfw3

all: app
.PHONY: all

run: app
	./app
.PHONY: run

clean:
	rm *.o
	rm app
.PHONY: clean

app: $(OBJS)
	gcc $(LIBS) -o $@ $^

%.o: %.c
	gcc -c -o $@ $^

