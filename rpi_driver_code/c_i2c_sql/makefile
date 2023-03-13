SRC := $(wildcard src/*.c)
INC := $(wildcard include/*.c)
OBJ_SRC := $(SRC:.c=.o)
OBJ_INC := $(INC:.c=.o)

all: build

build: $(OBJ_SRC) $(OBJ_INC)
	$(CC) $(mysql_config --cflags) -g $^ -o runtime $(mysql_config --libs) -l bcm2835

src/%.o: src/%.c
	$(CC) $(mysql_config --cflags) -c $< -o $@ $(mysql_config --libs) -l bcm2835

include/%.o: include/%.c
	$(CC) $(mysql_config --cflags) -c $< -o $@ $(mysql_config --libs) -l bcm2835

clean:
	rm -rf src/*.o include/*.o build
