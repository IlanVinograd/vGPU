TOP = RAM
SRC = ../../../src/VRAM
OBJ_DIR = obj_dir
EXE = $(OBJ_DIR)/V$(TOP)

CXXFLAGS += $(shell sdl2-config --cflags)
LDFLAGS  += $(shell sdl2-config --libs)

all:
	verilator -Wall -cc $(SRC)/$(TOP).v \
		--Mdir $(OBJ_DIR) \
		--exe sim_main.cpp \
		--build \
		-CFLAGS "$(CXXFLAGS)" -LDFLAGS "$(LDFLAGS)"

run: all
	@echo "[*] Running simulation..."
	@$(EXE)

run-only:
	@echo "[*] Running existing binary (no rebuild)..."
	@$(EXE)

clean:
	rm -rf $(OBJ_DIR)