#include "hashtable.h"
#include <string>
#include <iostream>
using namespace std;

const int TABLE_SIZE = 5007; 

class ActorHashTable {
private:
    ActorNode* table[TABLE_SIZE]; //array of ptrs to actor node, as told to be done in instructions

    //hash func
    int hashFunction(string key) {
        long long sum = 0;
        long long factor = 31;
        for (char c : key) {
            sum = (sum * factor + c) % TABLE_SIZE;
        }
        return (int)sum;
    }


public:
    ActorHashTable() {
    // to ensure every slot is set to nullptr (or 0).
        for (int i = 0; i < TABLE_SIZE; i++) {
        table[i] = nullptr;
    }
    }
    
    //give actor, return ptr of that node
    ActorNode* insertActor(string name) {
        int index = hashFunction(name);
        ActorNode* current = table[index]; 
        
        //searching
        while (current != nullptr) {
            if (current->name == name) {
                return current; 
            }
            current = current->next;
        }
        
        //inserting new actor if that one doesnt exist alr
        ActorNode* newActor = new ActorNode(name);
        newActor->next = table[index]; 
        table[index] = newActor;
        return newActor;
    }

    //searching in hash concept to search actor by name
    ActorNode* searchActor(string name) {
        int index = hashFunction(name);
        ActorNode* current = table[index];
        
        while (current != nullptr) {
            if (current->name == name) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }
    
    ActorNode** getTable() {
        return table;
    }

    int getSize() {
        return TABLE_SIZE;
    }
    // Cleaning up the Hashtable one by one to ensure proper clean up  
   ~ActorHashTable() {
    for (int i = 0; i < TABLE_SIZE; i++) {
        ActorNode* current = table[i];
        ActorNode* nextNode;
        while (current != nullptr) {
            nextNode = current->next;
            delete current; 
            current = nextNode;
        }
    }
}
};

class GenreHashTable {
private:
    GenreHashNode* table[TABLE_SIZE];

    int hashFunction(string key) {
        long long sum = 0;
        long long factor = 31;
        for (char c : key) {
            sum = (sum * factor + c) % TABLE_SIZE;
        }
        return (int)sum;
    }

public:
    GenreHashTable() {
        for (int i = 0; i < TABLE_SIZE; i++) {
            table[i] = nullptr;
        }
    }

    GenreHashNode* insertGenre(string name) {
        int index = hashFunction(name);
        GenreHashNode* current = table[index];

        while (current != nullptr) {
            if (current->name == name) {
                return current;
            }
            current = current->next;
        }

        GenreHashNode* newGenre = new GenreHashNode(name);
        newGenre->next = table[index];
        table[index] = newGenre;
        return newGenre;
    }

    GenreHashNode* searchGenre(string name) {
        int index = hashFunction(name);
        GenreHashNode* current = table[index];

        while (current != nullptr) {
            if (current->name == name) {
                return current;
            }
            current = current->next;
        }
        return nullptr;
    }

    GenreHashNode** getTable() {
        return table;
    }

    int getSize() {
        return TABLE_SIZE;
    }
};