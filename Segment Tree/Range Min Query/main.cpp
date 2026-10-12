/*
 * ============================================================================
 *  SEGMENT TREE  -  POINT UPDATE + RANGE MINIMUM QUERY  (recursive, root = node 1)
 * ============================================================================
 *
 *  WHAT PROBLEM DOES IT SOLVE?
 *  - An array arr[0..n-1] and two operations, mixed in any order:
 *        query(l, r)      : minimum of arr[l..r]
 *        update(pos, val) : arr[pos] = val
 *  - Plain array: update O(1), but query O(n).  Segment tree: both O(log n).
 *
 *  DIFFERENCE FROM "Range Sum Query":
 *  - The tree shape, the recursion and the 4n size are exactly the same.
 *    Only three things change:
 *        node value       : sum of its segment  ->  MINIMUM of its segment
 *        combine children : a + b               ->  min(a, b)
 *        no overlap       : return 0            ->  return INT_MAX
 *
 *  HOW THE TREE IS STORED:
 *  - Every node covers a segment [start, end] and stores the minimum of that
 *    segment. The root covers [0, n-1].
 *  - A node with start < end is split at mid = start + (end - start) / 2:
 *        left child  covers [start, mid]       at index 2 * idx
 *        right child covers [mid + 1, end]     at index 2 * idx + 1
 *    A leaf (start == end) holds one element.
 *
 *  PICTURE  (arr = {5, 8, 6, 3, 2, 7},  n = 6,  "idx [start,end] = min"):
 *
 *                            1 [0,5] = 2
 *                     /                    \
 *              2 [0,2] = 5               3 [3,5] = 2
 *              /         \               /          \
 *        4 [0,1] = 5   5 [2,2] = 6  6 [3,4] = 2   7 [5,5] = 7
 *         /      \                   /      \
 *   8 [0,0]=5  9 [1,1]=8      12 [3,3]=3  13 [4,4]=2
 *
 *  ALGORITHM (step by step):
 *  buildTree(start, end, idx):                          called as (0, n-1, 1)
 *    1. Leaf: seg[idx] = arr[start].
 *    2. Otherwise build both children, then
 *       seg[idx] = min(seg[2*idx], seg[2*idx + 1]).
 *
 *  updateTree(start, end, idx, pos, val):
 *    1. Leaf: arr[pos] = val, seg[idx] = val.
 *    2. Otherwise go to the child that contains pos (pos <= mid -> left,
 *       else right), then recompute seg[idx] = min of the two children.
 *
 *  getQuery(start, end, idx, left, right):
 *    1. TOTAL overlap   -> return seg[idx]
 *    2. NO overlap      -> return INT_MAX
 *    3. PARTIAL overlap -> min(answer of left child, answer of right child)
 *
 *  WHY INT_MAX AND NOT 0 ON NO OVERLAP:
 *  - The value returned for an outside node must never WIN the min, i.e.
 *    min(x, value) = x for every x. INT_MAX does that.
 *  - With 0, query(1, 4) below would meet 8 [0,0] (outside) -> 0, and the
 *    answer would be 0 instead of 2.
 *
 *  DRY RUN  (the picture above):
 *    query(1, 4):
 *      1 [0,5] partial -> ask 2 and 3
 *        2 [0,2] partial -> ask 4 and 5
 *          4 [0,1] partial -> 8 [0,0] outside = INT_MAX, 9 [1,1] inside = 8 -> 8
 *          5 [2,2] inside = 6                                            -> 6
 *          min(8, 6) = 6
 *        3 [3,5] partial -> 6 [3,4] inside = 2, 7 [5,5] outside = INT_MAX -> 2
 *      answer = min(6, 2) = 2           (check: min(8, 6, 3, 2) = 2)
 *
 *    update(4, 9):   (arr[4] was 2, the minimum)
 *      1 [0,5]  mid = 2, pos 4 >  2 -> right -> 3 [3,5]
 *      3 [3,5]  mid = 4, pos 4 <= 4 -> left  -> 6 [3,4]
 *      6 [3,4]  mid = 3, pos 4 >  3 -> right -> 13 [4,4]  leaf: 2 -> 9
 *      back up: seg[6] = min(3, 9) = 3, seg[3] = min(3, 7) = 3,
 *               seg[1] = min(5, 3) = 3
 *    query(1, 4) now = min(8, 6, 3) = 3
 *
 *  TIME  : build O(n), update O(log n), query O(log n)
 *  SPACE : O(4n)
 *
 *  REMEMBER:
 *  - Unused slots of seg[] are never read, but starting them at INT_MAX keeps
 *    every value in the tree a valid "minimum".
 *  - long long values -> LLONG_MAX for no overlap.
 *  - Need WHERE the minimum is? Store pairs {value, index}: min() on pairs
 *    also picks the smallest index on a tie.
 *  - A Fenwick tree cannot do range minimum (it needs to "subtract" a prefix,
 *    and min has no inverse). A segment tree can.
 *  - No updates at all? A sparse table answers range minimum in O(1).
 * ============================================================================
 */

#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
private:
    vector<int> seg;    // seg[idx] = minimum of the segment that node idx covers

public:
    SegmentTree (int n) {
        seg.resize(4 * n, INT_MAX);
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

        seg[idx] = min(seg[2 * idx], seg[2 * idx + 1]);
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

        seg[idx] = min(seg[2 * idx], seg[2 * idx + 1]);
    }

    // minimum of arr[left..right]; (start, end, idx) is the node we are standing on
    int getQuery(int start, int end, int idx, int left, int right) {
        if (left <= start && right >= end) {return seg[idx];}       // total overlap
        else if (right < start || left > end) {return INT_MAX;}     // no overlap: INT_MAX never wins a min

        int mid = start + (end - start) / 2;                        // partial overlap: ask both children
        return min(getQuery(start, mid, 2 * idx, left, right), getQuery(mid + 1, end, 2 * idx + 1, left, right));
    }

    // prints every node as "idx [start, end] = min", indented by its depth
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

    cout << "Tree (idx [start, end] = min):\n";
    st.printTree(0, n - 1, 1, 1);

    cout << "\nmin[1..4] = " << st.getQuery(0, n - 1, 1, 1, 4) << "\n";    // min(8, 6, 3, 2) = 2
    cout << "min[0..2] = " << st.getQuery(0, n - 1, 1, 0, 2) << "\n";      // min(5, 8, 6) = 5
    cout << "min[5..5] = " << st.getQuery(0, n - 1, 1, 5, 5) << "\n";      // 7

    st.updateTree(arr, 0, n - 1, 1, 4, 9);      // arr[4] = 9 (was 2)

    cout << "\nAfter arr[4] = 9:\n";
    cout << "min[1..4] = " << st.getQuery(0, n - 1, 1, 1, 4) << "\n";      // min(8, 6, 3, 9) = 3
    cout << "min[4..5] = " << st.getQuery(0, n - 1, 1, 4, 5) << "\n";      // min(9, 7) = 7
    cout << "min[0..5] = " << st.getQuery(0, n - 1, 1, 0, 5) << "\n";      // 3

    // check every range [l, r] against a plain loop
    bool allMatch = true;
    for (int l = 0; l < n; l++) {
        int brute = INT_MAX;
        for (int r = l; r < n; r++) {
            brute = min(brute, arr[r]);
            if (st.getQuery(0, n - 1, 1, l, r) != brute) {allMatch = false;}
        }
    }
    cout << "\nAll " << n * (n + 1) / 2 << " ranges match a plain loop? " << (allMatch ? "Yes" : "No") << "\n";

    return 0;
}
