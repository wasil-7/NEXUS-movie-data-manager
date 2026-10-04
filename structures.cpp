
#include <string>
#include <iostream>
using namespace std;

class MovieNode;
class ActorNode;

//7.1--MovieNode Class
class genre_node {  //this class is there for the list of genres
public:
    string genreName;
    genre_node* next;

    genre_node(string g) {
        genreName = g;
        next = nullptr;
    }
};

//7.2--ActorNode Class
class movies_list {  //this class contains the list of movies the actor has appeared in
public:
    MovieNode* movie; //full circle, points back to the node of movie in the avl tree which in turn has the name and other meta data of a movie
    movies_list* next;

    movies_list(MovieNode* m) {
        movie = m;
        next = nullptr;
    }
};

class cast { //this class acts as a mediator(node class) for the actual actor node class
public:
    ActorNode* actor; //referencing to the actual actor node that stores an actor's data
    cast* next;

    cast(ActorNode* a) {
        actor = a;
        next = nullptr;
    }
};

//7.3--GRAPHS
class neighbour {  //this class is used for maintaining the adjacecny list via being the source for graph edges
public:
    MovieNode* neighborMovie;  //connecting movies via this pointer
    neighbour* next;

    neighbour(MovieNode* m) {
        neighborMovie = m;
        next = nullptr;
    }
};

//7.2--ActorNode Class
class ActorNode {
public:
    string name;
    movies_list* movies; //referencing the linked list that contains the sequence of movies an actor has appeared in
    ActorNode* next;     //next here is used in chaining(as we will be doing dynaimc hashing via actor node)

    ActorNode(string n) {
        name = n;
        movies = nullptr;
        next = nullptr;
    }

    //7.4 linked list concept
    void addMovie(MovieNode* m) {
        movies_list* newNode = new movies_list(m);
        newNode->next = movies;
        movies = newNode;
    }
};

class GenreHashNode {
public:
    string name;
    movies_list* movies; // List of movies belonging to this genre
    GenreHashNode* next; // For chaining in Hash Table

    GenreHashNode(string n) {
        name = n;
        movies = nullptr;
        next = nullptr;
    }

    void addMovie(MovieNode* m) {
        movies_list* newNode = new movies_list(m);
        newNode->next = movies;
        movies = newNode;
    }
};

class MovieNode {
public:
    string title;
    string director;
    string rating;
    int r_year; //release year
    
    cast* actorsH;      //referencing to store actors
    genre_node* genreH;   //referencing to store movie genres

    //pointers used for traversal of tree bcz movie node is part of an AVL tree
    MovieNode* left;
    MovieNode* right;
    int height; //Essential for AVL Tree balancing logic later

    //part of the graphs implementation in 7.3(to connect similar movies)
    neighbour* neighborsHead; 
    bool visited; //Essential for BFS/DFS to prevent cycles(bakhshi understand why??)
    
    //Shortest Path Pointer(part of 7.3)
    MovieNode* parent; 

    MovieNode(string t, string d, string r, int y) {
        title = t;
        director = d;
        rating = r;
        r_year = y;
        actorsH = nullptr;
        genreH = nullptr;
        left = nullptr;
        right = nullptr;
        height = 1;
        neighborsHead = nullptr;
        visited = false;
        parent = nullptr;
    }

    //7.4 linked list concept
    void addGenre(string g) {
        genre_node* newNode = new genre_node(g);
        newNode->next = genreH;
        genreH = newNode;
    }

    //7.4 linked list concept
    void addActor(ActorNode* actor) {
        cast* newNode = new cast(actor);
        newNode->next = actorsH;
        actorsH = newNode;
    }

    // 7.1 Update Methods
    void setRating(string r) {
         rating = r;
    }
    void setTitle(string t) { 
        title = t;
     }
    void setDirector(string d) {
         director = d; 
    }
    void setYear(int y) { 
        r_year = y; 
    }
};
