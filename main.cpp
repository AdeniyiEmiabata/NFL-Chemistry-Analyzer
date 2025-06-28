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
    cout << endl << endl;
    cout << "Welcome! This is the NFL Player Chemistry Analyzer." << endl; 
    cout << "This tool is used for assessing potential chemistry between two active NFL players." << endl;
    cout << "A Madden Ultimate Team/FIFA Ultimate Team-style algorithm is used to generate a relationship score." << endl;
    cout << "Scores are based on player details, such as team, offensive/defensive schemes, draft year, and more!" << endl;
    cout << "Player data is accurate, as of June 27th, 2025" << endl;
    cout << "Enjoy!" << endl;

    Main_Func();

    return 0;

}
