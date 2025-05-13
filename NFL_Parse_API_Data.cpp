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

httplib::Client client_1("http://sports.core.api.espn.com");
// Access 32 TEAMS first then ROSTER then PLAYER DATA
string NFL_Teams_Path = "/v2/sports/football/leagues/nfl/teams?limit=32";

string api_data = "full_api_data.txt";

// check if the API data file and parsed data output file already exist, to avoid reprocessing
bool Check_DataFiles_Size (string& filename){

    if (filesystem::exists(filename) ){

        if(filesystem::file_size(filename) == 108883){
            cout << "Great! Welcome back!" << endl;
            return true;
        }

        return false;

    }
    
    return false;
}

// Parse API Data till players and store in a text file
map<string, string> Data_Extraction(){

    // Return map:
    map <string,string> Team_to_Player_Data_Lookup;
    
    ofstream Store_Here(api_data);

    auto Result_Teams = client_1.Get(NFL_Teams_Path);

    // END PROGRAM IF DATA IS NOT CORRECTLY GOTTEN
    if (Result_Teams->status != 200){
        std::cout << "Error getting API data!" << endl;
        exit(0);
    }

    // Parse data (teams)
    json raw_api_data = json::parse(Result_Teams->body);
    json All_Teams = raw_api_data["items"];
    string All_Teams_Data_Parsed = "{\"count\":32,\"team_data\":[";
    
    for(auto j: All_Teams){
        // get team http link
        string Team_Path = (j)["$ref"];
        Team_Path.erase(0, 31);
        //cout << Team_Path << endl;
        auto Result_Teams_Paths = client_1.Get(Team_Path);

        All_Teams_Data_Parsed.append(Result_Teams_Paths->body);
        All_Teams_Data_Parsed.append(",");
    }

    All_Teams_Data_Parsed.erase(All_Teams_Data_Parsed.end()-1);
    All_Teams_Data_Parsed.append("]}");

    cout << All_Teams_Data_Parsed << endl; // NO ISSUES HERE
    // Parse to get each team's roster
    string Team_Roster_Data_Parsed;
    //string append_to_path = "&limit="
    // cout <<"here" << endl;
    json Team_Rosters = json::parse(All_Teams_Data_Parsed);
    //json Team_Rosters_v2 = json::parse(Team_Rosters["items"]);
    int count = 0;
    
    for(auto j: Team_Rosters["team_data"]){

        string Team_Roster_Path = (j["athletes"])["$ref"];
        Team_Roster_Path.erase(0,31);
        cout << Team_Roster_Path << endl; // CHECK
        auto Result_Team_Roster_Data = client_1.Get(Team_Roster_Path);

        // find number of players to ensure the http path can display all players. &limit = _____
        json temp_find_count = json::parse(Result_Team_Roster_Data->body);
        int num_players = *temp_find_count.begin();
        // num_players.erase(std::remove(num_players.begin(), num_players.end(), '\"'), num_players.end());

        Team_Roster_Path.append("&limit=");
        Team_Roster_Path.append(to_string(num_players));

        Result_Team_Roster_Data = client_1.Get(Team_Roster_Path);
        json get_team_name = json::parse(Result_Team_Roster_Data -> body);
        string team_name = j["displayName"];
        cout << team_name << endl;
        team_name.erase(std::remove(team_name.begin(), team_name.end(), '\"'), team_name.end());
        Team_to_Player_Data_Lookup[team_name] = Result_Team_Roster_Data->body;       

        Team_Roster_Data_Parsed.append(Result_Team_Roster_Data->body);
        Team_Roster_Data_Parsed.append(",");
        
    }


    Team_Roster_Data_Parsed.erase(Team_Roster_Data_Parsed.end()-1);

    Store_Here << Team_Roster_Data_Parsed << endl;
    Store_Here.close();

    return Team_to_Player_Data_Lookup;
    
}

string Get_Team_Name(map<string,string> Team_to_Player_Data_Lookup){

    string Team_Name;

    cout << "Enter Team Name: " << endl;
    getline(cin, Team_Name);

    if (Team_to_Player_Data_Lookup.find(Team_Name) != Team_to_Player_Data_Lookup.end()){
        return Team_Name;
    }

    else{
        cout << "Error! Invalid team name" << endl;
        exit(0);
    }
   
}

map <string, string> Retrieve_Player_Info_For_Team(string Team_Name, map<string,string> Team_to_Player_Data_Lookup){

    map <string, string> PlayerName_PlayerData;

    json Player_List = json::parse(Team_to_Player_Data_Lookup[Team_Name]);

    for(auto j: Player_List["items"]){
        string Player_Info_Path = j["$ref"];

        Player_Info_Path.erase(0,31);
        
        auto Result_Player_Info = client_1.Get(Player_Info_Path);

        json Player_Info = json::parse(Result_Player_Info->body);

        PlayerName_PlayerData[Player_Info["displayName"]] = Player_Info_Path;

    }

    return PlayerName_PlayerData;
}

void Display_Player_Info (string Team_Name, map <string, string> PlayerName_PlayerData){
    ////// HERE 05.08.2025
    string Player_Name;

    cout << "Enter Player Name: " << endl;
    getline(cin, Player_Name);

    string Player_Info_Path = PlayerName_PlayerData[Player_Name];

    auto Result_Player_Info = client_1.Get(Player_Info_Path);

    json Player_Info = json::parse(Result_Player_Info->body);

    cout << "**********Player Bio**************" << endl << endl;
    cout << "Player Name: " << Player_Name << endl;
    cout << "Age: " << Player_Info["age"] << endl;
    cout << "Height: " << Player_Info["displayHeight"] << endl;
    cout << "Weight: " << Player_Info["displayWeight"] << endl;
    cout << "Draft Year: " << Player_Info["debutYear"] << endl;
    cout << "Position: " << (Player_Info["position"])["displayName"] << endl;

}
        


// // Get player name from user input
// string Get_Player_Name(){

//     //Get player name from user
//     string PlayerName;
//     std::cout << "What is the player's name?" << endl;
//     getline(cin, PlayerName);
//     std::cout << endl;
    
//     return PlayerName;

// }



int main(){

    // Determine whether or not to GET data again
    // bool Data_Exists = Check_DataFiles_Size(api_data);
    // if(!Data_Exists){
    //     Data_Extraction();
    //     cout << "1" << endl;
    // }

    map<string, string> FullData = Data_Extraction();
    cout << "2" << endl;
    string Team_Name = Get_Team_Name(FullData);
    cout << "3" << endl;
    map<string, string> PlayerName_Player_Data = Retrieve_Player_Info_For_Team(Team_Name, FullData);
    cout << "4" << endl;
    Display_Player_Info(Team_Name, PlayerName_Player_Data);
    cout << "5" << endl;
    
    return 0;
}
