#include <iostream>
#include <vector>
#include <string>
#include <set>
#include <algorithm>
#include <sqlite3.h>
#include <map>
#include "NFL_Players.hpp"

using namespace std;
map<string, string> List_of_NFL_Teams = { {"Arizona Cardinals","NFC West"},
{"Atlanta Falcons", "NFC South"},
{"Baltimore Ravens", "AFC North"},
{"Buffalo Bills", "AFC East"},
{"Carolina Panthers", "NFC South"},
{"Chicago Bears","NFC North"},
{"Cincinnati Bengals","AFC North"},
{"Cleveland Browns", "AFC North"},
{"Dallas Cowboys", "NFC East"},
{"Denver Broncos","AFC West"},
{"Detroit Lions", "NFC North"},
{"Green Bay Packers", "NFC North"},
{"Houston Texans", "AFC South"},
{"Indianapolis Colts", "AFC South"},
{"Jacksonville Jaguars", "AFC South"},
{"Kansas City Chiefs", "AFC West"},
{"Los Angeles Chargers", "AFC West"},
{"Los Angeles Rams", "NFC West"},
{"Las Vegas Raiders", "AFC West"},
{"Miami Dolphins", "AFC East"},
{"Minnesota Vikings", "NFC North"},
{"New England Patriots", "AFC East"},
{"New Orleans Saints", "NFC South"},
{"New York Giants", "NFC East"},
{"New York Jets", "AFC East"},
{"Philadelphia Eagles", "NFC East"},
{"Pittsburgh Steelers", "AFC North"},
{"Seattle Seahawks", "NFC West"},
{"San Francisco 49ers", "NFC West"},
{"Tampa Bay Buccaneers", "NFC South"},
{"Tennessee Titans", "AFC South"},
{"Washington Commanders", "NFC East"}
};

// List of Positions (Sides of the Ball)
vector<string> Defense = {"Defensive Linemen", "Linebacker", "Defensive Back"};
vector<string> Offense = {"Quarterback", "Offensive Linemen", "Receiver", "Running Back"};
vector<string> Special_Teams = {"Special Teams"};


map<string, string> Position_Groups = {{"Defensive End", "Defensive Linemen"}, {"Linebacker", "Linebacker"}, {"Safety", "Defensive Back"},
{"Cornerback", "Defensive Back"}, {"Defensive Tackle", "Defensive Linemen"}, {"Quarterback", "Quarterback"}, {"Guard", "Offensive Linemen"}, {"Center", "Offensive Linemen"},
{"Offensive Tackle", "Offensive Linemen"}, {"Wide Receiver", "Receiver"}, {"Tight End", "Receiver"}, {"Running Back", "Running Back"}, {"Fullback", "Running Back"},
{"Place Kicker", "Special Teams"}, {"Long Snapper", "Special Teams"}, {"Punter", "Special Teams"}};


