/**
 * Computational companion for 2×N floor tiling
 *
 * Counts + enumerates ways to tile a 2×N floor using 1×1 and 2×1 tiles.
 * Verifies recurrence relation: a_N = 3*a_{N-1} + a_{N-2} - a_{N-3}
 * with initial values a_0=1, a_1=2, a_2=7.
 *
 * Compile: g++ -std=c++17 -O2 -o tiling tiling_generator.cpp
 *
 * Subcommands:
 *   ./tiling count <N>       — number of tilings for a 2×N floor
 *   ./tiling enumerate <N>   — print all tilings as ASCII grids
 *   ./tiling verify <N>      — verify recurrence vs DP for N=0..N
 *   ./tiling table <N>       — print a_0 through a_N
 */

#include <iostream>
#include <vector>
#include <string>
#include <cassert>
#include <iomanip>
#include <algorithm>
#include <map>

// Counting via the recurrence relation
// a_N = 3*a_{N-1} + a_{N-2} - a_{N-3}, with a_0=1, a_1=2, a_2=7
long long count_recurrence(int N) {
    if (N < 0) return 0;
    if (N == 0) return 1;
    if (N == 1) return 2;
    if (N == 2) return 7;

    long long a0 = 1, a1 = 2, a2 = 7;
    for (int i = 3; i <= N; i++) {
        long long a3 = 3 * a2 + a1 - a0;
        a0 = a1;
        a1 = a2;
        a2 = a3;
    }
    return a2;
}

// Counting via bitmask DP
//
// Process the grid column by column - "profile" is a bitmask of 2 bits
// representing which cells in the current column are already filled by tiles
// extending from the previous column.
//
// At each column, we try all ways to fill the empty cells using:
//   - 1×1 tiles (fill one cell)
//   - Vertical 2×1 tiles (fill both cells in this column)
//   - Horizontal 2×1 tiles (extend into the next column, setting bits in the
//     next profile)
//
// Profile bits: bit 0 = top row, bit 1 = bottom row.
// ---------------------------------------------------------------------------
long long count_dp(int N) {
    if (N == 0) return 1;

    // dp[profile] = number of ways to fill columns 0..col-1 s.t.
    // column col has the given profile of pre-filled cells.
    std::vector<long long> dp(4, 0);
    dp[0] = 1; // column 0 starts empty

    for (int col = 0; col < N; col++) {
        std::vector<long long> ndp(4, 0);

        for (int mask = 0; mask < 4; mask++) {
            if (dp[mask] == 0) continue;

            bool top_filled = (mask >> 0) & 1;
            bool bot_filled = (mask >> 1) & 1;

            if (top_filled && bot_filled) {
                // Both cells pre-filled; nothing to place, next profile = 0
                ndp[0] += dp[mask];
            } else if (top_filled && !bot_filled) {
                // Only bottom cell is empty
                // Option 1: place 1×1 in bottom
                ndp[0] += dp[mask];
                // Option 2: place horizontal 2×1 in bottom (extends right)
                if (col + 1 < N)
                    ndp[2] += dp[mask]; // bit 1 set in next column
            } else if (!top_filled && bot_filled) {
                // Only top cell is empty
                // Option 1: place 1×1 in top
                ndp[0] += dp[mask];
                // Option 2: place horizontal 2×1 in top (extends right)
                if (col + 1 < N)
                    ndp[1] += dp[mask]; // bit 0 set in next column
            } else {
                // Both cells empty
                // Option 1: vertical 2×1 covering both
                ndp[0] += dp[mask];
                // Option 2: two 1×1 tiles
                ndp[0] += dp[mask];
                // Option 3: 1×1 top + horizontal bottom
                if (col + 1 < N)
                    ndp[2] += dp[mask];
                // Option 4: horizontal top + 1×1 bottom
                if (col + 1 < N)
                    ndp[1] += dp[mask];
                // Option 5: horizontal top + horizontal bottom
                if (col + 1 < N)
                    ndp[3] += dp[mask];
            }
        }

        dp = ndp;
    }

    return dp[0]; 
}

