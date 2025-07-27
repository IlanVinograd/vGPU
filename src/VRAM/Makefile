TOP = RAM
OBJ_DIR = obj_dir

all:
	verilator -Wall -cc $(TOP).v --exe sim_main.cpp --build

run: all 
	./$(OBJ_DIR)/V$(TOP)

clean:
	rm -rf $(OBJ_DIR) *.log *.dmp