CC = gcc
LD = ld

BUILD_DIR = build

CFLAGS = \
	-I. \
	-ffreestanding \
	-fno-pie \
	-fno-stack-protector \
	-nostdlib \
	-c

LDFLAGS = \
	-Ttext 0x0 \
	--oformat binary

SRC_GATE = البوابة.c



OBJ_GATE = \
	$(BUILD_DIR)/البوابة.o



BIN = \
	$(BUILD_DIR)/msg.bin

OBJS = \
	$(OBJ_GATE) 


all: $(BUILD_DIR) $(BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_GATE): $(SRC_GATE)
	$(CC) $(CFLAGS) $(SRC_GATE) -o $(OBJ_GATE)



$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(BIN)

clean:
	rm -rf $(BUILD_DIR)
