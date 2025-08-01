TOP = RAM
SRC = ../../../src/VRAM
OBJ_DIR = obj_dir
EXE = $(OBJ_DIR)/V$(TOP)

CXXFLAGS_BASE = $(shell sdl2-config --cflags)
LDFLAGS_BASE  = $(shell sdl2-config --libs)

CXXFLAGS_PROF = $(CXXFLAGS_BASE) -pg
LDFLAGS_PROF  = $(LDFLAGS_BASE) -pg

CXXFLAGS_FAST = $(CXXFLAGS_BASE)
LDFLAGS_FAST  = $(LDFLAGS_BASE)

all:
	verilator -Wall -cc $(SRC)/$(TOP).v \
		--Mdir $(OBJ_DIR) \
		--exe sim_main.cpp \
		--build \
		-CFLAGS "$(CXXFLAGS_FAST)" -LDFLAGS "$(LDFLAGS_FAST)"

run: all
	@echo "[*] Running simulation (FAST)..."
	@$(EXE)

prof-build:
	verilator -Wall -cc $(SRC)/$(TOP).v \
		--Mdir $(OBJ_DIR) \
		--exe sim_main.cpp \
		--build \
		--prof-cfuncs \
		-CFLAGS "$(CXXFLAGS_PROF)" -LDFLAGS "$(LDFLAGS_PROF)"

prof: prof-build
	@echo "[*] Running simulation with profiling..."
	@$(EXE)
	@echo "[*] Generating profile.svg..."
	@gprof $(EXE) gmon.out | gprof2dot -f prof | dot -Tsvg -o profile.svg
	@echo "[*] Done. Open with: explorer.exe profile.svg"

run-only:
	@echo "[*] Running existing binary (no rebuild)..."
	@$(EXE)

clean:
	rm -rf $(OBJ_DIR) gmon.out profile.svg