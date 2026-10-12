/*
 * ============================================================================
 *  SEGMENT TREE  -  POINT UPDATE + RANGE SUM QUERY  (recursive, root = node 1)
 * ============================================================================
 *
 *  WHAT PROBLEM DOES IT SOLVE?
 *  - An array arr[0..n-1] and two operations, mixed in any order:
 *        query(l, r)      : sum of arr[l..r]
 *        update(pos, val) : arr[pos] = val
 *  - Plain array : update O(1), but query O(n).
 *    Prefix sums : query O(1), but every update rebuilds them in O(n).
 *    Segment tree: BOTH in O(log n).
 *
 *  HOW THE TREE IS STORED:
 *  - Every node covers a segment [start, end] of the array and stores the sum
 *    of that segment. The root covers [0, n-1].
 *  - A node with start < end is split at mid = start + (end - start) / 2:
 *        left child  covers [start, mid]
 *        right child covers [mid + 1, end]
 *    A leaf (start == end) holds one element.
 *  - Nodes are kept in seg[] like a binary heap:
 *        root = 1,   children of idx = 2 * idx  and  2 * idx + 1
 *  - (start, end, idx) are always passed TOGETHER and always describe the same
 *    node. The tree does not store the segments; they are recomputed on the way
 *    down from the root.
 *
 *  PICTURE  (arr = {5, 8, 6, 3, 2, 7},  n = 6,  "idx [start,end] = sum"):
 *
 *                            1 [0,5] = 31
 *                     /                    \
 *              2 [0,2] = 19              3 [3,5] = 12
 *              /         \               /          \
 *        4 [0,1] = 13  5 [2,2] = 6  6 [3,4] = 5   7 [5,5] = 7
 *         /      \                   /      \
 *   8 [0,0]=5  9 [1,1]=8      12 [3,3]=3  13 [4,4]=2
 *
 *    Node 13 exists although n = 6 -> seg[] needs more than 2n slots (4n is
 *    always enough, see WHY 4n below). Indexes 10, 11 are simply unused.
 *
 *  ALGORITHM (step by step):
 *  buildTree(start, end, idx):                          called as (0, n-1, 1)
 *    1. Leaf: seg[idx] = arr[start].
 *    2. Otherwise build the left child, build the right child, then
 *       seg[idx] = seg[2*idx] + seg[2*idx + 1].        (children first!)
 *
 *  updateTree(start, end, idx, pos, val):
 *    1. Leaf: it must be pos itself -> arr[pos] = val, seg[idx] = val.
 *    2. Otherwise go to the ONE child whose segment contains pos:
 *           pos <= mid -> left  child  (start,   mid, 2*idx)
 *           pos >  mid -> right child  (mid + 1, end, 2*idx + 1)
 *       then recompute seg[idx] from the two children.
 *    Only the nodes on the path root -> leaf change: O(log n) of them.
 *
 *  getQuery(start, end, idx, left, right):
 *    1. TOTAL overlap  [start,end] inside [left,right]  -> return seg[idx]
 *    2. NO overlap     [start,end] outside [left,right] -> return 0
 *                      (0 = the "do nothing" value of +, see REMEMBER)
 *    3. PARTIAL overlap -> ask both children and add their answers.
 *
 *  DRY RUN  (the picture above):
 *    query(1, 4):
 *      1 [0,5] partial -> ask 2 and 3
 *        2 [0,2] partial -> ask 4 and 5
 *          4 [0,1] partial -> 8 [0,0] outside = 0,  9 [1,1] inside = 8  -> 8
 *          5 [2,2] inside = 6                                          -> 6
 *        3 [3,5] partial -> 6 [3,4] inside = 5,  7 [5,5] outside = 0   -> 5
 *      answer = 8 + 6 + 5 = 19        (check: 8 + 6 + 3 + 2 = 19)
 *
 *    update(2, 10):   (arr[2] was 6)
 *      1 [0,5]  mid = 2, pos 2 <= 2 -> left  -> 2 [0,2]
 *      2 [0,2]  mid = 1, pos 2 >  1 -> right -> 5 [2,2]  leaf: 6 -> 10
 *      back up: seg[2] = 13 + 10 = 23,  seg[1] = 23 + 12 = 35
 *    query(1, 4) now = 8 + 10 + 5 = 23
 *
 *  TIME  : build O(n)     (2n - 1 nodes, O(1) work each)
 *          update O(log n) (one root-to-leaf path)
 *          query O(log n)  (at most 4 nodes visited per level)
 *  SPACE : O(4n)
 *
 *  WHY 4n:
 *  - Every split halves the segment, so the height is h = ceil(log2 n).
 *    Heap indexes on level h are < 2^(h+1), and 2^(h-1) < n, so every index is
 *    < 2^(h+1) < 4n. When n is not a power of two, 2n is NOT enough (n = 6
 *    above uses node 13 >= 12). Proofs in ../README.md.
 *
 *  REMEMBER:
 *  - Any ASSOCIATIVE operation works: change the "+" (build, update, query)
 *    and the "no overlap" value to the operation's identity:
 *        sum -> 0,  min -> INT_MAX,  max -> INT_MIN,  xor -> 0,  gcd -> 0
 *    Min and max are done that way in "Range Min Query" / "Range Max Query".
 *  - Sums of many ints overflow int -> use long long for big values.
 *  - Updating a RANGE (add x to arr[l..r]) one point at a time is O(n log n);
 *    that needs LAZY PROPAGATION.
 *  - No updates at all? Prefix sums answer a sum query in O(1) and are simpler.
 *
 *  FIXES MADE:
 *  - seg.resize(2 * n) -> 4 * n. With n = 6 the build writes node 13, past the
 *    end of a 12-slot vector (undefined behaviour: garbage or a crash).
 *  - updateTree went into the wrong child. The test was "mid <= pos" and BOTH
 *    branches recursed with the left range [start, mid]:
 *      - pos == mid went right instead of left
 *      - the right child (2*idx + 1) was given the left child's range
 *    Example: arr = {5, 8, 6, 3}, update(0, 10) wrote 10 into the leaf of
 *    arr[1]; query(0, 0) still gave 5 and query(0, 3) gave 24 instead of 27.
 *    Now pos <= mid -> (start, mid, 2*idx), else (mid + 1, end, 2*idx + 1).
 *
 *  COMPLETED: main() was empty. Added a demo on the example above and a
 *             printTree() helper that prints every node, to compare with the
 *             picture.
 * ============================================================================
 */

