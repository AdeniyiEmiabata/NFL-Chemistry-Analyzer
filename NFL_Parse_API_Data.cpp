#include <iostream>
#include <fstream>
#include "cpp-httplib/httplib.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <string>
#include <filesystem>
#include <map>
#include <thread>
#include <future>

using namespace std;
using json = nlohmann::json;

// GLOBAL VARIABLES
httplib::Client client_1("http://sports.core.api.espn.com");
// Access 32 TEAMS first then ROSTER then PLAYER DATA
string NFL_Teams_Path = "/v2/sports/football/leagues/nfl/teams?limit=32";

vector<string> List_of_NFL_Teams = {
    "Arizona Cardinals",
    "Atlanta Falcons",
    "Baltimore Ravens",
    "Buffalo Bills",
    "Carolina Panthers",
    "Chicago Bears",
    "Cincinnati Bengals",
    "Cleveland Browns",
    "Dallas Cowboys",
    "Denver Broncos",
    "Detroit Lions",
    "Green Bay Packers",
    "Houston Texans",
    "Indianapolis Colts",
    "Jacksonville Jaguars",
    "Kansas City Chiefs",
    "Los Angeles Chargers",
    "Los Angeles Rams",
    "Las Vegas Raiders",
    "Miami Dolphins",
    "Minnesota Vikings",
    "New England Patriots",
    "New Orleans Saints",
    "New York Giants",
    "New York Jets",
    "Philadelphia Eagles",
    "Pittsburgh Steelers",
    "Seattle Seahawks"
    "San Francisco 49ers",
    "Tampa Bay Buccaneers",
    "Tennessee Titans",
    "Washington Commanders",   
};

int Get_Team_Name(){

    string Team_Name;

    cout << "Enter Team Name: " << endl;
    getline(cin, Team_Name);

    vector<string>::iterator Position = std::find(List_of_NFL_Teams.begin(), List_of_NFL_Teams.end(), Team_Name);

    if(Position != List_of_NFL_Teams.end()){
        int Position_Value = std::distance(List_of_NFL_Teams.begin(), Position);
        return Position_Value;
    }

    else{
        cout << "Error! Invalid team name" << endl;
        exit(0);
    }
   
}

string GET_Team_Data_from_HTTP(int Position){

    auto Result_Teams = client_1.Get(NFL_Teams_Path);

    // END PROGRAM IF DATA IS NOT CORRECTLY GOTTEN
    if (Result_Teams->status != 200){
        std::cout << "Error getting API data!" << endl;
        exit(0);
    }

    json Team_Path_Json = json::parse(Result_Teams->body);
    string Team_Path = (*((Team_Path_Json["items"]).begin() + Position))["$ref"];
    Team_Path.erase(0, 31);

    return Team_Path;

}

map<string, string> Get_Player_Data(string Team_Path){
    
    auto Results_Players = client_1.Get(Team_Path);

    // END PROGRAM IF DATA IS NOT CORRECTLY GOTTEN
    if (Results_Players->status != 200){
        std::cout << "Error getting API data!" << endl;
        exit(0);
    }

    json Team_Data = json::parse(Results_Players->body);
    string Team_Roster_Path = (Team_Data["athletes"])["$ref"];

    auto Result_Team_Roster_Data = client_1.Get(Team_Roster_Path);

    // find number of players to ensure the http path can display all players. &limit = _____
    json temp_find_count = json::parse(Result_Team_Roster_Data->body);
    int num_players = *temp_find_count.begin();

    Team_Roster_Path.append("&limit=");
    Team_Roster_Path.append(to_string(num_players));

    Result_Team_Roster_Data = client_1.Get(Team_Roster_Path);

    // END PROGRAM IF DATA IS NOT CORRECTLY GOTTEN
    if (Results_Players->status != 200){
        std::cout << "Error getting API data!" << endl;
        exit(0);
    }

    json Player_List = json::parse(Result_Team_Roster_Data->body);

    map<string, string> Player_Data;

    for(auto j: Player_List["items"]){

        string Player_Info_Path = j["$ref"];

        Player_Info_Path.erase(0,31);
        
        auto Result_Player_Info = client_1.Get(Player_Info_Path);

        json Player_Info = json::parse(Result_Player_Info->body);

        Player_Data[Player_Info["displayName"]] = Player_Info_Path;

    }

    return Player_Data;

}

void Access_Player_Information(map<string, string> Player_Data){
    
    string Player_Name;

    cout << "Enter Player Name: " << endl;

    getline(cin, Player_Name);

    if(Player_Data.find(Player_Name) == Player_Data.end()){
        cout << "This player is in a different team" << endl;
        exit(0);
    }

    auto Result_Player_Information = client_1.Get(Player_Data[Player_Name]);

    json Player_Information = json::parse(Result_Player_Information->body);

    cout << "\n\n**********Player Bio**************" << endl << endl;
    cout << "Player Name: " << Player_Name << endl;
    cout << "Age: " << Player_Information["age"] << endl;
    cout << "Height: " << Player_Information["displayHeight"] << endl;
    cout << "Weight: " << Player_Information["displayWeight"] << endl;
    cout << "Draft Year: " << (Player_Information["draft"])["year"] << endl;
    cout << "Position: " << (Player_Information["position"])["displayName"] << endl;
}

map<string, string> SetUp_DataStructures(){

    int Position = Get_Team_Name();

    string Team_Data = GET_Team_Data_from_HTTP(Position);

    map<string, string> Player_Data = Get_Player_Data(Team_Data);

    return Player_Data;

}


int main(){

    //string num_players;
    //int num_players_int;
    //cout << "How many players do you want to look up?" << endl;
    // getline(cin, num_players);
    // num_players_int = stoi(num_players);
    // int count = 0;

    // while (count < num_players_int){}
    cout << "Player 1" << endl<<endl;
    future<map<string,string>> map_result = async(launch::async, SetUp_DataStructures);

    this_thread::sleep_for(chrono::seconds(15));

    cout << endl;

    cout << "Player 2"<< endl<<endl;
    future<map<string,string>> map_result_2 = async(launch::async, SetUp_DataStructures);
    this_thread::sleep_for(chrono::seconds(35));

    cout << endl;

    cout << "Player 1" << endl<<endl;
    thread t1 (Access_Player_Information, map_result.get());
    this_thread::sleep_for(chrono::seconds(15));

    cout << "Player 2" << endl<<endl;
    thread t2 (Access_Player_Information, map_result_2.get());

    t1.join();
    t2.join();

    return 0;
}
