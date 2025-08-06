TOP = FIFO
SRC = ../../../src/FIFO
OBJ_DIR = obj_dir
EXE = $(OBJ_DIR)/V$(TOP)

VERILOG_SOURCES = \
	$(SRC)/FIFO.v \
	$(SRC)/FIFO_memory.v \
	$(SRC)/rptr_empty.v \
	$(SRC)/two_ff_sync.v \
	$(SRC)/wptr_full.v

all:
	verilator -Wall -cc $(VERILOG_SOURCES) \
		--top-module $(TOP) \
		--Mdir $(OBJ_DIR) \
		--exe sim_main.cpp \
		--build

run: all
	@echo "[*] Running simulation (FAST)..."
	@$(EXE)

prof-build:
	verilator -Wall -cc $(VERILOG_SOURCES) \
		--top-module $(TOP) \
		--Mdir $(OBJ_DIR) \
		--exe sim_main.cpp \
		--build \
		--prof-cfuncs

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