// Enumeration via backtracking
//
// Scan cells left-to-right, top-to-bottom. At each empty cell, try:
//   1. Place a 1×1 tile
//   2. Place a horizontal 2×1 tile (if the cell to the right is empty)
//   3. Place a vertical 2×1 tile (if the cell below is empty)
// and record
struct Enumerator {
    int N;
    // grid[row][col] stores the tile label (A, B, C, ...)
    std::vector<std::vector<char>> grid;
    std::vector<std::vector<std::vector<char>>> results;
    char next_label;

    Enumerator(int n) : N(n), grid(2, std::vector<char>(n, '.')), next_label('A') {}

    // Find the next empty cell scanning left-to-right, top-to-bottom
    bool find_next_empty(int& row, int& col) {
        for (int c = 0; c < N; c++) {
            for (int r = 0; r < 2; r++) {
                if (grid[r][c] == '.') {
                    row = r;
                    col = c;
                    return true;
                }
            }
        }
        return false;
    }

    void solve() {
        int row, col;
        if (!find_next_empty(row, col)) {
            // All cells filled — record this tiling
            results.push_back(grid);
            return;
        }

        char label = next_label++;

        // Option 1: place a 1×1 tile
        grid[row][col] = label;
        solve();
        grid[row][col] = '.';

        // Option 2: place a horizontal 2×1 tile (extends right)
        if (col + 1 < N && grid[row][col + 1] == '.') {
            grid[row][col] = label;
            grid[row][col + 1] = label;
            solve();
            grid[row][col] = '.';
            grid[row][col + 1] = '.';
        }

        // Option 3: place a vertical 2×1 tile (extends down)
        if (row + 1 < 2 && grid[row + 1][col] == '.') {
            grid[row][col] = label;
            grid[row + 1][col] = label;
            solve();
            grid[row][col] = '.';
            grid[row + 1][col] = '.';
        }

        next_label--;
    }

    void enumerate() {
        results.clear();
        next_label = 'A';
        solve();
    }
};

// Print a single tiling as ASCII art with numeric labels and merged cells
void print_tiling(const std::vector<std::vector<char>>& grid, int index) {
    int N = grid[0].size();
    std::cout << "Tiling #" << index << ":\n";

    // Determine tile size for each label: 1 for 1×1, 2 for 2×1
    std::map<char, int> tile_size;
    for (int c = 0; c < N; c++) {
        for (int r = 0; r < 2; r++) {
            char ch = grid[r][c];
            if (tile_size.find(ch) == tile_size.end())
                tile_size[ch] = 1;
            else
                tile_size[ch] = 2;
        }
    }

    // Helper: print a content row with merged cells for horizontal tiles
    auto print_row = [&](int r) {
        std::cout << "|";
        int c = 0;
        while (c < N) {
            std::string s = std::to_string(tile_size[grid[r][c]]);
            if (c + 1 < N && grid[r][c] == grid[r][c + 1]) {
                // Horizontal 2×1 tile: merged cell is 7 chars wide
                int padding = 7 - (int)s.size();
                std::cout << std::string(padding / 2, ' ') << s
                          << std::string(padding - padding / 2, ' ') << "|";
                c += 2;
            } else {
                // Single cell: 3 chars wide
                int padding = 3 - (int)s.size();
                std::cout << std::string(padding / 2, ' ') << s
                          << std::string(padding - padding / 2, ' ') << "|";
                c++;
            }
        }
        std::cout << "\n";
    };

    // Helper: print a border row
    // type: 0=top, 1=middle, 2=bottom
    auto print_border = [&](int type) {
        std::cout << "+";
        for (int c = 0; c < N; c++) {
            // Segment: "---" unless middle border with vertical tile
            if (type == 1 && grid[0][c] == grid[1][c])
                std::cout << "   ";
            else
                std::cout << "---";

            // Junction after this column
            if (c + 1 < N) {
                if (type == 0 && grid[0][c] == grid[0][c + 1])
                    std::cout << "-";  // top row horizontal span
                else if (type == 2 && grid[1][c] == grid[1][c + 1])
                    std::cout << "-";  // bottom row horizontal span
                else
                    std::cout << "+";
            } else {
                std::cout << "+";
            }
        }
        std::cout << "\n";
    };

    print_border(0);  // top border
    print_row(0);     // top row
    print_border(1);  // middle border
    print_row(1);     // bottom row
    print_border(2);  // bottom border
    std::cout << "\n";
}

