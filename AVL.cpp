#include "avl.h"
#include "structures.h"

#include <string>
#include <iostream>
using namespace std;

class MoviesTree {
private:
    MovieNode* root;

    //helpers as studied in avl lectures
    int getHeight(MovieNode* N) {
        if (N == nullptr) {
            return 0;
        }

        return N->height;
    }
int max(int a, int b){
    if (a > b)
        {
            return a;
        }
    else{
        return b;
    }
}

    int getBalance(MovieNode* N) {
        if (N == nullptr){ 
            return 0;
        }
        return getHeight(N->left) - getHeight(N->right);
    }

    // Rotations
    MovieNode* rightRotate(MovieNode* y) {
        MovieNode* x = y->left;
        MovieNode* T2 = x->right;
        x->right = y;
        y->left = T2;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        return x;
    }

    MovieNode* leftRotate(MovieNode* x) {
        MovieNode* y = x->right;
        MovieNode* T2 = y->left;
        y->left = x;
        x->right = T2;
        x->height = max(getHeight(x->left), getHeight(x->right)) + 1;
        y->height = max(getHeight(y->left), getHeight(y->right)) + 1;
        return y;
    }

    MovieNode* insertRec(MovieNode* node, MovieNode* newNode) {
        if (node == nullptr){ 
            return newNode;
        }
        //lexicographical comparison of titles
        if (newNode->title < node->title){
            node->left = insertRec(node->left, newNode);
                } 
        else if (newNode->title > node->title){
            node->right = insertRec(node->right, newNode);
        }
        else 
            return node; //to avoid dupes
        
        node->height = 1 + max(getHeight(node->left), getHeight(node->right));
        int balance = getBalance(node);

        if (balance > 1 && newNode->title < node->left->title){
            return rightRotate(node);
        }

        if (balance < -1 && newNode->title > node->right->title){
            return leftRotate(node);
        }

        if (balance > 1 && newNode->title > node->left->title) {
            node->left = leftRotate(node->left);
            return rightRotate(node);
        }

        if (balance < -1 && newNode->title < node->right->title) {
            node->right = rightRotate(node->right);
            return leftRotate(node);
        }

        return node;
    }

    MovieNode* searchRec(MovieNode* root, string title) {
        if (root == nullptr || root->title == title){
            return root;
        }

        if (root->title < title){
            return searchRec(root->right, title);
        }
            
        return searchRec(root->left, title);
    }

    //to make sure if the node had alr been visited or not
    void resetVisitedRec(MovieNode* node) {
        if (node == nullptr){ 
            return;
        }
        node->visited = false;
        resetVisitedRec(node->left);
        resetVisitedRec(node->right);
    }
    void searchByYearRec(MovieNode* node, int year, bool& found) {
        if (!node) {
            return;
        }

        searchByYearRec(node->left, year, found);
        
        if (node->r_year == year) {
            cout << "-> " << node->title << " (" << node->director << ")\n";
            found = true;
        }
        
        searchByYearRec(node->right, year, found);
    }


    void searchByContentRatingRec(MovieNode* node, string targetRating, bool& found) {
        if (!node){
             return;
        }
        
        searchByContentRatingRec(node->left, targetRating, found);
        
        if (node->rating == targetRating) {
            cout << "-> " << node->title << " [" << node->rating << "]\n";
            found = true;
        }

        searchByContentRatingRec(node->right, targetRating, found);
    }

public:
    MoviesTree() {
        root = nullptr;
    }

    void insert(MovieNode* newNode) {
        root = insertRec(root, newNode);
    }

    MovieNode* search(string title) {
        return searchRec(root, title);
    }

    MovieNode* getRoot() {
        return root;
    }

    void resetAllVisited() {
        resetVisitedRec(root);
    }

    void searchMoviesByYear(int year) {
        cout << "\n--- Movies Released in " << year << " ---\n";
        bool found = false;
        searchByYearRec(root, year, found);
        if (!found) cout << "No movies found for this year.\n";
        cout << "-------------------------------\n";
    }
   
    void searchMoviesByRating(string targetRating) {
        cout << "\n--- Movies Rated " << targetRating << " ---\n";
        bool found = false;
        searchByContentRatingRec(root, targetRating, found);
        if (!found) cout << "No movies found with this rating.\n";
        cout << "-------------------------------\n";
    }

};
