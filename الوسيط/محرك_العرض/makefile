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

SRC_SERVER = \
	المحركات/خادم_العرض/الخادم.c

SRC_BACKGROUND = \
	المحركات/خادم_العرض/الأدارة/الرسم/الرسم.c

OBJ_GATE = \
	$(BUILD_DIR)/البوابة.o

OBJ_SERVER = \
	$(BUILD_DIR)/الخادم.o

OBJ_BACKGROUND = \
	$(BUILD_DIR)/الرسم.o

BIN = \
	$(BUILD_DIR)/app.bin

OBJS = \
	$(OBJ_GATE) \
	$(OBJ_SERVER) \
	$(OBJ_BACKGROUND)

all: $(BUILD_DIR) $(BIN)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(OBJ_GATE): $(SRC_GATE)
	$(CC) $(CFLAGS) $(SRC_GATE) -o $(OBJ_GATE)

$(OBJ_SERVER): $(SRC_SERVER)
	$(CC) $(CFLAGS) $(SRC_SERVER) -o $(OBJ_SERVER)

$(OBJ_BACKGROUND): $(SRC_BACKGROUND)
	$(CC) $(CFLAGS) $(SRC_BACKGROUND) -o $(OBJ_BACKGROUND)

$(BIN): $(OBJS)
	$(LD) $(LDFLAGS) $(OBJS) -o $(BIN)

clean:
	rm -rf $(BUILD_DIR)
