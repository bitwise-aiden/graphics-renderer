build:
	mkdir build && gcc -Wall -std=c99 ./src/*.c -lSDL2 -lm -o ./build/renderer

run:
	./build/renderer

clean:
	rm -rf build
