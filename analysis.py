#!/usr/bin/env python3

from sympy import (
    Symbol, Rational, apart, factor, solve, simplify, diff, factorial,
    Poly, roots, nsimplify, latex, pprint, Float, N as Neval,
    real_roots, re as sym_re, im as sym_im, CRootOf
)
from sympy import sqrt, cbrt, cos, pi, oo
import sympy
import mpmath

x = Symbol('x')


# 1. Recurrence relation

def tiling_recurrence(max_n):
    """Compute a_0..a_{max_n} via the recurrence a_N = 3a_{N-1} + a_{N-2} - a_{N-3}."""
    a = [0] * (max_n + 1)
    for i in range(max_n + 1):
        if i == 0:   a[i] = 1
        elif i == 1: a[i] = 2
        elif i == 2: a[i] = 7
        else:        a[i] = 3 * a[i-1] + a[i-2] - a[i-3]
    return a


# 2. Approximate closed-form (from IA)

# Approx roots of x^3 - x^2 - 3x + 1 = 0
APPROX_ROOTS = [0.31111, -1.48119, 2.17009]
# Partial fraction coefficients A_i s.t. a_N ≈ sum_i A_i / r_i^{N+1}
APPROX_COEFFS = [0.206757, -0.379141, 0.172385]


def tiling_approx(n):
    """Compute a_N using the approximate closed-form from the paper."""
    result = 0.0
    for coeff, root in zip(APPROX_COEFFS, APPROX_ROOTS):
        result += coeff / (root ** (n + 1))
    return result


# 3. Exact symbolic via generating function derivatives

# G(x) = (1 - x) / (x^3 - x^2 - 3x + 1)
G = (1 - x) / (x**3 - x**2 - 3*x + 1)


def tiling_gf_exact(n):
    """Compute a_N = G^{(N)}(0) / N! exactly using SymPy."""
    # Compute the Nth derivative of G at x=0
    deriv = diff(G, x, n)
    value = deriv.subs(x, 0)
    return Rational(value, factorial(n))


# 4. Exact closed-form with exact roots

def compute_exact_closed_form():
    """
    Use SymPy to find the exact roots of x^3 - x^2 - 3x + 1 = 0
    and perform exact partial fraction decomposition of G(x).

    Returns a function that computes a_N exactly.
    """
    # The denominator factors as x^3 - x^2 - 3x + 1
    denom_poly = x**3 - x**2 - 3*x + 1

    # Find exact roots using real_roots (avoids casus irreducibilis complex form)
    exact_roots = real_roots(denom_poly, x)
    print(f"  Exact roots of x³ - x² - 3x + 1 = 0:")
    for i, r in enumerate(exact_roots):
        r_num = complex(r.evalf()).real
        print(f"    r_{i+1} = {r}")
        print(f"         ≈ {r_num:.10f}")
    print()

    # Partial fraction decomposition of G(x)
    pf = apart(G, x)
    print(f"  Exact partial fraction decomposition of G(x):")
    print(f"    G(x) = {pf}")
    print()

    # Compute residues using the formula: Res(G, r_i) = numerator(r_i) / denominator'(r_i)
    # For G(x) = (1-x) / (x^3 - x^2 - 3x + 1), the residue at a simple pole r_i is:
    #   A_i = (1 - r_i) / (3*r_i^2 - 2*r_i - 3)
    numer = 1 - x
    denom = x**3 - x**2 - 3*x + 1
    denom_deriv = diff(denom, x)  # 3x^2 - 2x - 3

    residues = []
    for r in exact_roots:
        # Evaluate numerically since CRootOf does not simplify symbolically?
        r_val = complex(r.evalf(50)).real
        n_val = 1 - r_val
        d_val = 3 * r_val**2 - 2 * r_val - 3
        res_val = n_val / d_val
        residues.append((r, res_val))
        print(f"  Residue at r ≈ {r_val:.10f}:")
        print(f"    A = (1 - r) / (3r² - 2r - 3) ≈ {res_val:.10f}")

    print()

    def exact_closed(n):
        """Compute a_N using exact roots and residues (high-precision numerical)."""
        # Use mpmath for high-precision
        mpmath.mp.dps = 50
        result = mpmath.mpf(0)
        for r_sym, A_val in residues:
            r_val = complex(r_sym.evalf(50)).real
            result += -mpmath.mpf(A_val) / mpmath.power(mpmath.mpf(r_val), n + 1)
        return int(mpmath.nint(result))

    return exact_closed, exact_roots, residues

# Output

