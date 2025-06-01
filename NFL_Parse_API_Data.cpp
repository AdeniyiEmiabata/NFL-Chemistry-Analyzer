#include <iostream>
#include <fstream>
#include "cpp-httplib/httplib.h"
#include <nlohmann/json.hpp>
#include <algorithm>
#include <string>
#include <filesystem>
#include <map>
#include <sqlite3.h>
#include <cstring>

using namespace std;
using json = nlohmann::json;

httplib::Client client_1("http://sports.core.api.espn.com");
// Access 32 TEAMS first then ROSTER then PLAYER DATA
string NFL_Teams_Path = "/v2/sports/football/leagues/nfl/teams?limit=32";


// Parse API Data till players and store in a text file
map<string, string> Data_Extraction(){

    // Return map:
    map <string,string> Team_to_Player_Data_Lookup;
    

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
       
        auto Result_Teams_Paths = client_1.Get(Team_Path);

        All_Teams_Data_Parsed.append(Result_Teams_Paths->body);
        All_Teams_Data_Parsed.append(",");
    }

    All_Teams_Data_Parsed.erase(All_Teams_Data_Parsed.end()-1);
    All_Teams_Data_Parsed.append("]}");

    
    // Parse to get each team's roster
    string Team_Roster_Data_Parsed;
  
    json Team_Rosters = json::parse(All_Teams_Data_Parsed);
  
    int count = 0;
    
    for(auto j: Team_Rosters["team_data"]){

        string Team_Roster_Path = (j["athletes"])["$ref"];
        Team_Roster_Path.erase(0,31);
        
        auto Result_Team_Roster_Data = client_1.Get(Team_Roster_Path);

        // find number of players to ensure the http path can display all players. &limit = _____
        json temp_find_count = json::parse(Result_Team_Roster_Data->body);
        int num_players = *temp_find_count.begin();

        Team_Roster_Path.append("&limit=");
        Team_Roster_Path.append(to_string(num_players));

        Result_Team_Roster_Data = client_1.Get(Team_Roster_Path);
        json get_team_name = json::parse(Result_Team_Roster_Data -> body);
        string team_name = j["displayName"];
    
        team_name.erase(std::remove(team_name.begin(), team_name.end(), '\"'), team_name.end());
        Team_to_Player_Data_Lookup[team_name] = Result_Team_Roster_Data->body;       

        Team_Roster_Data_Parsed.append(Result_Team_Roster_Data->body);
        Team_Roster_Data_Parsed.append(",");
        
    }


    Team_Roster_Data_Parsed.erase(Team_Roster_Data_Parsed.end()-1);

    return Team_to_Player_Data_Lookup;
    
}

void Validate_Team_Name(map<string,string> Team_to_Player_Data_Lookup, string Team_Name){


    if (Team_to_Player_Data_Lookup.find(Team_Name) != Team_to_Player_Data_Lookup.end()){
        return;
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

string Remove_Quotes_from_String(string Data){
    Data.erase(std::remove(Data.begin(), Data.end(), '\"'), Data.end());

    return Data;
}

string Escape_Character(string Player_Info){
    string Output_String;
    for(auto i: Player_Info){
        if(i == '\''){
            Output_String += '\'';
        }
        Output_String += i;
    }

    return Output_String;
}

map<string, string> Run_All_Parsing_Functions(string Team_Name){

    map<string, string> Team_to_Player_Data_Lookup = Data_Extraction();

    Validate_Team_Name(Team_to_Player_Data_Lookup, Team_Name);

    map<string, string> PlayerName_Player_Data = Retrieve_Player_Info_For_Team(Team_Name, Team_to_Player_Data_Lookup);

    return PlayerName_Player_Data;

}

int main(){

    sqlite3* DB;
    char* errmsg = nullptr;

    if(sqlite3_open("Player_Data.db", &DB)){
        cout << "DB cannot be opened: " << sqlite3_errmsg(DB) << endl;
        exit(0);
    }

    const char* CreateSQLTable = "CREATE TABLE IF NOT EXISTS Players (id INTEGER PRIMARY KEY AUTOINCREMENT, Name TEXT, Position TEXT, Team TEXT, Age INTEGER, Debut_Year INTEGER, Years_of_Experience INTEGER);";

    if(sqlite3_exec(DB, CreateSQLTable, nullptr, nullptr, &errmsg) != SQLITE_OK){
        cout << "SQL Open Error: " << errmsg << endl;
        sqlite3_free(errmsg);
        exit(0);
    }

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
    "Seattle Seahawks",
    "San Francisco 49ers",
    "Tampa Bay Buccaneers",
    "Tennessee Titans",
    "Washington Commanders"
    };

    for(auto i: List_of_NFL_Teams){
        map<string, string> PlayerName_Player_Data = Run_All_Parsing_Functions(i);

        for(auto j: PlayerName_Player_Data){

            auto Player_Info = client_1.Get(j.second);
    
            json Player_Info_Parsed = json::parse(Player_Info->body);
    
            char insertSQL [500];
    
            // Escape in the event of apostrophe/single quote in player name
            cout << "LINK LINK: " << j.second << endl;
            string Player_Name =  Escape_Character(Remove_Quotes_from_String(Player_Info_Parsed["fullName"]));
            string Player_Position = Remove_Quotes_from_String((Player_Info_Parsed["position"])["displayName"]);
    
            int age;
            if(Player_Info_Parsed.contains("age")){
                age = Player_Info_Parsed["age"].get<int>();
            }
            else{
                age = -1;
            }
    
            int draft_year;
            int years_of_experience;
            (Player_Info_Parsed["experience"])["years"].get<int>() > 0 ? years_of_experience = (Player_Info_Parsed["experience"])["years"].get<int>() - 1 : years_of_experience = (Player_Info_Parsed["experience"])["years"].get<int>();
    
            if(Player_Info_Parsed.contains("draft")){
                draft_year = (Player_Info_Parsed["draft"])["year"].get<int>();
            }
            else{
                draft_year = -1;
            }
    
            cout << "Name: " << Player_Name << " Age: " << age << " Draft: " << draft_year << endl;
    
            snprintf(insertSQL, 500, "INSERT INTO Players (Name, Position, Team, Age, Debut_Year, Years_of_Experience) VALUES ('%s', '%s', '%s', %i, %i, %i);", Player_Name.c_str(), Player_Position.c_str(), i.c_str(), age, draft_year, years_of_experience);
    
            const char* insertSQL_const = insertSQL;
    
            if (sqlite3_exec(DB, insertSQL_const, nullptr, nullptr, &errmsg) != SQLITE_OK){
                cout << "SQL Execution Error: " << errmsg << endl;
                sqlite3_free(errmsg);
                exit(0);
            }
        
        }
        
    }

    sqlite3_close(DB);
   
    return 0;

}