// CLI
void print_usage(const char* prog) {
    std::cout << "Usage:\n"
              << "  " << prog << " count <N>       Count tilings for a 2×N floor\n"
              << "  " << prog << " enumerate <N>   Print all tilings as ASCII grids\n"
              << "  " << prog << " verify <N>      Verify recurrence vs DP for N=0..N\n"
              << "  " << prog << " lego            Solve the LEGO problem (2×10 floor)\n"
              << "  " << prog << " table <N>       Print a_0 through a_N\n";
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        print_usage(argv[0]);
        return 1;
    }

    std::string cmd = argv[1];

    if (cmd == "count") {
        if (argc < 3) { std::cerr << "Usage: " << argv[0] << " count <N>\n"; return 1; }
        int N = std::stoi(argv[2]);
        long long result = count_dp(N);
        std::cout << "Number of tilings for a 2×" << N << " floor: " << result << "\n";

    } else if (cmd == "enumerate") {
        if (argc < 3) { std::cerr << "Usage: " << argv[0] << " enumerate <N>\n"; return 1; }
        int N = std::stoi(argv[2]);
        if (N > 6) {
            std::cerr << "Warning: N=" << N << " may produce a very large number of tilings ("
                      << count_dp(N) << "). Proceed? [y/N] ";
            char c;
            std::cin >> c;
            if (c != 'y' && c != 'Y') return 0;
        }

        Enumerator en(N);
        en.enumerate();

        std::cout << "All tilings of a 2×" << N << " floor (" << en.results.size() << " total):\n\n";
        for (size_t i = 0; i < en.results.size(); i++) {
            print_tiling(en.results[i], i + 1);
        }

    } else if (cmd == "verify") {
        if (argc < 3) { std::cerr << "Usage: " << argv[0] << " verify <N>\n"; return 1; }
        int N = std::stoi(argv[2]);

        std::cout << "Verifying recurrence vs bitmask DP for N=0.." << N << ":\n\n";
        std::cout << std::setw(5) << "N" << " | "
                  << std::setw(15) << "Recurrence" << " | "
                  << std::setw(15) << "Bitmask DP" << " | "
                  << "Match?\n";
        std::cout << std::string(50, '-') << "\n";

        bool all_ok = true;
        for (int i = 0; i <= N; i++) {
            long long rec = count_recurrence(i);
            long long dp = count_dp(i);
            bool match = (rec == dp);
            if (!match) all_ok = false;

            std::cout << std::setw(5) << i << " | "
                      << std::setw(15) << rec << " | "
                      << std::setw(15) << dp << " | "
                      << (match ? "OK" : "MISMATCH") << "\n";
        }

        std::cout << "\nVerifying against full enumeration for N=0..6:\n\n";
        std::cout << std::setw(5) << "N" << " | "
                  << std::setw(15) << "Enumeration" << " | "
                  << std::setw(15) << "Recurrence" << " | "
                  << "Match?\n";
        std::cout << std::string(50, '-') << "\n";

        for (int i = 0; i <= std::min(N, 6); i++) {
            Enumerator en(i);
            en.enumerate();
            long long en_count = en.results.size();
            long long rec = count_recurrence(i);
            bool match = (en_count == rec);
            if (!match) all_ok = false;

            std::cout << std::setw(5) << i << " | "
                      << std::setw(15) << en_count << " | "
                      << std::setw(15) << rec << " | "
                      << (match ? "OK" : "MISMATCH") << "\n";
        }

        if (all_ok)
            std::cout << "\nAll checks passed!\n";
        else
            std::cout << "\nSome checks FAILED!\n";

    } else if (cmd == "table") {
        if (argc < 3) { std::cerr << "Usage: " << argv[0] << " table <N>\n"; return 1; }
        int N = std::stoi(argv[2]);

        std::cout << "Tiling counts a_0 through a_" << N << ":\n\n";
        std::cout << std::setw(5) << "N" << " | " << std::setw(20) << "a_N" << "\n";
        std::cout << std::string(30, '-') << "\n";

        for (int i = 0; i <= N; i++) {
            std::cout << std::setw(5) << i << " | " << std::setw(20) << count_recurrence(i) << "\n";
        }

    } else {
        print_usage(argv[0]);
        return 1;
    }

    return 0;
}
