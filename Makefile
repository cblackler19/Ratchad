CC = gcc
CFLAGS = -m64
LDFLAGS = -lws2_32
OBJ = src/ratchad_res.o
SRC = src/main.c src/tos.c
TARGET = Ratchad.exe

all: $(TARGET)

$(TARGET): $(SRC) $(OBJ)
	$(CC) $(CFLAGS) $(SRC) $(OBJ) -o $(TARGET) $(LDFLAGS)

clean:
	if exist $(TARGET) del $(TARGET)