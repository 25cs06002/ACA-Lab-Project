#!/bin/bash
Instructions
SimpleScalar Cross Compiler Installation

http://www.cse.iitd.ac.in/%7Edrajeswari/CSL718/simplescalar-installn-files.zip

Make folder in Desktop: simplescalar_project

Make sure all the files are in Desktop/simplescalar_project

f2c-1994.09.27
gcc-2.7.2.3
glibc-1.09
simplesim-3.0
simpleutils-990811
ssbig-na-sstrix
sslittle-na-sstrix

delete gcc-2.6.3 folder

open terminal inside Desktop/simplescalar_project then follow along the steps:

export IDIR=~/Desktop/simplescalar_project
export HOST=i686-pc-linux
export TARGET=sslittle-na-sstrix
cd $IDIR

cd $IDIR/simpleutils-990811
In line 588 of the file ld/ldlex.l, find yy_current_buffer and replace it with YY_CURRENT_BUFFER . (If this is not done, an error of "`yy_current_buffer' undeclared" would be thrown)

./configure --host=$HOST --target=$TARGET --with-gnu-as --with-gnu-ld --prefix=$IDIR
make CFLAGS=-O
make install

cd $IDIR/simplesim-3.0
make config-pisa
make

#replace gcc.c (inside gcc-2.7.2.3)
#replace ar & ranlib (inside sslittle-na-sstrix/bin/)

cd $IDIR/gcc-2.7.2.3
./configure --host=$HOST --target=$TARGET --with-gnu-as --with-gnu-ld --prefix=$IDIR
chmod -R +w .

In line#60 of file protoize.c, replace "#include <varargs.h>" with "#include<stdarg.h>" 
In line#341 of file obstack.h, replace "*((void **)__o->next_free)++ = ((void *)datum);\" with "*((void **)__o->next_free++)=((void *)datum);\"

cp ./patched/sys/cdefs.h ../sslittle-na-sstrix/include/sys/cdefs.h  
(To avoid error looking like "Fix these errors in the source :").

cp ../sslittle-na-sstrix/lib/libc.a ../lib/
cp ../sslittle-na-sstrix/lib/crt0.o ../lib/

make LANGUAGES=c CFLAGS=-O CC="gcc -m64"
This should throw an error : "insn-output.c:xyz: missing terminating " character".
Insert \ at the end of lines 675, 750 & 823 of file insn-output.c. The lines should look like "return "FIXME\n\"

sudo apt-get update
sudo apt-get install libc6-dev-i386 gcc-multilib g++-multilib

chmod +x ~/Desktop/simplescalar_project/sslittle-na-sstrix/bin/ar
(if permission denied)

