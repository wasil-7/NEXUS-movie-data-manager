#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "structures.cpp" 
#include "AVL.cpp"
#include "Graphs.cpp"
#include "HashTable.cpp"

using namespace std;

string string_cleaner(string s) {
    int len = s.length();
    int start_index = 0;
    int end_index = len - 1;

    // remove surrounding quotes manually
    if (len >= 2 && s[0] == '"' && s[len - 1] == '"') {
        start_index = 1;
        end_index = len - 2;
    }

    // trim leading invalid ASCII
    while (start_index <= end_index) {
        unsigned char c = s[start_index];
        if (c <= 32 || c > 126) start_index++;
        else break;
    }

    // trim trailing invalid ASCII
    while (end_index >= start_index) {
        unsigned char c = s[end_index];
        if (c <= 32 || c > 126) end_index--;
        else break;
    }

    string cleaned = "";
    for (int i = start_index; i <= end_index; i++) {
        cleaned += s[i];
    }
    return cleaned;
}

//parsing
void loadData(const string& filename, MoviesTree& tree, ActorHashTable& hashTable, GenreHashTable& genreTable, MovieGraph& graph) {
    ifstream file(filename);
    if (!file.is_open()) {
        cout << "Failed to open file.\n";
        return;
    }
    cout << "Loading Data... Please wait.\n";
    string line;

    getline(file, line); //this getline skips the header
    
    int line_counter = 1;
    while (getline(file, line)){
        stringstream ss(line);
        string title_val, genres_val, actor_one, actor_two, actor_three, director_val, rating_val, year_val; //variables to store parsed sections from the metadata.csv
        string drop; //we used this var to remove/drop uneccassry data 

        getline(ss, drop, ','); //drops colors as not needed
        getline(ss, director_val, ',');    //saves director's name(1 col done)
        getline(ss, drop, ','); //drop 
        getline(ss, drop, ','); //drop 
        getline(ss, drop, ','); //drop 
        getline(ss, drop, ','); //drop 
        getline(ss, actor_two, ','); //saves actor 2(2nd col)
        getline(ss, drop, ','); //drop
        getline(ss, drop, ','); //drop 
        getline(ss, genres_val, ',');  //saves genres(3rd col)

        //since genres are seperated via | we alter the logic for parsing all genres here
        stringstream genres_stream(genres_val);
        int genre_count = 0;
        string genre_list[20]; //hardcoded this bit to assume there cant be more than 20 genres
        while( getline(genres_stream, genre_list[genre_count], '|') && genre_count < 10) {          
            genre_count++;  //col 4 done here ie; genres
        }

        getline(ss, actor_one, ','); //saves actor 1(5th col)
        getline(ss, title_val, ',');  //saves title(6th col)
        
        getline(ss, drop, ','); //drop 
        getline(ss, drop, ','); //drop 
        getline(ss, actor_three, ','); //save actor 3(7th col)
        getline(ss, drop, ','); //drop 
        getline(ss, drop, ','); //drop
        getline(ss, drop, ','); //drop 
        getline(ss, drop, ','); //drop 
        getline(ss, drop, ','); //drop 
        getline(ss, drop, ','); //drop 
        getline(ss, rating_val, ','); //saves rating(8th col)
        getline(ss, drop, ','); //drop 
        getline(ss, year_val, ','); //saves year(9th col)
        
        //  Clean Data & Convert Types
        title_val = string_cleaner(title_val);
        director_val = string_cleaner(director_val);
        rating_val = string_cleaner(rating_val);
        
        int year = 0;
        year_val = string_cleaner(year_val);

        bool valid_year = true;
        if (year_val.length() > 0) {
            for (int k = 0; k < year_val.length(); k++) {
                if (year_val[k] < '0' || year_val[k] > '9') {
                    valid_year = false;
                    break;
                }
            }
        }

        if (valid_year && year_val.length() > 0) {
            int temp_year = 0;
            for (int k = 0; k < year_val.length(); k++) {
                temp_year = temp_year * 10 + (year_val[k] - '0');
            }
            year = temp_year;
        } else {
            year = 0;
        }

        //insertion into the avl tree straight after parsing
        MovieNode* newMovie = new MovieNode(title_val, director_val, rating_val, year);
        tree.insert(newMovie);

        for(int genre_index = 0; genre_index < genre_count; genre_index++) {
             // cleanString removes potential hidden chars
            string gName = string_cleaner(genre_list[genre_index]);
            newMovie->addGenre(gName); 
   
            if (!gName.empty()) {
                GenreHashNode* genreNode = genreTable.insertGenre(gName);
                genreNode->addMovie(newMovie);
            }
        }

        //insertion for hashtable as well
        string actor_names[] = {actor_one, actor_two, actor_three};
        for (string actor_name : actor_names) {
            actor_name = string_cleaner(actor_name);
            if (!actor_name.empty()) {
                ActorNode* actorProfile = hashTable.insertActor(actor_name);
                actorProfile->addMovie(newMovie);
                newMovie->addActor(actorProfile);
            }
        }
        line_counter++;
    }

    file.close();
    cout << "Successfully loaded " << line_counter-1 << " movies.\n";

    cout << "Building Graph Connections...\n";
    graph.buildGraphFromActors(hashTable.getTable(), hashTable.getSize());
    graph.buildGraphFromGenres(genreTable.getTable(), genreTable.getSize());
    cout << "Graph Built successfully!\n";
}

