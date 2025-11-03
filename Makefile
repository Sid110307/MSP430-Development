CCS_ROOT    := C:/Progra~1/TI/CCS2031
COMPILER    := $(CCS_ROOT)/ccs/tools/compiler/ti-cgt-msp430_21.6.1.LTS
CCS_INCLUDE := $(CCS_ROOT)/ccs/ccs_base/msp430/include
CC          := "$(COMPILER)/bin/cl430.exe"
HEX430      := "$(COMPILER)/bin/hex430.exe"
FLASHER     := "C:/Progra~1/TI/UniFlash_9.3.0/dslite.bat"
LINKCMD     := lib/link.cmd

SRC_DIR  := src
BIN_DIR  := bin
SRCS     := $(wildcard $(SRC_DIR)/*.c)
OBJS     := $(patsubst $(SRC_DIR)/%.c,$(BIN_DIR)/%.obj,$(SRCS))

OUT      := $(BIN_DIR)/firmware.out
HEX      := $(BIN_DIR)/firmware.txt

COMMON_FLAGS := \
    -vmsp \
    --use_hw_mpy=none \
    --advice:power=all \
    -g \
    -Os \
    -D __MSP430G2553__ \
    --printf_support=minimal \
    --code_model=small \
    --data_model=small \
    --opt_for_speed=0 \
    --gen_func_subsections=on \
    --gen_data_subsections=on \
    --diag_warning=225 \
    --diag_wrap=off \
    --display_error_number \
    --include_path=$(COMPILER)/include \
    --include_path=$(CCS_INCLUDE) \
    --include_path=$(SRC_DIR)

.PHONY: all flash clean
all: $(HEX)

$(BIN_DIR):
	mkdir $(BIN_DIR)

$(BIN_DIR)/%.obj: $(SRC_DIR)/%.c | $(BIN_DIR)
	$(CC) $(COMMON_FLAGS) --preproc_with_compile --obj_directory=$(BIN_DIR) --asm_directory=$(BIN_DIR) -c $< -o $@

$(OUT): $(OBJS) $(LINKCMD)
	$(CC) $(COMMON_FLAGS) -z --rom_model \
		--unused_section_elimination \
		-m$(BIN_DIR)/firmware.map \
		--heap_size=64 --stack_size=64 \
		-i$(CCS_INCLUDE) \
		-i$(COMPILER)/lib \
		-i$(COMPILER)/include \
		$(LINKCMD) \
		$(OBJS) \
		-o $(OUT)

$(HEX): $(OUT)
	$(HEX430) --memwidth=8 --romwidth=8 --diag_wrap=off --ti_txt -o $(HEX) $(OUT)

flash: $(OUT)
	$(FLASHER) --config=lib/MSP430G2553.ccxml --flash --verify $(OUT)

clean:
	-rm -rf $(BIN_DIR)
