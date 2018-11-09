LLC ?= llc
CLANG ?= clang
CC= gcc
CFLAGS=-g
BPFOBJ= calc.o
IFACE=veth0
SRC=/root/p4c
INCLUDE=
P4C=p4c-xdp

all: $(BPFOBJ)

# Verify LLVM compiler tools are available and bpf target is supported by llc
.PHONY: verify_cmds verify_target_bpf $(CLANG) $(LLC)

verify_cmds: $(CLANG) $(LLC)
	@for TOOL in $^ ; do \
		if ! (which -- "$${TOOL}" > /dev/null 2>&1); then \
			echo "*** ERROR: Cannot find LLVM tool $${TOOL}" ;\
			exit 1; \
		else \
			echo "pass verify_cmds:" \
			true; fi; \
	done

verify_target_bpf: verify_cmds
	@if ! (${LLC} -march=bpf -mattr=help > /dev/null 2>&1); then \
		echo "*** ERROR: LLVM (${LLC}) does not support 'bpf' target" ;\
		echo "   NOTICE: LLVM version >= 3.7.1 required" ;\
		exit 2; \
	else \
		echo "pass verify_target_bpf:" \
		true; fi

# P4 generate .c
# keep the intermediate .c file
.PRECIOUS: %.c
%.c: %.p4
	@if ! ($(P4C) --help > /dev/null 2>&1); then \
		echo "*** ERROR: Cannot find p4c-xdp"; \
		exit 1;\
	fi;
	$(P4C) --Werror $(INCLUDE) --target xdp -o $@ $<;

# BPF program
# do extra CLANG so first pass can return non-zero to shell and stop make
%.o: %.c
	@$(CLANG) $(INCLUDE) \
		-D__KERNEL__ -D__ASM_SYSREG_H -Wno-unused-value -Wno-pointer-sign \
		-Wno-compare-distinct-pointer-types \
		-Wno-gnu-variable-sized-type-not-at-end \
		-Wno-tautological-compare \
		-O2 -emit-llvm -g -c $< -o - > /dev/null
	$(CLANG) $(INCLUDE)\
		-D__KERNEL__ -D__ASM_SYSREG_H -Wno-unused-value -Wno-pointer-sign \
		-Wno-compare-distinct-pointer-types \
		-Wno-gnu-variable-sized-type-not-at-end \
		-Wno-tautological-compare \
		-O2 -emit-llvm -g -c $< -o -| $(LLC) -march=bpf -filetype=obj -o $@
	sudo ./$(BPFLOADER) $@ || true

clean: clean_loader
	rm -f *.o *_ebpf.c xdp[1-9]*.[ch]

clean_loader:
	rm -f bpfloader

# For actually attaching BPF
attach:
	ip link show $(IFACE); \
	for TOOL in $(BPFOBJ); do \
		ip link set dev $(IFACE) xdp obj $${TOOL} verb; \
		ip link set dev $(IFACE) xdp off; \
	done
