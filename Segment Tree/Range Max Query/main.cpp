/*
 * ============================================================================
 *  SEGMENT TREE  -  POINT UPDATE + RANGE MAXIMUM QUERY  (recursive, root = node 1)
 * ============================================================================
 *
 *  WHAT PROBLEM DOES IT SOLVE?
 *  - An array arr[0..n-1] and two operations, mixed in any order:
 *        query(l, r)      : maximum of arr[l..r]
 *        update(pos, val) : arr[pos] = val
 *  - Plain array: update O(1), but query O(n).  Segment tree: both O(log n).
 *
 *  DIFFERENCE FROM "Range Sum Query":
 *  - The tree shape, the recursion and the 4n size are exactly the same.
 *    Only three things change:
 *        node value       : sum of its segment  ->  MAXIMUM of its segment
 *        combine children : a + b               ->  max(a, b)
 *        no overlap       : return 0            ->  return INT_MIN
 *
 *  HOW THE TREE IS STORED:
 *  - Every node covers a segment [start, end] and stores the maximum of that
 *    segment. The root covers [0, n-1].
 *  - A node with start < end is split at mid = start + (end - start) / 2:
 *        left child  covers [start, mid]       at index 2 * idx
 *        right child covers [mid + 1, end]     at index 2 * idx + 1
 *    A leaf (start == end) holds one element.
 *
 *  PICTURE  (arr = {5, 8, 6, 3, 2, 7},  n = 6,  "idx [start,end] = max"):
 *
 *                            1 [0,5] = 8
 *                     /                    \
 *              2 [0,2] = 8               3 [3,5] = 7
 *              /         \               /          \
 *        4 [0,1] = 8   5 [2,2] = 6  6 [3,4] = 3   7 [5,5] = 7
 *         /      \                   /      \
 *   8 [0,0]=5  9 [1,1]=8      12 [3,3]=3  13 [4,4]=2
 *
 *  ALGORITHM (step by step):
 *  buildTree(start, end, idx):                          called as (0, n-1, 1)
 *    1. Leaf: seg[idx] = arr[start].
 *    2. Otherwise build both children, then
 *       seg[idx] = max(seg[2*idx], seg[2*idx + 1]).
 *
 *  updateTree(start, end, idx, pos, val):
 *    1. Leaf: arr[pos] = val, seg[idx] = val.
 *    2. Otherwise go to the child that contains pos (pos <= mid -> left,
 *       else right), then recompute seg[idx] = max of the two children.
 *
 *  getQuery(start, end, idx, left, right):
 *    1. TOTAL overlap   -> return seg[idx]
 *    2. NO overlap      -> return INT_MIN
 *    3. PARTIAL overlap -> max(answer of left child, answer of right child)
 *
 *  WHY INT_MIN AND NOT 0 ON NO OVERLAP:
 *  - The value returned for an outside node must never WIN the max, i.e.
 *    max(x, value) = x for every x. INT_MIN does that.
 *  - 0 only looks right while every value is positive. For arr = {-5, -8},
 *    query(1, 1) meets [0,0] (outside) -> 0, and max(0, -8) = 0 instead of -8.
 *
 *  DRY RUN  (the picture above):
 *    query(1, 4):
 *      1 [0,5] partial -> ask 2 and 3
 *        2 [0,2] partial -> ask 4 and 5
 *          4 [0,1] partial -> 8 [0,0] outside = INT_MIN, 9 [1,1] inside = 8 -> 8
 *          5 [2,2] inside = 6                                            -> 6
 *          max(8, 6) = 8
 *        3 [3,5] partial -> 6 [3,4] inside = 3, 7 [5,5] outside = INT_MIN -> 3
 *      answer = max(8, 3) = 8           (check: max(8, 6, 3, 2) = 8)
 *
 *    update(1, 1):   (arr[1] was 8, the maximum)
 *      1 [0,5]  mid = 2, pos 1 <= 2 -> left  -> 2 [0,2]
 *      2 [0,2]  mid = 1, pos 1 <= 1 -> left  -> 4 [0,1]
 *      4 [0,1]  mid = 0, pos 1 >  0 -> right -> 9 [1,1]  leaf: 8 -> 1
 *      back up: seg[4] = max(5, 1) = 5, seg[2] = max(5, 6) = 6,
 *               seg[1] = max(6, 7) = 7
 *    query(1, 4) now = max(1, 6, 3) = 6
 *
 *  TIME  : build O(n), update O(log n), query O(log n)
 *  SPACE : O(4n)
 *
 *  REMEMBER:
 *  - Unused slots of seg[] are never read, but starting them at INT_MIN keeps
 *    every value in the tree a valid "maximum".
 *  - long long values -> LLONG_MIN for no overlap.
 *  - Need WHERE the maximum is? Store pairs {value, -index} (or compare by
 *    hand) so that a tie picks the smallest index.
 *  - A max tree can also find "the first position with arr[i] >= x" in
 *    O(log n): from the root, go left if the left child's max >= x, else go
 *    right (CSES Hotel Queries).
 *  - No updates at all? A sparse table answers range maximum in O(1).
 * ============================================================================
 */

