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

compileBuddy: git
	@gcc $(shell find src/ -name "pmm.c")  \
	     $(shell find src/ -name "chunks.c")  \
	     $(shell find src/ -name "buddys.c")  \
	     $(shell find src/ -name "spinlock.c")  \
	     $(shell find src/ -name "slabs.c")  \
	     $(shell find test/ -name "*.c") \
	     -D BUDDY \
	     -I ./include \
      	     -w -Iframework -Itest -DTEST -lpthread \
	     -fsanitize=address \
	     -ggdb \
             -o build/test

compileSlab: git
	@gcc $(shell find src/ -name "pmm.c")  \
	     $(shell find src/ -name "chunks.c")  \
	     $(shell find src/ -name "buddys.c")  \
	     $(shell find src/ -name "spinlock.c")  \
	     $(shell find src/ -name "slabs.c")  \
	     $(shell find test/ -name "*.c") \
	     -D SLAB \
	     -I ./include \
      	     -w -Iframework -Itest -DTEST -lpthread \
	     -fsanitize=address \
	     -ggdb \
             -o build/test

debug:
	qemu-system-x86_64 -nographic -serial mon:stdio -machine accel=tcg -smp 4 -drive format=raw,file=/home/liscopye/NJUOS/os-workbench/kernel/build/kernel-x86_64-qemu &
	cgdb -x gdb.init
testLock:
	qemu-system-x86_64  -nographic -serial mon:stdio -machine accel=tcg -smp 4  -drive format=raw,file=/home/liscopye/NJUOS/os-workbench/kernel/build/kernel-x86_64-qemu 

debugTest:
	gdb -x init.gdb


buddyTest0:
	@build/test 0

buddyTest1:
	@build/test 1

buddyTest2:
	@build/test 2

buddyTest3:
	@build/test 3

testAllbuddy:
	@build/test 0
	@build/test 1
	@build/test 2
	@build/test 3

slabTest0:
	@build/test 0

slabTest1:
	@build/test 1

slabTest2:
	@build/test 2

slabTest3:
	@build/test 3

testAllSlab:
	@build/test 0
	@build/test 1
	@build/test 2
	@build/test 3

