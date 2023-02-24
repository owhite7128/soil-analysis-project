SRC := $(wildcard ../src/*.c)
INC := $(wildcard ../include/*.c)
OBJ_SRC := $(SRC:.c=.o)
OBJ_INC := $(INC:.c=.o)

all: build

build: $(OBJ_SRC) $(OBJ_INC)
	$(CC) -g $^ -o build -lm

../src/%.o: ../src/%.c
	$(CC) -c $< -o $@ -lm

../include/%.o: ../include/%.c
	$(CC) -c $< -o $@ -lm

prod:
	make
	rm -rf ../src/*.o ../include/*.o

clean:
	rm -rf ../src/*.o ../include/*.o build