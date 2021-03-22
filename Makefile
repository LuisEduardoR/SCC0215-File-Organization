SRC = ./src/*.c
OUT = programaTrab4
all:
	gcc $(SRC) -o $(OUT) -Wall
run:
	./$(OUT)
