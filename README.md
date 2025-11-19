# ⚙️ ACA Project — SimpleScalar Cross Compiler & LFU Cache Implementation

A complete and well-structured repository for installing the SimpleScalar toolchain, configuring the cross-compiler, modifying cache replacement policies, and evaluating cache performance using LRU, FIFO, and LFU algorithms using a sample program (matmul.c).

This project helps you understand processor architecture, cache behavior, and performance evaluation techniques used in Advanced Computer Architecture.

---

## ✅ What This Project Includes

- Complete **SimpleScalar installation guide**
- Working **cross-compiler setup (sslittle-na-sstrix)**
- Full **LFU (Least Frequently Used) cache policy implementation**
- Sample C program (`matmul.c`) for benchmark testing
- LRU, FIFO & LFU **cache simulation scripts**
- Organized `results/` folder structure

---

## 📅 Workflow to Run Entire Project

1. Install and patch complete SimpleScalar toolchain  
2. Build cross-compiler  
3. Modify cache policy (LFU)  
4. Rebuild simulator  
5. Compile benchmark program  
6. Run LRU | FIFO | LFU simulations  
7. Compare performance metrics

---

## 📁 Project Structure

Create the directory: Desktop/simplescalar_project

Place the following folders inside:

- f2c-1994.09.27
- gcc-2.7.2.3
- glibc-1.09
- simplesim-3.0
- simpleutils-990811
- ssbig-na-sstrix
- sslittle-na-sstrix

⚠️ Delete the folder: gcc-2.6.3

## 1️⃣ Environment Setup

Open terminal inside:

~/Desktop/simplescalar_project


Run:
```bash
export IDIR=~/Desktop/simplescalar_project
export HOST=i686-pc-linux
export TARGET=sslittle-na-sstrix
cd $IDIR
```

## 2️⃣ Build simpleutils-990811

Fix lexer file: Open file: ld/ldlex.l

In Line 588, replace ```yy_current_buffer``` with ```YY_CURRENT_BUFFER```

Build:

```bash
cd $IDIR/simpleutils-990811
./configure --host=$HOST --target=$TARGET --with-gnu-as --with-gnu-ld --prefix=$IDIR
make CFLAGS=-O
make install
```

## 3️⃣ Build SimpleScalar Simulator
```bash
cd $IDIR/simplesim-3.0
make config-pisa
make
```

## 4️⃣ Patch & Build GCC 2.7.2.3

- replace gcc.c (inside gcc-2.7.2.3)
- replace ar & ranlib (inside sslittle-na-sstrix/bin/)

Inside GCC 2.7.2.3 :
```bash
$IDIR/gcc-2.7.2.3
./configure --host=$HOST --target=$TARGET --with-gnu-as --with-gnu-ld --prefix=$IDIR
chmod -R +w .
```

Required File Fixes
- protoize.c — in line 60, replace ```#include <varargs.h>``` with ```#include <stdarg.h>```
- obstack.h — in line 341, replace ```*((void **)__o->next_free)++ = ((void *)datum);```  with ```*((void **)__o->next_free++) = ((void *)datum);```

Copy required headers & libraries
```
cp ./patched/sys/cdefs.h ../sslittle-na-sstrix/include/sys/cdefs.h
cp ../sslittle-na-sstrix/lib/libc.a ../lib/
cp ../sslittle-na-sstrix/lib/crt0.o ../lib/
```

Run the command:
```bash
make LANGUAGES=c CFLAGS=-O CC="gcc -m64"
```
This should throw an error : ```insn-output.c:xyz: missing terminating "character".```

Fix missing terminating quotes in file ```insn-output.c```
- Add \ at end of lines 675, 750, 823. The lines should look like ```return "FIXME\n\```

Install dependencies
```bash
sudo apt-get update
sudo apt-get install libc6-dev-i386 gcc-multilib g++-multilib
```

Grant permissions:

```bash
chmod +x ~/Desktop/simplescalar_project/sslittle-na-sstrix/bin/ar
```

Build GCC
```bash
make LANGUAGES=c CFLAGS=-O CC="gcc -m64"
make enquire
make LANGUAGES=c CFLAGS=-O CC="gcc -m64" install
```

--- 

## 5️⃣ Implement LFU Cache Policy

Open file ```cache.h```
- After line 105, add ```LFU``` in ```enum cache_policy{}``` function
- After line 124, add ```unsigned int lfu_count; /* count how many times this block was accessed */```

Now open file ```cache.c```
- After line 398, add ```blk->lfu_count = 0;```
- After line 412, add ```case 'u': return LFU;```
- After line 585, Add LFU logic:
```bash
case LFU:
{
    struct cache_blk_t *min_blk = NULL;
    struct cache_blk_t *current;

    unsigned int min_use = (unsigned int) -1;

    for (current = cp->sets[set].way_head; current; current = current->way_next) {
        if (current->lfu_count < min_use) {
            min_use = current->lfu_count;
            min_blk = current;
        }
    }
    repl = min_blk;
}
break;
```
- After line 645, add ```repl->lfu_count = 1;   // first use after load```
- After line 706 and 737, add ```blk->lfu_count++;```

## 6️⃣ Rebuild Simulator With LFU
```bash
cd ~/Desktop/simplescalar_project/simplesim-3.0
make clean
make config-pisa
make
```

## 7️⃣ Create Results Directory
```bash
cd ~/Desktop/simplescalar_project
mkdir results
cd results
mkdir lru fifo lfu
```

## 8️⃣ Sample Program

- Create the file ```matmul.c```:
```bash
#include <stdio.h>
#define SIZE 32
int main() {
    float A[SIZE][SIZE], B[SIZE][SIZE], C[SIZE][SIZE];
    int i, j, k;

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++) {
            A[i][j] = (float)(i + j + 1);
            B[i][j] = (float)(i - j + 2);
            C[i][j] = 0.0;
        }

    for (i = 0; i < SIZE; i++)
        for (j = 0; j < SIZE; j++)
            for (k = 0; k < SIZE; k++)
                C[i][j] += A[i][k] * B[k][j];

    printf("C[0][0] = %f\n", C[0][0]);
    return 0;
}
```

- Compile:
```bash
~/Desktop/simplescalar_project/sslittle-na-sstrix/bin/gcc -O2 matmul.c -o matmul.pisa
```

## 9️⃣ Running Cache Simulations

Go to results:
```bash
cd ~/Desktop/simplescalar_project/results
```
- LRU Commands
```bash
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
```

- FIFO Commands
```bash
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
```

- LFU Commands
```bash
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
```

---

# 💡 *Great architecture comes from understanding the smallest details — and measuring them precisely.*
