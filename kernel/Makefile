export MODULE  := L1
NAME           := kernel
SRCS           := framework/main.c $(shell find -L ./src/ -name "*.c")
INC_PATH       := include/ framework/

export AM_HOME := $(PWD)/../abstract-machine
ifeq ($(ARCH),)
export ARCH := x86_64-qemu
endif

include $(AM_HOME)/Makefile
include ../Makefile.lab
image: git

test: git
	@gcc $(shell find src/ -name "pmm.c")  \
	     $(shell find src/ -name "chunks.c")  \
	     $(shell find src/ -name "buddys.c")  \
	     $(shell find src/ -name "spinlock.c")  \
	     $(shell find test/ -name "*.c") \
	     -I ./include \
      	     -w -Iframework -Itest -DTEST -lpthread \
	     -ggdb \
             -o build/test
	@build/test 2> /dev/null
debug:
	qemu-system-x86_64 -nographic -serial mon:stdio -machine accel=tcg -smp 4 -drive format=raw,file=/home/liscopye/NJUOS/os-workbench/kernel/build/kernel-x86_64-qemu &
	gdb -x gdb.init
testlock:
	qemu-system-x86_64  -nographic -serial mon:stdio -machine accel=tcg -smp 4  -drive format=raw,file=/home/liscopye/NJUOS/os-workbench/kernel/build/kernel-x86_64-qemu 

debugtest:
	gdb -x init.gdb