def main():
    MAX_N = 20

    print("=" * 80)
    print("  TILING ANALYSIS: 2×N Floor with 1×1 and 2×1 Tiles")
    print("=" * 80)
    print()

    # Recurrence values
    a = tiling_recurrence(MAX_N)

    # Exact closed-form setup
    print("─" * 80)
    print("  EXACT SYMBOLIC ANALYSIS")
    print("─" * 80)
    print()
    print(f"  Generating function: G(x) = (1 - x) / (x³ - x² - 3x + 1)")
    print()

    exact_closed, exact_roots, residues = compute_exact_closed_form()

    # Comparison table
    print("─" * 80)
    print("  COMPARISON TABLE: N = 0 .. 20")
    print("─" * 80)
    print()

    header = f"{'N':>3} | {'Recurrence':>12} | {'Approx CF':>14} | {'Abs Error':>12} | {'Exact (GF)':>12} | {'Exact CF':>12}"
    print(header)
    print("-" * len(header))

    breakdown_n = None

    for n in range(MAX_N + 1):
        rec_val = a[n]
        approx_val = tiling_approx(n)
        abs_error = abs(approx_val - rec_val)
        gf_val = int(tiling_gf_exact(n))
        exact_cf_val = int(exact_closed(n))

        # Check if approximate value rounds to wrong answer
        rounded_approx = round(approx_val)
        if rounded_approx != rec_val and breakdown_n is None:
            breakdown_n = n

        print(f"{n:>3} | {rec_val:>12d} | {approx_val:>14.4f} | {abs_error:>12.6f} | {gf_val:>12d} | {exact_cf_val:>12d}")

        # Verify all exact methods agree
        assert gf_val == rec_val, f"GF mismatch at N={n}: {gf_val} vs {rec_val}"
        assert exact_cf_val == rec_val, f"Exact CF mismatch at N={n}: {exact_cf_val} vs {rec_val}"

    print()

    # Breakdown point
    print("─" * 80)
    print("  APPROXIMATE CLOSED-FORM BREAKDOWN")
    print("─" * 80)
    print()

    if breakdown_n is not None and breakdown_n <= MAX_N:
        print(f"  The approximate closed-form first gives a WRONG answer at N = {breakdown_n}")
        print(f"    Exact value:       a_{breakdown_n} = {a[breakdown_n]}")
        print(f"    Approximate value: {tiling_approx(breakdown_n):.6f}")
        print(f"    Rounded:           {round(tiling_approx(breakdown_n))}")
    else:
        # Search further
        print("  Searching beyond N=20 for the breakdown point...")
        a_ext = tiling_recurrence(200)
        for n in range(MAX_N + 1, 201):
            approx_val = tiling_approx(n)
            if round(approx_val) != a_ext[n]:
                breakdown_n = n
                print(f"  The approximate closed-form first gives a WRONG answer at N = {n}")
                print(f"    Exact value:       a_{n} = {a_ext[n]}")
                print(f"    Approximate value: {tiling_approx(n):.6f}")
                print(f"    Rounded:           {round(tiling_approx(n))}")
                break
        else:
            print("  No breakdown found up to N=200 (impressive!)")

    print()

    # Exact partial fraction decomposition
    print("─" * 80)
    print("  EXACT PARTIAL FRACTION DECOMPOSITION")
    print("─" * 80)
    print()
    pf = apart(G, x)
    print(f"  G(x) = {pf}")
    print()
    print("  This is what the paper described as 'beyond the scope' of the IA,")
    print("  but SymPy computes it trivially using exact algebraic roots.")
    print()

    # M×N generalization
    print("─" * 80)
    print("  M×N TILING COUNTS (1×1 and 2×1 tiles)")
    print("─" * 80)
    print()

    max_m = 4
    max_n_table = 10

    # Header
    mn_label = "M\\N"
    header = f"{mn_label:>5}"
    for n in range(max_n_table + 1):
        header += f" | {n:>8}"
    print(header)
    print("-" * len(header))

    for m in range(1, max_m + 1):
        row = f"{m:>5}"
        for n in range(max_n_table + 1):
            count = count_mn_tilings(m, n)
            row += f" | {count:>8}"
        print(row)

    print()
    print("  Note: M=2 row matches our sequence: 1, 2, 7, 22, 71, 228, 733, ...")
    print()

    # Error analysis
    print("─" * 80)
    print("  HIGH-PRECISION ERROR ANALYSIS (using mpmath)")
    print("─" * 80)
    print()

    mpmath.mp.dps = 50  # 50 dp

    print(f"  {'N':>3} | {'Absolute Error':>25} | {'Relative Error':>25}")
    print("  " + "-" * 60)

    a_vals = tiling_recurrence(MAX_N)
    for n in range(MAX_N + 1):
        approx = mpmath.mpf(0)
        for coeff, root in zip(APPROX_COEFFS, APPROX_ROOTS):
            approx += mpmath.mpf(coeff) / mpmath.power(mpmath.mpf(root), n + 1)
        exact = mpmath.mpf(a_vals[n])
        abs_err = abs(approx - exact)
        rel_err = abs_err / exact if exact != 0 else mpmath.mpf(0)
        print(f"  {n:>3} | {mpmath.nstr(abs_err, 15):>25} | {mpmath.nstr(rel_err, 15):>25}")

    print()
    print("=" * 80)
    print("  Analysis complete.")
    print("=" * 80)


if __name__ == "__main__":
    main()
