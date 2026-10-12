# Range Queries — Revision Index

Every `main.cpp` starts with a block comment covering the idea, the step-by-step algorithm, a dry run on the example in that file, complexity, and common pitfalls. Read the comment, predict the output, then run it.

**Theory:** every topic also has a `README.md` next to its code that explains *how* it works, *why* it works (with proofs), its complexity, common mistakes, and links to the matching [cp-algorithms](https://cp-algorithms.com/) page and practice problems:

| Folder | READMEs |
|---|---|
| Segment Tree | [Segment Tree](<Segment Tree/README.md>) |

```
Segment Tree                 point update + range query   (recursive, root = node 1, array of size 4n)
   ├── Range Sum Query       a + b,      no overlap -> 0
   ├── Range Min Query       min(a, b),  no overlap -> INT_MAX
   └── Range Max Query       max(a, b),  no overlap -> INT_MIN
```

## Key idea of each folder

| Folder | Key idea in one line |
|--------|----------------------|
| `Segment Tree` | Node `idx` covers `[start, end]` and stores its sum / min / max; children `[start, mid]` and `[mid + 1, end]` live at `2·idx` and `2·idx + 1`. **Query:** inside `[l, r]` → take the node, outside → the identity (`0` / `INT_MAX` / `INT_MIN`), partial → combine both children. **Update:** go down to the leaf (`pos <= mid` → left), then recompute the path back up. O(log n) each; `seg[]` needs **4n** slots. Any associative operation works (xor, gcd, …) the same way. |

## Cheat sheet: which structure when?

| Problem | Use | Time |
|---|---|---|
| Range sum, array never changes | Prefix sums | O(n) build, O(1) per query |
| Range min / max / gcd, array never changes | Sparse table | O(n log n) build, O(1) per query |
| Range sum + point updates | [Segment tree — Range Sum Query](<Segment Tree/Range Sum Query/main.cpp>) (or Fenwick tree) | O(n) build, O(log n) per operation |
| Range min + point updates | [Segment tree — Range Min Query](<Segment Tree/Range Min Query/main.cpp>) | O(n) build, O(log n) per operation |
| Range max + point updates | [Segment tree — Range Max Query](<Segment Tree/Range Max Query/main.cpp>) | O(n) build, O(log n) per operation |
| Range gcd / xor / … + point updates | Segment tree with that operation and its identity ([how](<Segment Tree/README.md#changing-the-operation>)) | O(n) build, O(log n) per operation |
| Add to / assign a whole range, then range queries | Segment tree with lazy propagation | O(log n) per operation |

## Building and running

```bash
g++ -std=c++17 -o main main.cpp
```

No compiled files are kept in the folders. Build each `main.cpp` when you want to run it.
