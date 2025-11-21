#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>



using namespace std;

class Node {
public:
    int key, data = 0, size = 1, height = 1, bf = 0;
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;

    Node(int key) {
        this->key = key;
    }

    int getHeight() {
        if (this == nullptr) return 0;
        return this->height;
    }

    int getSize() {
        if (this == nullptr) return 0;
        return this->size;
    }
};



class AVL {
public:
    Node* root = nullptr;

    int height() {
        if (root == nullptr) return 0;
        return root->height;
    }

    int size() {
        if (root == nullptr) return 0;
        return root->size;
    }

    Node* search(int key) {
        return searchrec(key, root);
    }

    Node* searchrec(int key, Node* x) {
        if (x == nullptr) return nullptr;
        if (key == x->key) return x;
        else if (key < x->key) return searchrec(key, x->left);
        else return searchrec(key, x->right);
    }

    void insert(Node* x) {
        if (root == nullptr) root = x;
        else insertnode(x, root);
    }

    void insertnode(Node* x, Node* y) {
        if (x->key == y->key) return; // ignore duplicate key
        Node* p = y;
        if (x->key < y->key) {
            if (y->left == nullptr) {
                y->left = x;
                x->parent = y;
            } else {
                insertnode(x, y->left);
            }
        }
        else { // x->key > y->key
            if (y->right == nullptr) {
                y->right = x;
                x->parent = y;
            } else {
                insertnode(x, y->right);
            }
        }
        fixme(p); // fixes p - an ancestor of x
    }

    void fixme(Node* x) {
        //fix x if its balance factor is violated

        recalc(x);
        // Uncomment one of the below two statements
        if ((x->bf <= 1) && (x->bf >= -1)) return; // AVL tree
        // if (true) return; // just BST
        Node* y = tallchild(x);
        Node* z = tallchild(y);
        // 4 cases of rotations
        if ((y == x->left) && (z == y->left)) { rrotate(x); } // Case 1
        else if ((y == x->left) && (z == y->right)) { lrotate(y); rrotate(x); } // Case 2
        else if ((y == x->right) && (z == y->left)) { rrotate(y); lrotate(x); } // Case 3
        else if ((y == x->right) && (z == y->right)) { lrotate(x); } // Case 4
    }

    void Transplant(Node* x, Node* y) {
        // Put node y in place of x in the tree
        // x along with its subtree will be orphaned from the tree
        // Assumes x is not nullptr
        if (x->parent == nullptr) root = y;
        else if (x->parent->left == x) x->parent->left = y;
        else x->parent->right = y;
        if (y != nullptr) y->parent = x->parent;
    }

    void rrotate(Node* x) {
        // Assumes x->left is not nullptr
        Node* y = x->left;
        Transplant(y, y->right);
        Transplant(x, y);
        y->right = x;
        x->parent = y;
        recalc(x);
        recalc(y);
    }

    void lrotate(Node* x) {
        // Assumes x->right is not nullptr
        Node* y = x->right;
        Transplant(y, y->left);
        Transplant(x, y); // Put y in x's place
        y->left = x; // Make x the left child of y
        x->parent = y;
        recalc(x);
        recalc(y);
    }

    Node* select(Node* x, int i) {
        if (x == nullptr) return nullptr;
        int r = (x->left == nullptr) ? 0 : x->left->size;
        if (i == r + 1) return x;
        else if (i < r + 1) return select(x->left, i);
        else return select(x->right, i - (r + 1));
    }
    Node* tallchild(Node* x) {
        if (x == nullptr) return nullptr;
        else if (x->left == nullptr) return x->right;
        else if (x->right == nullptr) return x->left;
        else if (x->left->height > x->right->height) return x->left;
        else return x->right;
    }

    void recalc(Node* x) {
        // Recalculate x's height, size, and bf
        // Assumes its children are correctly recalculated
        if (x == nullptr) return;
        if ((x->left != nullptr) && (x->right != nullptr)) {
            x->height = 1 + max(x->left->height, x->right->height);
            x->size = 1 + x->left->size + x->right->size;
            x->bf = x->left->height - x->right->height;
        }
        else if (x->right != nullptr) {
            x->height = 1 + x->right->height;
            x->size = 1 + x->right->size;
            x->bf = -x->right->height;
        }
        else if (x->left != nullptr) {
            x->height = 1 + x->left->height;
            x->size = 1 + x->left->size;
            x->bf = x->left->height;
        }
        else {
            x->height = 1;
            x->size = 1;
            x->bf = 0;
        }
    }

    void inorder(Node* p) {
        // Helpful for debugging
        if (p == nullptr) return;
        cout << " (";
        inorder(p->left);
        cout << p->key;
        inorder(p->right);
        cout << ") ";
    }
};

int main() {

    AVL b;

    int n = 100000; // Use 100,000 so the benchmark actually registers time
    auto startTime = chrono::high_resolution_clock::now();
    for (int i = 1; i <= n; i++) {
        // Uncomment one of the below two statements
        // Node* p = new Node((i * 7637 + 571) % n + 1); // random inserts
        Node* p = new Node(i); // sequential inserts
        b.insert(p);
    }

    auto endTime = chrono::high_resolution_clock::now();

    cout << "n = " << b.size() << " h = " << b.height();
    cout << " h/log n = " << b.height() / (log(b.size()) / log(2)) << endl;
    cout << "Total Time = " << chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() / 1000.0 << " micsec";
    cout << " Avg time per ins " << chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() / 1000.0 / b.size() << " micsec" << endl;

    // cout << b.select(b.root, n / 2)->key << endl;

    return 0;
}


/* delete is for your pleasure only - not a part of experiments here
  move this part of code into AVL class if you want to see the effects */


/*
void deleteNode(Node* x) {
    // Case 1:  x is a leaf
    if (x->left == nullptr && x->right == nullptr) {
        Transplant(x, nullptr);
        fixup(x->parent);
    }
    // Case 2:  x has one child
    else if (x->left == nullptr) {
        Transplant(x, x->right);
        fixup(x->right);
    }
    else if (x->right == nullptr) {
        Transplant(x, x->left);
        fixup(x->left);
    }
    else {  // Case 3: x has two children
        Node* y = x->right;
        while (y->left != nullptr) y = y->left; // get successor
        if (y->parent == x) { // Subcase 3.1
            Transplant(x, y);
            y->left = x->left;
            x->left->parent = y;
            fixup(y);
        }
        else { // Subcase 3.2
            Transplant(y, y->right);
            Transplant(x, y);
            y->right = x->right; y->left = x->left;
            x->right->parent = x->left->parent = y;
            recalc(y);
            fixup(y->right);
        }
    }
}

void deleteKey(int key) {
    Node* x = search(key);
    if (x != nullptr) deleteNode(x);
}

void fixup(Node* x) {
    // Needed for delete
    // Walks up the path from the point of deletion x and fixes every node
    Node* y = x;
    while (y != nullptr) { fixme(y); y = y->parent; }
}
*/