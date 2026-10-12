# Segment Tree

**Problem:** an array `arr[0..n-1]` and two operations in any order —
`query(l, r)`: the sum / minimum / maximum of `arr[l..r]`, and `update(pos, val)`: `arr[pos] = val`. Both in O(log n).

| Program | Shows |
|---|---|
| [Range Sum Query](<Range Sum Query/main.cpp>) | recursive sum tree (root = node 1): `buildTree`, point `updateTree`, range `getQuery`, `printTree`; the two bugs fixed in the original version (`FIXES MADE`) |
| [Range Min Query](<Range Min Query/main.cpp>) | the same tree with `min(a, b)`; no overlap returns `INT_MAX` (why not `0`) |
| [Range Max Query](<Range Max Query/main.cpp>) | the same tree with `max(a, b)`; no overlap returns `INT_MIN` (why not `0`) |

All three use the same array `{5, 8, 6, 3, 2, 7}`, so the three trees can be compared node by node. Only three things differ between them: the value of a node, how two children are combined, and what an outside node returns (see [Changing the operation](#changing-the-operation)).

---

## How it works

Every node covers a segment `[start, end]` of the array and stores the **sum of that segment** (the minimum / maximum in the other two programs; the sum is used below). The root covers `[0, n−1]`. A node with `start < end` is split at `mid = start + (end − start) / 2` into a left child `[start, mid]` and a right child `[mid + 1, end]`; a leaf (`start == end`) holds one element.

The nodes live in `seg[]` like a binary heap: the root is `1`, the children of `idx` are `2·idx` and `2·idx + 1`. A node's segment is **not stored** — it is passed down together with `idx`, so `(start, end, idx)` always travel as one unit.

```
arr = {5, 8, 6, 3, 2, 7}          "idx [start,end] = sum"

                            1 [0,5] = 31
                     /                    \
              2 [0,2] = 19              3 [3,5] = 12
              /         \               /          \
        4 [0,1] = 13  5 [2,2] = 6  6 [3,4] = 5   7 [5,5] = 7
         /      \                   /      \
   8 [0,0]=5  9 [1,1]=8      12 [3,3]=3  13 [4,4]=2
```

- **build:** build both children, then `seg[idx] = seg[2·idx] + seg[2·idx + 1]`.
- **point update:** walk from the root down to the leaf of `pos` (left if `pos ≤ mid`, right otherwise), set the leaf, then recompute every node on the way back up.
- **range query** at node `[start, end]`:
  - **total overlap** — `[start, end]` lies inside `[l, r]` → return `seg[idx]`;
  - **no overlap** — the two segments are disjoint → return `0`;
  - **partial overlap** → query both children and add.

Example: `query(1, 4)` takes `9 [1,1]`, `5 [2,2]`, `6 [3,4]` → `8 + 6 + 5 = 19`. A full dry run of a query and an update is in the comment at the top of each `main.cpp` — for the min and max trees too.

## Why it works

**Invariant.** For every node, `seg[idx] = arr[start] + … + arr[end]`.

- *Build.* A leaf gets `arr[start]`. An internal node gets the sum of its two children, whose segments split its own segment exactly in two. By induction from the leaves up, the invariant holds after build.
- *Update.* Changing `arr[pos]` changes the sum of exactly those nodes whose segment **contains** `pos`. On each level the segments are disjoint, so there is exactly one such node per level, and together they form the path from the root to the leaf `[pos, pos]` — the path the recursion walks. Each of them is recomputed **after** its child, so the invariant holds again.
- *Query.* Claim: `getQuery` at a node returns the sum of `arr` over `[start, end] ∩ [l, r]`.
  - Total overlap: the intersection is the whole node → `seg[idx]` by the invariant.
  - No overlap: the intersection is empty → `0`.
  - Partial overlap: the intersection splits into its part inside the left child and its part inside the right child. The children are smaller, so by induction the two calls return exactly those two sums.

  At the root `[0, n−1] ∩ [l, r] = [l, r]`, so the answer is the sum of `arr[l..r]`. ∎

**Min and max.** Read "sum" as "minimum" (or "maximum") everywhere above and the proof still holds. It only uses two facts: the answer for a segment can be built from the answers for its two halves (`min(A ∪ B) = min(min A, min B)`), and an outside node returns a value that changes nothing (`min(x, INT_MAX) = x`, `max(x, INT_MIN) = x`).

**A query visits O(log n) nodes.** A node only recurses on a *partial* overlap: it meets `[l, r]` but is not inside it, so it sticks out on the left (`start < l ≤ end`) or on the right (`start ≤ r < end`). The nodes of one level have disjoint segments, so at most one of them contains both `l − 1` and `l`, and at most one contains both `r` and `r + 1`. → at most **2** partial nodes per level → at most **4** nodes visited on the next level. With height `h` that is at most `4h + 1` nodes in total. In other words: every range splits into at most 2 tree nodes per level.

**Update and build.** An update visits one node per level: `h + 1` nodes. Build visits every node once; a tree in which every internal node has exactly 2 children and there are `n` leaves has `n − 1` internal nodes, so `2n − 1` nodes and O(n) time.

**Height.** A segment of length `L` splits into `⌈L/2⌉` and `⌊L/2⌋`. Since `⌈⌈x⌉ / 2⌉ = ⌈x / 2⌉`, a node at depth `d` covers at most `⌈n / 2^d⌉` elements. At `d = ⌈log₂ n⌉` that is 1, so every node there is a leaf: `h ≤ ⌈log₂ n⌉`.

**Why the array needs 4n slots.** Heap indexes at depth `d` lie in `[2^d, 2^(d+1) − 1]`, so every index is `< 2^(h+1)`. For `n ≥ 2`, `h = ⌈log₂ n⌉` means `2^(h−1) < n`, hence `2^(h+1) = 4 · 2^(h−1) < 4n`. ∎ (`n = 1` uses only index 1.)

- If `n` is a power of two, the tree is perfect and the largest index is exactly `2n − 1`, so `2n` slots would do.
- Otherwise `2n` can be **too small**: `n = 6` already uses index 13 (≥ 12) — see `13 [4,4]` in the picture. The exact maximum jumps around irregularly with `n`, which is why everyone simply allocates `4n`.

## Changing the operation

The structure only ever **combines two neighbouring answers**. To answer a different query, change the `+` in three places (build, update, query) and the value returned on no overlap. That is the only difference between the three programs in this folder:

| Query | `combine(a, b)` | no-overlap value (identity) |
|---|---|---|
| sum — [Range Sum Query](<Range Sum Query/main.cpp>) | `a + b` | `0` |
| minimum — [Range Min Query](<Range Min Query/main.cpp>) | `min(a, b)` | `INT_MAX` |
| maximum — [Range Max Query](<Range Max Query/main.cpp>) | `max(a, b)` | `INT_MIN` |
| xor | `a ^ b` | `0` |
| gcd | `gcd(a, b)` | `0` (since `gcd(0, x) = x`) |
| product mod p | `a * b % p` | `1` |

- **Associativity is required.** A query splits `[l, r]` into pieces and combines them in a grouping decided by the node boundaries, not by you. That gives the right answer only if `(a ∘ b) ∘ c = a ∘ (b ∘ c)`.
- **Commutativity is not required**, as long as the left piece is always combined before the right one (the code does `left + right`). E.g. composing functions or multiplying matrices works.
- **The identity** is only needed for the "no overlap" case, and it must not change the answer: `combine(x, id) = x`. Returning `0` for a minimum query is a classic bug.
- **When the answer does not combine directly, store more per node.** Average → store (sum, count). Count of the minimum → store (min, count). Maximum subarray sum → store (sum, best prefix, best suffix, best). Some answers, such as "the most frequent value", cannot be built from two children's answers at all — they need different techniques.

## When a segment tree is overkill

- **No updates, sum only:** prefix sums, O(1) per query.
- **No updates, min / max / gcd:** sparse table, O(1) per query (these operations are idempotent, so two overlapping blocks can cover `[l, r]`).
- **Point updates, sum:** a Fenwick tree (BIT) is shorter and faster. It answers `prefix(r) − prefix(l − 1)`, which needs an **inverse** (subtraction) — so it cannot do min / max directly, while a segment tree can.
- **Range updates** (add `x` to all of `arr[l..r]`): a segment tree with **lazy propagation**. Updating each point separately costs O((r − l + 1) log n).

## Complexity

| Operation | Time |
|---|---|
| build | O(n) |
| point update | O(log n) |
| range query | O(log n) |

Memory: `4n` values.

## Common mistakes

- `seg` of size `2n` with this recursive layout — it needs `4n` (one of the bugs fixed in `Range Sum Query/main.cpp`).
- Going to the wrong child. The split is `[start, mid]` / `[mid + 1, end]`, so `pos == mid` belongs to the **left** child: the test is `pos <= mid`, not `mid <= pos` (the other bug fixed in `Range Sum Query/main.cpp`).
- Passing a child's index with the wrong range. `(start, end, idx)` must always describe the **same** node: `(start, mid, 2·idx)` and `(mid + 1, end, 2·idx + 1)`.
- Copying the sum tree and forgetting the no-overlap value: `0` breaks a min query on positive values and a max query on negative values (examples in the min / max headers).
- Forgetting `seg[idx] = seg[2·idx] + seg[2·idx + 1]` after the recursive update — the leaf changes but the sums above it do not.
- `int` overflow: `2·10⁵` values of up to `10⁹` sum to `2·10¹⁴` → use `long long`.
- Problems usually give 1-indexed `l, r`; convert to `l − 1, r − 1`, or build the tree over `[1, n]`.
- Calling the functions with anything other than the root: always `(0, n − 1, 1)`.

## Related

- cp-algorithms: [Segment Tree](https://cp-algorithms.com/data_structures/segment_tree.html)
- Practice:
  - sum: CSES *Dynamic Range Sum Queries* (`long long`), *List Removals* (find the k-th remaining element); LeetCode 307 *Range Sum Query - Mutable*
  - min: CSES *Dynamic Range Minimum Queries*, *Static Range Minimum Queries* (a sparse table also works)
  - max: CSES *Hotel Queries* (walk down the max tree to the first value ≥ x)
  - other operations: CSES *Range Xor Queries*; Codeforces EDU *Segment Tree, part 1*
