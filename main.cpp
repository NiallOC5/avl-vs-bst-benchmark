#include <iostream>
#include <cmath>
#include <chrono>
#include <thread>
#include <algorithm> // Added for max()

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
        // Recalculate height and balance factor
        recalc(x);

        // MODE SWITCH: Uncomment "return" below to disable AVL balancing (simulate standard BST)
        if ((x->bf <= 1) && (x->bf >= -1)) return; // AVL Balanced Mode
        // if (true) return;                       // Standard BST Mode

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
        if (x->parent == nullptr) root = y;
        else if (x->parent->left == x) x->parent->left = y;
        else x->parent->right = y;
        if (y != nullptr) y->parent = x->parent;
    }

    void rrotate(Node* x) {
        Node* y = x->left;
        Transplant(y, y->right);
        Transplant(x, y);
        y->right = x;
        x->parent = y;
        recalc(x);
        recalc(y);
    }

    void lrotate(Node* x) {
        Node* y = x->right;
        Transplant(y, y->left);
        Transplant(x, y); 
        y->left = x; 
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
        if (p == nullptr) return;
        inorder(p->left);
        // cout << p->key << " "; // Uncomment to print keys
        inorder(p->right);
    }
};

int main() {
    AVL b;
    int n = 100000; // Dataset size
    
    cout << "Benchmarking AVL Tree with n=" << n << "..." << endl;

    auto startTime = chrono::high_resolution_clock::now();
    
    for (int i = 1; i <= n; i++) {
        // Toggle insertion mode:
        // Node* p = new Node((i * 7637 + 571) % n + 1); // Random inserts
        Node* p = new Node(i);                           // Sequential inserts
        b.insert(p);
    }

    auto endTime = chrono::high_resolution_clock::now();

    double totalTime = chrono::duration_cast<chrono::microseconds>(endTime - startTime).count() / 1000.0;

    cout << "------------------------------------------------" << endl;
    cout << "Results:" << endl;
    cout << "Tree Size (n):   " << b.size() << endl;
    cout << "Tree Height (h): " << b.height() << endl;
    cout << "Ratio h/log2(n): " << b.height() / (log(b.size()) / log(2)) << endl;
    cout << "Total Time:      " << totalTime << " ms" << endl;
    cout << "Avg Time/Insert: " << totalTime / b.size() << " ms" << endl;
    cout << "------------------------------------------------" << endl;

    return 0;
}