int NFL_Players::Calculate_Player_Relationship_Score (NFL_Players& PlayerA_Attributes, NFL_Players& PlayerB_Attributes){
    int Score = 0;
    
    // TEAM-FOCUSED: MAXIMUM 100
    if(PlayerA_Attributes.Team == PlayerB_Attributes.Team){
        Score += 80;

        if (PlayerA_Attributes.Broader_Position_Group == PlayerB_Attributes.Broader_Position_Group){
            Score += 10;
            if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group) {
                Score+= 7;
            }
        }

        if (PlayerA_Attributes.College == PlayerB_Attributes.College){
            Score += 3;
        }
        else if (abs(PlayerA_Attributes.Draft_Year - PlayerB_Attributes.Draft_Year) < 2){
            Score += 3;
        }
    }

    // DIVISION_FOCUSED: MAXIMUM 90
    else if (PlayerA_Attributes.Division == PlayerB_Attributes.Division){
        Score += 60;

        if (PlayerA_Attributes.Broader_Position_Group == PlayerB_Attributes.Broader_Position_Group){
            Score += 10;
            if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group){
                Score+= 5;
                if (PlayerA_Attributes.Position == PlayerB_Attributes.Position){
                    Score+= 7;
                }
            }
        }

        if (PlayerA_Attributes.College == PlayerB_Attributes.College){
            Score += 3;
            if(abs(PlayerA_Attributes.Draft_Year - PlayerB_Attributes.Draft_Year) < 2){
                Score += 5;
            }
        }

        else if(abs(PlayerA_Attributes.Draft_Year - PlayerB_Attributes.Draft_Year) < 2){
                Score += 3;
        }
    }

    // COLLEGE-FOCUSED: MAXIMUM 80
    else if(PlayerA_Attributes.College == PlayerB_Attributes.College){
        Score+= 50;

        if ((PlayerA_Attributes.Broader_Position_Group == PlayerB_Attributes.Broader_Position_Group)){
            Score += 5;
            if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group){
                Score+= 8;
                if(PlayerA_Attributes.Position == PlayerB_Attributes.Position){
                    Score += 12;
                }
            }
        }

        if (abs(PlayerA_Attributes.Draft_Year - PlayerB_Attributes.Draft_Year) < 2){
                Score += 5;
        }
        
    }

    //POSITION-GROUP FOCUSED: MAXIMUM 55
    else if(PlayerA_Attributes.Broader_Position_Group == PlayerB_Attributes.Broader_Position_Group){
        Score += 35;
        if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group){
                Score += 10;
                if(PlayerA_Attributes.Position == PlayerB_Attributes.Position){
                    Score += 15;
                }
            }

        if (abs(PlayerA_Attributes.Draft_Year - PlayerB_Attributes.Draft_Year) < 2){
                Score += 5;
            }
    }

    return Score;


}

