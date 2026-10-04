#include "graphs.h"
#include "structures.h"
#include <iostream>
using namespace std;

//since queue is required for BFS implementation, we make a queue here
class QueueNode {
public:
    MovieNode* data;
    QueueNode* next;

    QueueNode(MovieNode* m) {
        data = m;
        next = nullptr;
    }
};

class MovieQueue {
private:
    QueueNode* front;
    QueueNode* rear;

public:
    MovieQueue() {
        front = nullptr;
        rear = nullptr;
    }

    bool isEmpty() {
        return front == nullptr;
    }

    //enqueue and dequeue basic logic
    void enqueue(MovieNode* movie) {
        QueueNode* newNode = new QueueNode(movie);
        if (rear == nullptr) {
            front = rear = newNode;
            return;
        }
        rear->next = newNode;
        rear = newNode;
    }

    MovieNode* dequeue() {
        if (isEmpty()) return nullptr;
        QueueNode* temp = front;
        MovieNode* data = front->data;
        front = front->next;
        if (front == nullptr) rear = nullptr;
        delete temp;
        return data;
    }
};

class MovieGraph {
public:

    //makes the actual graph structure
    void addEdge(MovieNode* src, MovieNode* dest) {
        if (!src || !dest) return;

        //duplication avoidance done here to remain in touch with implementation constraints
        neighbour* temp = src->neighborsHead;
        while (temp) {
            if (temp->neighborMovie == dest) return;
            temp = temp->next;
        }

        neighbour* newNode = new neighbour(dest);
        //insertion at head
        newNode->next = src->neighborsHead;
        src->neighborsHead = newNode;
    }

    //connects the movies
    void connectMoviesInList(movies_list* head) {
        movies_list* i = head;
        while (i != nullptr) {
            movies_list* j = i->next;
            while (j != nullptr) {
                //Link A->B and B->A
                addEdge(i->movie, j->movie);
                addEdge(j->movie, i->movie);
                j = j->next;
            }
            i = i->next;
        }
    }

    //makes the connections via iterating the actors in hashtable
    void buildGraphFromActors(ActorNode* hashTable[], int tableSize) {
        for (int i = 0; i < tableSize; i++) {
            ActorNode* currentActor = hashTable[i];
            while (currentActor != nullptr) {
                connectMoviesInList(currentActor->movies);
                currentActor = currentActor->next;
            }
        }
    }

    //algos
    void BFS_Recommendations(MovieNode* startNode) {
        if (!startNode) return;
        cout << "\n--- BFS Recommendations (Similar Movies) ---\n";
        MovieQueue q;
        startNode->visited = true;
        q.enqueue(startNode);

        while (!q.isEmpty()) {
            MovieNode* current = q.dequeue();

            // Prints detail via release year
            if (current != startNode) {
                cout << "-> " << current->title << " (" << current->r_year << ")\n";
            }

            //visits neighbours of that movie
            neighbour* n = current->neighborsHead;
            while (n != nullptr) {
                if (!n->neighborMovie->visited) {
                    n->neighborMovie->visited = true;
                    q.enqueue(n->neighborMovie);
                }
                n = n->next;
            }
        }
        cout << "-------------------------------------------\n";
    }

    void DFS_Recommendations(MovieNode* startNode) {
        if (!startNode) return;
        cout << "\n--- DFS Recommendations (Deep Dive) ---\n";
        DFS_Recursive(startNode);
        cout << "\n---------------------------------------\n";
    }

private:

    void DFS_Recursive(MovieNode* current) {
        current->visited = true;
        cout << "-> " << current->title << "\n";
        neighbour* n = current->neighborsHead;
        while (n != nullptr) {
            if (!n->neighborMovie->visited) {
                DFS_Recursive(n->neighborMovie);
            }
            n = n->next;
        }
    }

public:

    void findShortestPath(MovieNode* start, MovieNode* end) {
        if (!start || !end) return;
        cout << "\n--- Shortest Path: " << start->title << " to " << end->title << " ---\n";
        
        if (start == end) {
            cout << "Start and End are the same movie.\n";
            return;
        }

        MovieQueue q;
        start->visited = true;
        start->parent = nullptr;
        q.enqueue(start);
        bool found = false;

        while (!q.isEmpty()) {
            MovieNode* current = q.dequeue();
            
            if (current == end) {
                found = true;
                break;
            }

            neighbour* n = current->neighborsHead;
            while (n != nullptr) {
                if (!n->neighborMovie->visited) {
                    n->neighborMovie->visited = true;
                    n->neighborMovie->parent = current;
                    q.enqueue(n->neighborMovie);
                }
                n = n->next;
            }
        }

        if (found) {
            // The path is End <- Parent <- Parent <- Start
            MovieNode* path[1000]; // Assuming path won't exceed 1000
            int count = 0;
            MovieNode* temp = end;

            while (temp != nullptr) {
                path[count++] = temp;
                temp = temp->parent;
            }

            for (int i = count - 1; i >= 0; i--) {
                cout << path[i]->title;
                if (i > 0) cout << " -> ";
            }
            cout << "\n(Degrees of Separation: " << count - 1 << ")\n";

        } else {
            cout << "No connection found between these movies.\n";
        }

        cout << "-------------------------------------------\n";
    }
    
    // Optimized: Connects movies in a linear chain (A-B-C-D) instead of a web
    void buildGraphFromGenres(GenreHashNode* hashTable[], int tableSize) {
        for (int i = 0; i < tableSize; i++) {
            GenreHashNode* currentGenre = hashTable[i];
            while (currentGenre != nullptr) {
                
                movies_list* temp = currentGenre->movies;
                
                // connecting adjacent movies in the dataset
                while (temp != nullptr && temp->next != nullptr) {
                    addEdge(temp->movie, temp->next->movie);
                    addEdge(temp->next->movie, temp->movie);
                    temp = temp->next;
                }
                
                currentGenre = currentGenre->next;
            }
        }
    }

    void displayCoActors(ActorNode* actor) {
        if (!actor) return;
        cout << "\n--- Co-Actors of " << actor->name << " ---\n";
        
        movies_list* mList = actor->movies;
        while (mList != nullptr) {
            MovieNode* m = mList->movie;
            cout << "In Movie: " << m->title << "\n   With: ";
            
            cast* c = m->actorsH;
            while (c != nullptr) {
                if (c->actor->name != actor->name) { 
                    cout << c->actor->name << ", ";
                }
                c = c->next;
            }
            cout << "\n";
            mList = mList->next;
        }
        cout << "-------------------------------\n";
    }
};
