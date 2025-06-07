#include <iostream>
#include <string>
#include <vector>
#include <set>
#include <algorithm>
#include <sqlite3.h>
#include <map>
#include "NFL_Players.hpp"

using namespace std;
int main(){

sqlite3* DB;
char* errmsg = nullptr;

if(sqlite3_open("Player_Data_01062025.db", &DB)){
    cout << "DB cannot be opened: " << sqlite3_errmsg(DB) << endl;
    exit(0);
}

NFL_Players Player_1 = Generate_Player(DB);

cout << "Press Enter to add a second player!" << endl;
string empty;
getline(cin, empty);
//std::cout << "PLAYER 2" << "\n";
NFL_Players Player_2 = Generate_Player(DB);

NFL_Players::Calculate_Player_Relationship_Score(Player_1, Player_2);
NFL_Players::Relationship_Remarks(Player_1, Player_2);


sqlite3_close(DB);

return 0;

}