#include <bits/stdc++.h>
using namespace std;

class SegmentTree {
private:
    vector<int> seg;    // seg[idx] = sum of the segment that node idx covers

public:
    SegmentTree (int n) {
        seg.resize(4 * n, 0);   // FIX: was 2 * n, too small when n is not a power of two
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

        seg[idx] = seg[2 * idx] + seg[2 * idx + 1];
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

        seg[idx] = seg[2 * idx] + seg[2 * idx + 1];
    }

    // sum of arr[left..right]; (start, end, idx) is the node we are standing on
    int getQuery(int start, int end, int idx, int left, int right) {
        if (left <= start && right >= end) {return seg[idx];}       // total overlap
        else if (right < start || left > end) {return 0;}           // no overlap: 0 changes no sum

        int mid = start + (end - start) / 2;                        // partial overlap: ask both children
        return getQuery(start, mid, 2 * idx, left, right) + getQuery(mid + 1, end, 2 * idx + 1, left, right);
    }

    // prints every node as "idx [start, end] = sum", indented by its depth
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

    cout << "Tree (idx [start, end] = sum):\n";
    st.printTree(0, n - 1, 1, 1);

    cout << "\nsum[1..4] = " << st.getQuery(0, n - 1, 1, 1, 4) << "\n";    // 8 + 6 + 3 + 2 = 19
    cout << "sum[0..5] = " << st.getQuery(0, n - 1, 1, 0, 5) << "\n";      // 31
    cout << "sum[3..3] = " << st.getQuery(0, n - 1, 1, 3, 3) << "\n";      // 3

    st.updateTree(arr, 0, n - 1, 1, 2, 10);     // arr[2] = 10 (was 6)

    cout << "\nAfter arr[2] = 10:\n";
    cout << "sum[1..4] = " << st.getQuery(0, n - 1, 1, 1, 4) << "\n";      // 8 + 10 + 3 + 2 = 23
    cout << "sum[0..2] = " << st.getQuery(0, n - 1, 1, 0, 2) << "\n";      // 5 + 8 + 10 = 23
    cout << "sum[0..5] = " << st.getQuery(0, n - 1, 1, 0, 5) << "\n";      // 35

    // check every range [l, r] against a plain loop
    bool allMatch = true;
    for (int l = 0; l < n; l++) {
        int brute = 0;
        for (int r = l; r < n; r++) {
            brute += arr[r];
            if (st.getQuery(0, n - 1, 1, l, r) != brute) {allMatch = false;}
        }
    }
    cout << "\nAll " << n * (n + 1) / 2 << " ranges match a plain loop? " << (allMatch ? "Yes" : "No") << "\n";

    return 0;
}
