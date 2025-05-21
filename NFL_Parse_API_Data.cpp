#include <iostream>
#include <fstream>
#include "cpp-httplib/httplib.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <string>
#include <filesystem>
#include <map>

using namespace std;
using json = nlohmann::json;

// GLOBAL VARIABLES
httplib::Client client_1("http://sports.core.api.espn.com");
// Access 32 TEAMS first then ROSTER then PLAYER DATA
string NFL_Teams_Path = "/v2/sports/football/leagues/nfl/teams?limit=32";

//string api_data = "full_api_data.txt";

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
    //string Team_Roster_Path;
    // num_players.erase(std::remove(num_players.begin(), num_players.end(), '\"'), num_players.end());

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

    auto Result_Player_Information = client_1.Get(Player_Data[Player_Name]);

    json Player_Information = json::parse(Result_Player_Information->body);

    cout << "**********Player Bio**************" << endl << endl;
    cout << "Player Name: " << Player_Name << endl;
    cout << "Age: " << Player_Information["age"] << endl;
    cout << "Height: " << Player_Information["displayHeight"] << endl;
    cout << "Weight: " << Player_Information["displayWeight"] << endl;
    cout << "Draft Year: " << Player_Information["debutYear"] << endl;
    cout << "Position: " << (Player_Information["position"])["displayName"] << endl;


}

// Parse API Data till players and store in a text file
// void Read_txt_file(){

//     // Return map:
//     map <string,string> Team_to_HTTP_Path;
    
    

//     auto Result_Teams = client_1.Get(NFL_Teams_Path);

//     // END PROGRAM IF DATA IS NOT CORRECTLY GOTTEN
//     if (Result_Teams->status != 200){
//         std::cout << "Error getting API data!" << endl;
//         exit(0);
//     }

//     // Parse data (teams)
//     json raw_api_data = json::parse(Result_Teams->body);
//     json All_Teams = raw_api_data["items"];
//     string All_Teams_Data_Parsed = "{\"count\":32,\"team_data\":[";
    
//     for(auto j: All_Teams){
//         // get team http link
//         string Team_Path = (j)["$ref"];
//         Team_Path.erase(0, 31);
//         //cout << Team_Path << endl;
//         auto Result_Teams_Paths = client_1.Get(Team_Path);

//         All_Teams_Data_Parsed.append(Result_Teams_Paths->body);
//         All_Teams_Data_Parsed.append(",");
//     }

//     All_Teams_Data_Parsed.erase(All_Teams_Data_Parsed.end()-1);
//     All_Teams_Data_Parsed.append("]}");

//     cout << All_Teams_Data_Parsed << endl; // NO ISSUES HERE
//     // Parse to get each team's roster
//     string Team_Roster_Data_Parsed;
//     //string append_to_path = "&limit="
//     // cout <<"here" << endl;
//     json Team_Rosters = json::parse(All_Teams_Data_Parsed);
//     //json Team_Rosters_v2 = json::parse(Team_Rosters["items"]);
//     int count = 0;
    
//     for(auto j: Team_Rosters["team_data"]){

//         string Team_Roster_Path = (j["athletes"])["$ref"];
//         Team_Roster_Path.erase(0,31);
//         cout << Team_Roster_Path << endl; // CHECK
//         auto Result_Team_Roster_Data = client_1.Get(Team_Roster_Path);

//         // find number of players to ensure the http path can display all players. &limit = _____
//         json temp_find_count = json::parse(Result_Team_Roster_Data->body);
//         int num_players = *temp_find_count.begin();
//         // num_players.erase(std::remove(num_players.begin(), num_players.end(), '\"'), num_players.end());

//         Team_Roster_Path.append("&limit=");
//         Team_Roster_Path.append(to_string(num_players));

//         Result_Team_Roster_Data = client_1.Get(Team_Roster_Path);
//         json get_team_name = json::parse(Result_Team_Roster_Data -> body);
//         string team_name = j["displayName"];
//         cout << team_name << endl;
//         team_name.erase(std::remove(team_name.begin(), team_name.end(), '\"'), team_name.end());
//         Team_to_Player_Data_Lookup[team_name] = Result_Team_Roster_Data->body;       

//         Team_Roster_Data_Parsed.append(Result_Team_Roster_Data->body);
//         Team_Roster_Data_Parsed.append(",");
        
//     }


//     Team_Roster_Data_Parsed.erase(Team_Roster_Data_Parsed.end()-1);

//     Store_Here << Team_Roster_Data_Parsed << endl;
//     Store_Here.close();

//     return Team_to_Player_Data_Lookup;
    
// }

// string Get_Team_Name(map<string,string> Team_to_Player_Data_Lookup){

//     string Team_Name;

//     cout << "Enter Team Name: " << endl;
//     getline(cin, Team_Name);

//     if (Team_to_Player_Data_Lookup.find(Team_Name) != Team_to_Player_Data_Lookup.end()){
//         return Team_Name;
//     }

//     else{
//         cout << "Error! Invalid team name" << endl;
//         exit(0);
//     }
   
// }

// map <string, string> Retrieve_Player_Info_For_Team(string Team_Name, map<string,string> Team_to_Player_Data_Lookup){

//     map <string, string> PlayerName_PlayerData;

//     json Player_List = json::parse(Team_to_Player_Data_Lookup[Team_Name]);

//     for(auto j: Player_List["items"]){
//         string Player_Info_Path = j["$ref"];

//         Player_Info_Path.erase(0,31);
        
//         auto Result_Player_Info = client_1.Get(Player_Info_Path);

//         json Player_Info = json::parse(Result_Player_Info->body);

//         PlayerName_PlayerData[Player_Info["displayName"]] = Player_Info_Path;

//     }

//     return PlayerName_PlayerData;
// }

// void Display_Player_Info (string Team_Name, map <string, string> PlayerName_PlayerData){
//     ////// HERE 05.08.2025
//     string Player_Name;

//     cout << "Enter Player Name: " << endl;
//     getline(cin, Player_Name);

//     string Player_Info_Path = PlayerName_PlayerData[Player_Name];

//     auto Result_Player_Info = client_1.Get(Player_Info_Path);

//     json Player_Info = json::parse(Result_Player_Info->body);

//     cout << "**********Player Bio**************" << endl << endl;
//     cout << "Player Name: " << Player_Name << endl;
//     cout << "Age: " << Player_Info["age"] << endl;
//     cout << "Height: " << Player_Info["displayHeight"] << endl;
//     cout << "Weight: " << Player_Info["displayWeight"] << endl;
//     cout << "Draft Year: " << Player_Info["debutYear"] << endl;
//     cout << "Position: " << (Player_Info["position"])["displayName"] << endl;

// }
        


int main(){

    int Position = Get_Team_Name();

    GET_Team_Data_from_HTTP(Position);

    Get_Player_Data(GET_Team_Data_from_HTTP(Position));

    Access_Player_Information(Get_Player_Data(GET_Team_Data_from_HTTP(Position)));
    
    return 0;
}
