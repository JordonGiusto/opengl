CC=g++
FLAGS= -lGL -lGLU -lglfw3 -lX11 -lXxf86vm -lXrandr -lpthread -lXi -lglut -lGLEW

main: main.o
	$(CC) main.cpp -o main $(FLAGS)

clean:
	rm -f *.o