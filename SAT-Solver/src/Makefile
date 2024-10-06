CC = g++
FLAGS = --std=c++11
TARGET = SAT_Solver
PYTHON_VERIFIER = verification_SAT.py

INPUT_DIR = ./CNF_Files
OUTPUT_DIR = ./Results

CNF_FILES := $(wildcard $(INPUT_DIR)/*.cnf)
OUT_FILES := $(patsubst $(INPUT_DIR)/%.cnf, $(OUTPUT_DIR)/%.out, $(CNF_FILES))

all:
	@echo "Building Solver..."
	$(CC) $(FLAGS) SAT_Solver.cpp -o $(TARGET)

process: $(OUT_FILES)
$(OUTPUT_DIR)/%.out: $(INPUT_DIR)/%.cnf
	@mkdir -p $(OUTPUT_DIR)
	@echo "Processing $< ..."
	@./$(TARGET) < $< > $@
	@echo "All .cnf files have been processed."

verify: $(OUT_FILES)
	@echo "Verifying all solutions..."
	@for out_file in $(OUT_FILES); do \
		base_name=$$(basename $$out_file .out); \
		cnf_file="$(INPUT_DIR)/$$base_name.cnf"; \
		echo "Verifying $$out_file with $$cnf_file ..."; \
		if [ -f "$$cnf_file" ]; then \
			python3 $(PYTHON_VERIFIER) $$cnf_file $$out_file; \
		else \
			echo "Warning: CNF file $$cnf_file not found."; \
		fi; \
	done
	@echo "All solutions have been verified."

clean:
	@echo "Cleaning up..."
	rm -f $(TARGET)
	rm -rf $(OUTPUT_DIR)
	@echo "Done."
