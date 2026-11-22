.PHONY: clean run report

CC=gcc -std=c17

TARGET_NAME=main


all:
	@mkdir -p build
	$(CC) src/main.c src/ring_buffer.c -Wall -Wextra -Iinclude -o build/main
run: all
	./build/main
clean:
	rm -rf build/*

report:
	$(CC) src/main.c src/ring_buffer.c -Iinclude \
	-Wall -Wextra -Wpedantic -Wnull-dereference -Wcast-align \
	-Wduplicated-cond -Wduplicated-branches -Wlogical-op \
	-Wswitch-default -Wswitch-enum -Wfloat-equal \
	-Wshadow -Wconversion -Wsign-conversion -Wdouble-promotion \
	-o build/main 
	