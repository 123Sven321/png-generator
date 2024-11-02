
CC = gcc
TARGET = mandelbrot-gen
LIBS = -lz
SRC = mandelbrot-generator.c ../png-generator.c


all: $(TARGET)

$(TARGET): mandelbrot-generator.c
	${CC} -o $(TARGET) $(SRC) $(LIBS)


clean:
	rm -f $(TARGET)