int main() {
    MoviesTree myAVL;
    ActorHashTable myHashTable;
    GenreHashTable myGenreTable; 
    MovieGraph myGraph;

    // Pass new table to loadData
    loadData("movie_metadata.csv", myAVL, myHashTable, myGenreTable, myGraph);

    int choice;
    string user_input;
    
    while (true) {
        cout << " MOVIES DATA MANAGER (24i-2551 , 24i-2626)\n";
        cout << "1. Search Movie (Title)\n";
        cout << "2. Search Actor Profile & Co-Actors\n"; 
        cout << "3. Search Movies by Year\n"; 
        cout << "4. Search Movies by Rating\n"; 
        cout << "5. Search Movies by Genre\n"; 
        cout << "6. Get Recommendations (BFS)\n";
        cout << "7. Find Shortest Path\n";
        cout << "8. Exit\n";
        cout << "Enter Choice: ";
        cin >> choice;
        cin.ignore(); 

        if (choice == 8) break;

        MovieNode* result_movie = nullptr;
        ActorNode* result_actor = nullptr;
        GenreHashNode* result_genre = nullptr;
        MovieNode* start_movie = nullptr;
        MovieNode* end_movie = nullptr;

        switch (choice) {
        case 1: 
            cout << "Enter Movie Title: ";
            getline(cin, user_input);
            result_movie = myAVL.search(user_input); 
            if (result_movie) {
                cout << "\nFound: " << result_movie->title << " (" << result_movie->r_year << ")\n";
                cout << "Rating: " << result_movie->rating << "\n";
                // Add functionality to update info here if needed
                cout << "Update this movie? (y/n): ";
                string ans;
                getline(cin, ans);
                if(ans == "y") {
                     cout << "Enter new rating: ";
                     string nr; 
                     getline(cin, nr);
                     result_movie->setRating(nr);
                     cout << "Updated!\n";
                }
            } else { cout << "Not found.\n"; }
            break;

        case 2: 
            cout << "Enter Actor Name: ";
            getline(cin, user_input);
            result_actor = myHashTable.searchActor(user_input); 
            if (result_actor) {
                cout << "\nActor: " << result_actor->name << "\n";
                // Show Co-Actors
                myGraph.displayCoActors(result_actor);
            } else { cout << "Not found.\n"; }
            break;

        case 3:
            cout << "Enter Year: ";
            int y; cin >> y;
             cin.ignore();
            myAVL.searchMoviesByYear(y);
            break;

        case 4:
            cout << "Enter Content Rating (e.g., PG-13, R): ";
            getline(cin, user_input);
            // clean the input just in case user adds spaces
            user_input = string_cleaner(user_input); 
            myAVL.searchMoviesByRating(user_input);
            break;

        case 5:
            cout << "Enter Genre: ";
            getline(cin, user_input);
            result_genre = myGenreTable.searchGenre(user_input);
            if (result_genre) {
                cout << "\n--- Movies in Genre: " << result_genre->name << " ---\n";
                movies_list* temp = result_genre->movies;
                while(temp) {
                    cout << " - " << temp->movie->title << "\n";
                    temp = temp->next;
                }
            } else { cout << "Genre not found.\n"; }
            break;

        case 6: 
            cout << "Enter Movie Title: ";
            getline(cin, user_input);
            start_movie = myAVL.search(user_input);
            if (start_movie) {
                myAVL.resetAllVisited(); 
                myGraph.BFS_Recommendations(start_movie);
            } else { cout << "Not found.\n"; }
            break;

        case 7: 
            cout << "Enter Start Movie: ";
            getline(cin, user_input);
            start_movie = myAVL.search(user_input);
            cout << "Enter Target Movie: ";
            getline(cin, user_input);
            end_movie = myAVL.search(user_input);

            if (start_movie && end_movie) {
                myAVL.resetAllVisited(); 
                myGraph.findShortestPath(start_movie, end_movie); 
            } else { cout << "One or both not found.\n"; }
            break;
            
        default: cout << "Invalid Option.\n";
        }
    }
    return 0;
}