make LANGUAGES=c CFLAGS=-O CC="gcc -m64" (#run again)
make enquire
make LANGUAGES=c CFLAGS=-O CC="gcc -m64" install

#DONE

#For implementation of LFU (Least Frequently Used), follow the steps:

open file "cache.h":
after line#105, add "LFU" in "enum cache_policy{}" function
after line#124, add "unsigned int lfu_count; /* count how many times this block was accessed */"

now open file "cache.c":
after line#398, add "blk->lfu_count = 0;"  
after line#412, add "case 'u': return LFU;"
after line#585, add """case LFU:
    {
      struct cache_blk_t *min_blk = NULL;
      struct cache_blk_t *current;

      unsigned int min_use = (unsigned int) -1;


      /* linear scan to find least frequently used */
      for (current = cp->sets[set].way_head; current; current = current->way_next) {
          if (current->lfu_count < min_use) {
              min_use = current->lfu_count;
              min_blk = current;
          }
      }
      repl = min_blk;
    }
    break;"""

after line#706, add "blk->lfu_count++;"
Again after line#737, add "blk->lfu_count++;"

after line#645, add "repl->lfu_count = 1;   // first use after load"

cd ~/Desktop/simplescalar_project/simplesim-3.0
make clean
make config-pisa
make

#DONE

cd ~/Desktop/simplescalar_project
mkdir results
cd results
mkdir lru
mkdir fifo
mkdir lfu

make file "matmul.c" in results folder
#include <stdio.h>
#define SIZE 32
int main() {
    float A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    int i, j, k;

    // Initialize matrices
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
            A[i][j] = (float)(i + j + 1);
            B[i][j] = (float)(i - j + 2);
            C[i][j] = 0.0;
        }

    // Matrix multiplication
    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
            for (k = 0; k < SIZE; k++)
                C[i][j] += A[i][k] * B[k][j];

    // Print one element to prevent optimization away
    printf("C[0][0] = %f\n", C[0][0]);
    return 0;
}

~/Desktop/simplescalar_project/sslittle-na-sstrix/bin/gcc -O2 matmul.c -o matmul.pisa
cd ~/Desktop/simplescalar_project/results

Run commands of LRU, FIFO and LFU 
	
LRU Commands: 

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:1:l -cache:il1 il1:8:16:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:2:l -cache:il1 il1:8:16:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:4:l -cache:il1 il1:8:16:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:8:l -cache:il1 il1:8:16:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:1:l -cache:il1 il1:8:32:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:2:l -cache:il1 il1:8:32:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:4:l -cache:il1 il1:8:32:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:8:l -cache:il1 il1:8:32:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:1:l -cache:il1 il1:8:64:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:2:l -cache:il1 il1:8:64:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:4:l -cache:il1 il1:8:64:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:8:l -cache:il1 il1:8:64:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_8k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:1:l -cache:il1 il1:16:16:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:2:l -cache:il1 il1:16:16:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:4:l -cache:il1 il1:16:16:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:8:l -cache:il1 il1:16:16:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:1:l -cache:il1 il1:16:32:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:2:l -cache:il1 il1:16:32:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:4:l -cache:il1 il1:16:32:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:8:l -cache:il1 il1:16:32:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:1:l -cache:il1 il1:16:64:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:2:l -cache:il1 il1:16:64:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:4:l -cache:il1 il1:16:64:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:8:l -cache:il1 il1:16:64:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_16k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:1:l -cache:il1 il1:32:16:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:2:l -cache:il1 il1:32:16:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:4:l -cache:il1 il1:32:16:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:8:l -cache:il1 il1:32:16:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:1:l -cache:il1 il1:32:32:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:2:l -cache:il1 il1:32:32:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:4:l -cache:il1 il1:32:32:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:8:l -cache:il1 il1:32:32:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:1:l -cache:il1 il1:32:64:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:2:l -cache:il1 il1:32:64:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:4:l -cache:il1 il1:32:64:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:8:l -cache:il1 il1:32:64:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_32k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:1:l -cache:il1 il1:64:16:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:2:l -cache:il1 il1:64:16:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:4:l -cache:il1 il1:64:16:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:8:l -cache:il1 il1:64:16:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:1:l -cache:il1 il1:64:32:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:2:l -cache:il1 il1:64:32:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:4:l -cache:il1 il1:64:32:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:8:l -cache:il1 il1:64:32:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:1:l -cache:il1 il1:64:64:1:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:2:l -cache:il1 il1:64:64:2:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:4:l -cache:il1 il1:64:64:4:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:8:l -cache:il1 il1:64:64:8:l -cache:dl2 none -cache:il2 none -redir:sim ./lru/output_dl1_64k_b64_a8.txt matmul.pisa

FIFO Commands: 

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:1:f -cache:il1 il1:8:16:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:2:f -cache:il1 il1:8:16:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:4:f -cache:il1 il1:8:16:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:8:f -cache:il1 il1:8:16:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:1:f -cache:il1 il1:8:32:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:2:f -cache:il1 il1:8:32:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:4:f -cache:il1 il1:8:32:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:8:f -cache:il1 il1:8:32:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:1:f -cache:il1 il1:8:64:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:2:f -cache:il1 il1:8:64:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:4:f -cache:il1 il1:8:64:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:8:f -cache:il1 il1:8:64:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_8k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:1:f -cache:il1 il1:16:16:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:2:f -cache:il1 il1:16:16:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:4:f -cache:il1 il1:16:16:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:8:f -cache:il1 il1:16:16:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:1:f -cache:il1 il1:16:32:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:2:f -cache:il1 il1:16:32:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:4:f -cache:il1 il1:16:32:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:8:f -cache:il1 il1:16:32:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:1:f -cache:il1 il1:16:64:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:2:f -cache:il1 il1:16:64:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:4:f -cache:il1 il1:16:64:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:8:f -cache:il1 il1:16:64:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_16k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:1:f -cache:il1 il1:32:16:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:2:f -cache:il1 il1:32:16:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:4:f -cache:il1 il1:32:16:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:8:f -cache:il1 il1:32:16:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:1:f -cache:il1 il1:32:32:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:2:f -cache:il1 il1:32:32:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:4:f -cache:il1 il1:32:32:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:8:f -cache:il1 il1:32:32:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:1:f -cache:il1 il1:32:64:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:2:f -cache:il1 il1:32:64:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:4:f -cache:il1 il1:32:64:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:8:f -cache:il1 il1:32:64:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_32k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:1:f -cache:il1 il1:64:16:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:2:f -cache:il1 il1:64:16:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:4:f -cache:il1 il1:64:16:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:8:f -cache:il1 il1:64:16:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:1:f -cache:il1 il1:64:32:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:2:f -cache:il1 il1:64:32:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:4:f -cache:il1 il1:64:32:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:8:f -cache:il1 il1:64:32:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:1:f -cache:il1 il1:64:64:1:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:2:f -cache:il1 il1:64:64:2:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:4:f -cache:il1 il1:64:64:4:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:8:f -cache:il1 il1:64:64:8:f -cache:dl2 none -cache:il2 none -redir:sim ./fifo/output_dl1_64k_b64_a8.txt matmul.pisa

LFU Commands:

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:1:u -cache:il1 il1:8:16:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:2:u -cache:il1 il1:8:16:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:4:u -cache:il1 il1:8:16:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:16:8:u -cache:il1 il1:8:16:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:1:u -cache:il1 il1:8:32:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:2:u -cache:il1 il1:8:32:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:4:u -cache:il1 il1:8:32:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:32:8:u -cache:il1 il1:8:32:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:1:u -cache:il1 il1:8:64:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:2:u -cache:il1 il1:8:64:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:4:u -cache:il1 il1:8:64:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:8:64:8:u -cache:il1 il1:8:64:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_8k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:1:u -cache:il1 il1:16:16:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:2:u -cache:il1 il1:16:16:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:4:u -cache:il1 il1:16:16:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:16:8:u -cache:il1 il1:16:16:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:1:u -cache:il1 il1:16:32:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:2:u -cache:il1 il1:16:32:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:4:u -cache:il1 il1:16:32:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:32:8:u -cache:il1 il1:16:32:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:1:u -cache:il1 il1:16:64:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:2:u -cache:il1 il1:16:64:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:4:u -cache:il1 il1:16:64:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:16:64:8:u -cache:il1 il1:16:64:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_16k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:1:u -cache:il1 il1:32:16:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:2:u -cache:il1 il1:32:16:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:4:u -cache:il1 il1:32:16:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:16:8:u -cache:il1 il1:32:16:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:1:u -cache:il1 il1:32:32:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:2:u -cache:il1 il1:32:32:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:4:u -cache:il1 il1:32:32:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:32:8:u -cache:il1 il1:32:32:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:1:u -cache:il1 il1:32:64:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:2:u -cache:il1 il1:32:64:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:4:u -cache:il1 il1:32:64:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:32:64:8:u -cache:il1 il1:32:64:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_32k_b64_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:1:u -cache:il1 il1:64:16:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b16_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:2:u -cache:il1 il1:64:16:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b16_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:4:u -cache:il1 il1:64:16:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b16_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:16:8:u -cache:il1 il1:64:16:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b16_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:1:u -cache:il1 il1:64:32:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b32_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:2:u -cache:il1 il1:64:32:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b32_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:4:u -cache:il1 il1:64:32:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b32_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:32:8:u -cache:il1 il1:64:32:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b32_a8.txt matmul.pisa

../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:1:u -cache:il1 il1:64:64:1:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b64_a1.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:2:u -cache:il1 il1:64:64:2:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b64_a2.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:4:u -cache:il1 il1:64:64:4:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b64_a4.txt matmul.pisa
../simplesim-3.0/sim-cache -cache:dl1 dl1:64:64:8:u -cache:il1 il1:64:64:8:u -cache:dl2 none -cache:il2 none -redir:sim ./lfu/output_dl1_64k_b64_a8.txt matmul.pisa

