#include <iostream>
#include <string>
#include <set>
#include <sqlite3.h>
using namespace std;

class NFL_Players{

string Name;
string Team;
string Division;
string Position;
string College;
string Position_Group;
string Broader_Position_Group;
int Draft_Year;
string age;
// string Phase (to be added)
public:
NFL_Players(string Name, string Team, string Division, string Position, string College, string Position_Group, string Broader_Position_Group, int Draft_Year):Name(Name), Team(Team), Division(Division), 
Position(Position), College(College), Position_Group(Position_Group), Broader_Position_Group(Broader_Position_Group), Draft_Year(Draft_Year){
    std::cout << "-----------------------PLAYER BIO-----------------------" << "\n";
    std::cout << "Name: " << Name << "\n";
    std::cout << "Team: " << Team << "\n";
    std::cout << "Division: " << Division << "\n";
    std::cout << "Offense/Defense/Special Teams: " << Broader_Position_Group << "\n";
    std::cout << "Position: " << Position << "\n";
    std::cout << "Position Group: " << Position_Group << "\n";
    std::cout << "College: " << College << "\n";
    if (Draft_Year!= -1){
        std::cout << "Draft Year: " << Draft_Year << "\n\n";
    }
    else{
        std::cout << "Draft Year: UDFA" << "\n\n";
    }
};

~NFL_Players(){};

static int Calculate_Player_Relationship_Score (NFL_Players& PlayerA_Attributes, NFL_Players& PlayerB_Attributes);
static void Relationship_Remarks(NFL_Players& PlayerA_Attributes, NFL_Players& PlayerB_Attributes);

// void Print_Player_Bio(NFL_Players&Player_Details){
//}

};

string Search_Division(string Team_Name);

NFL_Players Generate_Player(sqlite3*DB);

string Search_PositionGroup(string Position);

string Search_Broader_Position_Group (string Position_Group);

string Escape_Character(string Player_Info);

string Offensive_Scheme(string Team_Name);

string Defensive_Scheme(string Team_Name);