void NFL_Players::Relationship_Remarks(NFL_Players& PlayerA_Attributes, NFL_Players& PlayerB_Attributes){

    int Relationship_Score = Calculate_Player_Relationship_Score(PlayerA_Attributes, PlayerB_Attributes);

    if (Relationship_Score >= 20 && Relationship_Score < 45){
        cout << PlayerA_Attributes.Name << " and " << PlayerB_Attributes.Name << " have a low chemistry score of " << Relationship_Score << "\n";
        if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group){
            std::cout << "These players may weaken your " << PlayerA_Attributes.Position_Group << " group and your overall " << PlayerA_Attributes.Broader_Position_Group << "\n";
        }
        cout << "We do not recommend having these players in the same team. Refresh the pool for better options!" << "\n";
        cout << "Low relationship scores affect player performance!" << "\n";

        if((PlayerA_Attributes.Position_Group == "Receiver" && PlayerB_Attributes.Position == "Quarterback") || (PlayerB_Attributes.Position_Group == "Receiver" && PlayerA_Attributes.Position == "Quarterback")){
            cout << "Your QB will struggle to be on the same page with your receiver! QB-Receiver chemistry is critical to a good offense\n";
        }

        if((PlayerA_Attributes.Position_Group == "Receiver" && PlayerB_Attributes.Position == "Quarterback") || (PlayerB_Attributes.Position_Group == "Receiver" && PlayerA_Attributes.Position == "Quarterback")){
            cout << "Your QB will struggle to be on the same page with your receiver! QB-Receiver chemistry is critical to a good offense\n";
        }

        return;
    }

    else if (Relationship_Score >= 45 && Relationship_Score < 65){
        cout << PlayerA_Attributes.Name << " and " << PlayerB_Attributes.Name << " have a below average chemistry score of " << Relationship_Score << "\n";
        if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group){
            std::cout << "These players may weaken your " << PlayerA_Attributes.Position_Group << " group and your overall " << PlayerA_Attributes.Broader_Position_Group << "\n";
        }
        cout << "These players are compatible. Reinforce your team with other players with higher relationship scores" << "\n";
        cout << "Low relationship scores affect player performance!" << "\n";

        if((PlayerA_Attributes.Position_Group == "Receiver" && PlayerB_Attributes.Position == "Quarterback") || (PlayerB_Attributes.Position_Group == "Receiverr" && PlayerA_Attributes.Position == "Quarterback")){
            cout << "Your QB may struggle to be on the same page with your receiver! QB-Receiver chemistry is critical to a good offense\n";
        }

        return;
    }

    else if (Relationship_Score >= 65 && Relationship_Score < 75){
        cout << PlayerA_Attributes.Name << " and " << PlayerB_Attributes.Name << " have an average chemistry score of " << Relationship_Score << "\n";
        cout << "These players are mildly compatible. Reinforce your team with other players with higher relationship scores" << "\n";
        return;
    }

    else if (Relationship_Score >= 75 && Relationship_Score < 85){
        cout << PlayerA_Attributes.Name << " and " << PlayerB_Attributes.Name << " have a good chemistry score of " << Relationship_Score << "\n";
        if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group){
            std::cout << "These players will strengthen your " << PlayerA_Attributes.Position_Group << " room and your overall " << PlayerA_Attributes.Broader_Position_Group << "\n";
        }
        cout << "These players are very compatible. Reinforce your team with more players with good relationship scores" << "\n";
        cout << "High relationship scores improve player performance" << "\n";
        if((PlayerA_Attributes.Position_Group == "Receiver" && PlayerB_Attributes.Position == "Quarterback") || (PlayerB_Attributes.Position_Group == "Receiver" && PlayerA_Attributes.Position == "Quarterback")){
            cout << "Your QB will have no trouble building a good relationship with your receiver!\n";
        }
        return;
    }

    else if (Relationship_Score >= 85 && Relationship_Score < 100){
        cout << PlayerA_Attributes.Name << " and " << PlayerB_Attributes.Name << " have a great chemistry score of " << Relationship_Score << "\n";
        if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group){
            std::cout << "These players will strengthen your " << PlayerA_Attributes.Position_Group << " room and your overall " << PlayerA_Attributes.Broader_Position_Group << "\n";;
        }
        cout << "These players will gel very well. Search the player pool for more players with similar relationship scores" << "\n";
        cout << "High relationship scores improve player performance" << "\n";

        if((PlayerA_Attributes.Position_Group == "Receiver" && PlayerB_Attributes.Position == "Quarterback") || (PlayerB_Attributes.Position_Group == "Receiver" && PlayerA_Attributes.Position == "Quarterback")){
            cout << "This QB-Receiver duo can be the foundation of your offense!\n";
        }
        return;
    }

    else if(Relationship_Score == 100){
        cout << PlayerA_Attributes.Name << " and " << PlayerB_Attributes.Name << " have an excellent chemistry score of " << Relationship_Score << "\n";
        if (PlayerA_Attributes.Position_Group == PlayerB_Attributes.Position_Group){
            std::cout << "These players will strengthen your " << PlayerA_Attributes.Position_Group << " room and your overall " << PlayerA_Attributes.Broader_Position_Group << "\n";;
        }
        cout << "Excellent selections! Search the player pool for more players with similar relationship scores" << "\n";
        cout << "High relationship scores improve player performance" << "\n";
        return;
    }

    cout << PlayerA_Attributes.Name << " and " << PlayerB_Attributes.Name << " have an extremely low chemistry score of " << Relationship_Score << "\n";
    cout << "Avoid putting these players in the same lineup!" << "\n";
    cout << "Low relationship scores improve player performance" << "\n";
    return;
}

