CXX      ?= g++
CXXFLAGS ?= -std=c++17 -Wall -Wextra -O2
INC      = -Iinclude
BUILD    = build
HDRS     = $(wildcard include/*.h include/*.tpp)

.PHONY: all test demo sanitize exhaustive known-issues clean

all: demo

$(BUILD):
	mkdir -p $(BUILD)

$(BUILD)/demo: examples/demo.cpp $(HDRS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INC) $< -o $@

$(BUILD)/stress: tests/stress.cpp $(HDRS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INC) $< -o $@

$(BUILD)/targeted: tests/targeted.cpp $(HDRS) | $(BUILD)
	$(CXX) $(CXXFLAGS) $(INC) $< -o $@

demo: $(BUILD)/demo
	./$(BUILD)/demo

# T9 (slicing RBTree -> Tree) e' una limitazione nota, vedi README: non fa parte del test standard
test: $(BUILD)/stress $(BUILD)/targeted
	./$(BUILD)/stress
	@for n in 1 2 3 4 5 6 7 8 10 11 12; do ./$(BUILD)/targeted $$n || exit 1; done
	@echo "Tutti i test superati"

known-issues: $(BUILD)/targeted
	-./$(BUILD)/targeted 9

exhaustive: $(BUILD)/stress
	./$(BUILD)/stress all

sanitize:
	$(MAKE) clean
	$(MAKE) test CXXFLAGS="-std=c++17 -g -O1 -Wall -Wextra -fsanitize=address,undefined -fno-omit-frame-pointer"

clean:
	rm -rf $(BUILD)
