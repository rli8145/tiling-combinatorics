A computational companion to my [IB Mathematics AA HL Internal Assessment](MATHIA.pdf), which was written in my senior year of high school (the version attached is revised and slightly shortened). The paper derives a recurrence relation, generating function, and approximate closed-form for the number of ways to tile a 2×N floor using 1×1 and 2×1 tiles. 

The paper is written to be accessible to a wide audience, and this repo further enhances understanding and verifies mathematical correctness through the `tiling_generator.cpp` CLI and the `analysis.py` script.

## Summary of Paper

**Problem:** How many ways can you tile a 2×N floor using 1×1 and 2×1 tiles?

**Recurrence:** $a_N = 3·a_{N-1} + a_{N-2} − a_{N-3}$ with $a_0 = 1, a_1 = 2, a_2 = 7$

**Generating function:** $G(x) = (1 − x) / (x³ − x² − 3x + 1)$

**Sequence:** 1, 2, 7, 22, 71, 228, 733, 2356, 7573, 24342, 78243, ... 

## Components

### 1. C++ Tiling Enumerator (`tiling_generator.cpp`)

Counts and visualizes all tilings using bitmask DP and backtracking - 

**Compile:**
```bash
g++ -std=c++17 -O2 -o tiling tiling_generator.cpp
```

**Usage:**
```bash
./tiling count 10        # Count tilings for a 2×10 floor
./tiling enumerate 3     # Print all tilings of a 2×3 floor as ASCII art
./tiling verify 20       # Verify recurrence vs DP for N=0..20
./tiling table 15        # Print a_0 through a_15
```

**Sample output (`./tiling enumerate 2`):**
```
All tilings of a 2×2 floor (7 total):

Tiling #1:
+---+---+
| 1 | 1 |
+---+---+
| 1 | 1 |
+---+---+

Tiling #2:
+---+---+
| 1 | 2 |
+---+   +
| 1 | 2 |
+---+---+

...
```

`2` represents a square occupied by a 2x1 tile, `1` represents a square occupied by a 1x1 tile

### 2. Python Analysis Script (`analysis.py`)

Heavily relies on SymPy and mpmath, which allows for symbolic analysis and arbitrary-precision arithmetic beyond C++.

**Features:**
- Computes a_N four ways (recurrence, approximate closed-form, exact GF derivatives, exact closed-form with symbolic roots)
- Prints a comparison table for N=0..20
- Finds the breakdown point where the approximate closed-form gives a wrong answer
- Computes the **exact** partial fraction decomposition
- Error analysis using mpmath

**Install dependencies:**
```bash
pip install -r requirements.txt
```

**Run:**
```bash
python3 analysis.py
```

