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
- Prints a comparison table for N=0..15
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

**Output:**
```
================================================================================
  TILING ANALYSIS: 2×N Floor with 1×1 and 2×1 Tiles
================================================================================

────────────────────────────────────────────────────────────────────────────────
  EXACT SYMBOLIC ANALYSIS
────────────────────────────────────────────────────────────────────────────────

  Generating function: G(x) = (1 - x) / (x³ - x² - 3x + 1)

  Exact roots of x³ - x² - 3x + 1 = 0:
    r_1 = CRootOf(x**3 - x**2 - 3*x + 1, 0)
         ≈ -1.4811943041
    r_2 = CRootOf(x**3 - x**2 - 3*x + 1, 1)
         ≈ 0.3111078175
    r_3 = CRootOf(x**3 - x**2 - 3*x + 1, 2)
         ≈ 2.1700864866

  Exact partial fraction decomposition of G(x):
    G(x) = -(x - 1)/(x**3 - x**2 - 3*x + 1)

  Residue at r ≈ -1.4811943041:
    A = (1 - r) / (3r² - 2r - 3) ≈ 0.3791441193
  Residue at r ≈ 0.3111078175:
    A = (1 - r) / (3r² - 2r - 3) ≈ -0.2067595751
  Residue at r ≈ 2.1700864866:
    A = (1 - r) / (3r² - 2r - 3) ≈ -0.1723845442

────────────────────────────────────────────────────────────────────────────────
  COMPARISON TABLE: N = 0 .. 15
────────────────────────────────────────────────────────────────────────────────

  N |   Recurrence |      Approx CF |    Abs Error |   Exact (GF) |     Exact CF
--------------------------------------------------------------------------------
  0 |            1 |         1.0000 |     0.000014 |            1 |            1
  1 |            2 |         1.9999 |     0.000056 |            2 |            2
  2 |            7 |         6.9998 |     0.000230 |            7 |            7
  3 |           22 |        21.9991 |     0.000895 |           22 |           22
  4 |           71 |        70.9966 |     0.003372 |           71 |           71
  5 |          228 |       227.9876 |     0.012439 |          228 |          228
  6 |          733 |       732.9549 |     0.045122 |          733 |          733
  7 |         2356 |      2355.8384 |     0.161565 |         2356 |         2356
  8 |         7573 |      7572.4276 |     0.572442 |         7573 |         7573
  9 |        24342 |     24339.9892 |     2.010767 |        24342 |        24342
 10 |        78243 |     78235.9879 |     7.012103 |        78243 |        78243
 11 |       251498 |    251473.6967 |    24.303320 |       251498 |       251498
 12 |       808395 |    808311.2108 |    83.789237 |       808395 |       808395
 13 |      2598440 |   2598152.4476 |   287.552379 |      2598440 |      2598440
 14 |      8352217 |   8351234.1278 |   982.872221 |      8352217 |      8352217
 15 |     26846696 |  26843348.4186 |  3347.581397 |     26846696 |     26846696

  ⚠ Approximate closed-form breaks down at N=8
    (rounded approximation gives 7572, correct value is 7573)

────────────────────────────────────────────────────────────────────────────────
  EXACT PARTIAL FRACTION DECOMPOSITION
────────────────────────────────────────────────────────────────────────────────

  G(x) = -(x - 1)/(x**3 - x**2 - 3*x + 1)

────────────────────────────────────────────────────────────────────────────────
  HIGH-PRECISION ERROR ANALYSIS (using mpmath)
────────────────────────────────────────────────────────────────────────────────

    N |            Absolute Error |            Relative Error
  ------------------------------------------------------------
    0 |       1.42203216124357e-5 |       1.42203216124357e-5
    1 |         5.618217797329e-5 |        2.8091088986645e-5
    2 |      0.000230008700794978 |        3.2858385827854e-5
    3 |      0.000894512387105691 |       4.06596539593496e-5
    4 |       0.00337164651580072 |       4.74879790957848e-5
    5 |        0.0124385622814959 |       5.45550977258594e-5
    6 |        0.0451218977426705 |       6.15578413951849e-5
    7 |         0.161564550952951 |       6.85757856336803e-5
    8 |         0.572442371479491 |       7.55899077617181e-5
    9 |          2.01076749521817 |       8.26048597164642e-5
   10 |          7.01210279294878 |       8.96195543748166e-5
   11 |          24.3033200093138 |       9.66342476254835e-5
   12 |          83.7892368613728 |      0.000103648880635547
   13 |           287.55237903665 |      0.000110663466940414
   14 |          982.872221084302 |       0.00011767800346714
   15 |           3347.5813970131 |      0.000124692490912591

================================================================================
  Analysis complete.
================================================================================
```