#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
private:
    vector<int> seg;    // seg[idx] = maximum of the segment that node idx covers

public:
    SegmentTree (int n) {
        seg.resize(4 * n, INT_MIN);
    }

    // fills seg[] bottom-up: both children first, then this node
    void buildTree(vector<int>& arr, int start, int end, int idx) {
        if (start == end) {
            seg[idx] = arr[start];
            return;
        }

        int mid = start + (end - start) / 2;
        buildTree(arr, start, mid, 2 * idx);
        buildTree(arr, mid + 1, end, 2 * idx + 1);

        seg[idx] = max(seg[2 * idx], seg[2 * idx + 1]);
    }

    // arr[pos] = val, then fixes every node on the path from the leaf back to the root
    void updateTree(vector<int>& arr, int start, int end, int idx, int pos, int val) {
        if (start == end) {
            arr[pos] = val;
            seg[idx] = arr[pos];
            return;
        }

        int mid = start + (end - start) / 2;
        if (pos <= mid) {updateTree(arr, start, mid, 2 * idx, pos, val);}
        else {updateTree(arr, mid + 1, end, 2 * idx + 1, pos, val);}

        seg[idx] = max(seg[2 * idx], seg[2 * idx + 1]);
    }

    // maximum of arr[left..right]; (start, end, idx) is the node we are standing on
    int getQuery(int start, int end, int idx, int left, int right) {
        if (left <= start && right >= end) {return seg[idx];}       // total overlap
        else if (right < start || left > end) {return INT_MIN;}     // no overlap: INT_MIN never wins a max

        int mid = start + (end - start) / 2;                        // partial overlap: ask both children
        return max(getQuery(start, mid, 2 * idx, left, right), getQuery(mid + 1, end, 2 * idx + 1, left, right));
    }

    // prints every node as "idx [start, end] = max", indented by its depth
    void printTree(int start, int end, int idx, int depth) {
        cout << string(4 * depth, ' ') << idx << " [" << start << ", " << end << "] = " << seg[idx] << "\n";
        if (start == end) return;

        int mid = start + (end - start) / 2;
        printTree(start, mid, 2 * idx, depth + 1);
        printTree(mid + 1, end, 2 * idx + 1, depth + 1);
    }
};

int main() {
    vector<int> arr = {5, 8, 6, 3, 2, 7};
    int n = arr.size();

    SegmentTree st(n);
    st.buildTree(arr, 0, n - 1, 1);     // root = node 1, covers [0, n - 1]

    cout << "Tree (idx [start, end] = max):\n";
    st.printTree(0, n - 1, 1, 1);

    cout << "\nmax[1..4] = " << st.getQuery(0, n - 1, 1, 1, 4) << "\n";    // max(8, 6, 3, 2) = 8
    cout << "max[3..5] = " << st.getQuery(0, n - 1, 1, 3, 5) << "\n";      // max(3, 2, 7) = 7
    cout << "max[0..0] = " << st.getQuery(0, n - 1, 1, 0, 0) << "\n";      // 5

    st.updateTree(arr, 0, n - 1, 1, 1, 1);      // arr[1] = 1 (was 8)

    cout << "\nAfter arr[1] = 1:\n";
    cout << "max[1..4] = " << st.getQuery(0, n - 1, 1, 1, 4) << "\n";      // max(1, 6, 3, 2) = 6
    cout << "max[0..1] = " << st.getQuery(0, n - 1, 1, 0, 1) << "\n";      // max(5, 1) = 5
    cout << "max[0..5] = " << st.getQuery(0, n - 1, 1, 0, 5) << "\n";      // 7

    // check every range [l, r] against a plain loop
    bool allMatch = true;
    for (int l = 0; l < n; l++) {
        int brute = INT_MIN;
        for (int r = l; r < n; r++) {
            brute = max(brute, arr[r]);
            if (st.getQuery(0, n - 1, 1, l, r) != brute) {allMatch = false;}
        }
    }
    cout << "\nAll " << n * (n + 1) / 2 << " ranges match a plain loop? " << (allMatch ? "Yes" : "No") << "\n";

    return 0;
}