NFL_Players Generate_Player(sqlite3* DB){
    string Name;
    string Team;
    string Division;
    string Position;
    string College;
    string Position_Group;
    string Broader_Position_Group;
    int Draft_Year;
    int Age;

    char querySQL[256];
    string player_name;
    cout << "Enter Player Name (WARNING: Include any name Suffixes!): " << endl;
    getline(cin, player_name);

    snprintf(querySQL, 256, "SELECT COUNT(*) FROM Players WHERE Name = '%s'", player_name.c_str());
    const char* querySQL_const = querySQL;
    sqlite3_stmt *statement; // associative table, pointer to first row

    sqlite3_prepare_v2(DB, querySQL_const, -1, &statement, NULL); //prepare statement in binary format

    sqlite3_step(statement);

    if (sqlite3_column_int(statement, 0) > 1){
        int num_players = sqlite3_column_int(statement, 0);
        cout << "There are " << num_players << " players with that name" << endl << endl;

        char querySQL_multipleplayers [256];
        snprintf(querySQL_multipleplayers, 256, "SELECT * FROM Players WHERE Name = '%s'", player_name.c_str());
        const char* querySQL_multipleplayers_const = querySQL_multipleplayers;
        sqlite3_stmt *multipleplayers_stmt;
        sqlite3_prepare_v2(DB, querySQL_multipleplayers_const, -1, &multipleplayers_stmt, NULL);

        while(sqlite3_step(multipleplayers_stmt)!=SQLITE_DONE){
            cout << player_name << ", Team: " << sqlite3_column_text(multipleplayers_stmt, 3) << ", Position: " << sqlite3_column_text(multipleplayers_stmt, 2) << endl;
        }

        cout << "Enter team name for desired player: " << endl;
        string team_name;
        getline(cin, team_name);

        char querySQL_finalselection [256];
        snprintf(querySQL_finalselection, 256, "SELECT * FROM Players WHERE Name = '%s' AND Team = '%s'", player_name.c_str(), team_name.c_str());
        const char* querySQL_finalselection_const = querySQL_finalselection;
        sqlite3_stmt *final_selection_stmt;
        sqlite3_prepare_v2(DB, querySQL_finalselection_const, -1, &final_selection_stmt, NULL);

        sqlite3_step(final_selection_stmt);
        Name = reinterpret_cast<const char*>(sqlite3_column_text(final_selection_stmt, 1));
        Position = reinterpret_cast<const char*>(sqlite3_column_text(final_selection_stmt, 2));
        Team = reinterpret_cast<const char*>(sqlite3_column_text(final_selection_stmt, 3));

        Age = sqlite3_column_int(final_selection_stmt, 4);

        Draft_Year = sqlite3_column_int(final_selection_stmt, 5);
        
        College = reinterpret_cast<const char*>(sqlite3_column_text(final_selection_stmt, 6));

    }

    else if(sqlite3_column_int(statement, 0) == 1){
        char querySQL_singleplayer [256];
        snprintf(querySQL_singleplayer, 256, "SELECT * FROM Players WHERE Name = '%s'", player_name.c_str());
        const char* querySQL_singleplayer_const = querySQL_singleplayer;
        sqlite3_stmt *singleplayer_stmt; // associative table, pointer to first row

        sqlite3_prepare_v2(DB, querySQL_singleplayer_const, -1, &singleplayer_stmt, NULL); //prepare statement in binary format??

        sqlite3_step(singleplayer_stmt);
        Name = reinterpret_cast<const char*>(sqlite3_column_text(singleplayer_stmt, 1));
        Position = reinterpret_cast<const char*>(sqlite3_column_text(singleplayer_stmt, 2));
        Team = reinterpret_cast<const char*>(sqlite3_column_text(singleplayer_stmt, 3));

        Age = sqlite3_column_int(singleplayer_stmt, 4);

        Draft_Year = sqlite3_column_int(singleplayer_stmt, 5);

        College = reinterpret_cast<const char*>(sqlite3_column_text(singleplayer_stmt, 6));
    }

    else{
        cout << "The player name entered is incorrect! Check for spelling errors or missing suffixes!" << endl;
        exit(0);
    }
    
    Division = Search_Division(Team);
    Position_Group = Search_PositionGroup(Position);
    Broader_Position_Group = Search_Broader_Position_Group (Position_Group);
    NFL_Players Player_Details(Name, Team, Division, Position, College, Position_Group, Broader_Position_Group, Draft_Year);
    //Print_Player_Bio(Player_Details);
    return Player_Details;

}

string Search_Division(string Team_Name){

    string Division = List_of_NFL_Teams[Team_Name];

    return Division;
    

}

string Search_PositionGroup(string Position){

    string Position_Group = Position_Groups[Position];
    
    return Position_Group;
}

string Search_Broader_Position_Group (string Position_Group){
    
    string Broader_Position_Group;

    vector<string>::iterator Offensive_Iter = std::find(Offense.begin(), Offense.end(), Position_Group);
    if (Offensive_Iter != Offense.end()){
        Broader_Position_Group = "Offense";
    }

    vector<string>::iterator Defensive_Iter = std::find(Defense.begin(), Defense.end(), Position_Group);
    if (Defensive_Iter != Defense.end()){
        Broader_Position_Group = "Defense";
    }

    return Broader_Position_Group;
}