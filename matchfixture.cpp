#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <set>

using namespace std;

struct Team {
    string name;
    string town;
    string stadium;
};

struct Game {
    string home;
    string away;
    string town;
    string stadium;
    int leg{};
    int weekend{};
};

void readTeamsFromFile(const string& filename, vector<Team>& teams) {
    ifstream file("C:\\Users\\githu\\OneDrive\\Desktop\\C++vscode\\teamsfix.csv");
    if (!file.is_open()) {
        cerr << "Could not open the file: " << filename << endl;
        return;
    }

    string line;
    getline(file, line); // Skip header
    while (getline(file, line)) {
        stringstream s(line);
        Team team;
        getline(s, team.name, ',');
        getline(s, team.town, ',');
        getline(s, team.stadium, ',');
        teams.push_back(team);
    }
    file.close();
}

void generateFixtures(const vector<Team>& teams, vector<Game>& fixtures) {
    int weekend = 1;
    int matchesThisWeekend = 0;
    vector<Game> localTownMatchesLeg1;
    vector<Game> localTownMatchesLeg2;
    set<string> usedStadiums;

    for (int leg = 1; leg <= 2; ++leg) {
        for (size_t i = 0; i < teams.size(); ++i) {
            for (size_t j = i + 1; j < teams.size(); ++j) {
                if (teams[i].town == teams[j].town) {
                    Game match = {teams[i].name, teams[j].name, teams[i].town, teams[i].stadium, leg, 0};
                    if (leg == 1) {
                        localTownMatchesLeg1.push_back(match);
                    } else {
                        localTownMatchesLeg2.push_back(match);
                    }
                } else {
                    string stadium = (leg == 1) ? teams[i].stadium : teams[j].stadium;
                    if (usedStadiums.find(stadium) != usedStadiums.end()) {
                        continue;
                    }
                    Game match = {teams[i].name, teams[j].name, teams[i].town, stadium, leg, weekend};
                    fixtures.push_back(match);
                    usedStadiums.insert(stadium);
                    matchesThisWeekend++;

                    if (matchesThisWeekend == 2) {
                        weekend++;
                        matchesThisWeekend = 0;
                        usedStadiums.clear();
                    }
                }
            }
        }

        const vector<Game>& localMatches = (leg == 1) ? localTownMatchesLeg1 : localTownMatchesLeg2;
        for (const auto& match : localMatches) {
            string stadium = match.stadium;
            if (usedStadiums.find(stadium) != usedStadiums.end()) {
                continue;
            }
            Game newMatch = match; // Create a copy for modification
            newMatch.weekend = weekend;
            fixtures.push_back(newMatch);
            usedStadiums.insert(stadium);
            matchesThisWeekend++;

            if (matchesThisWeekend == 2) {
                weekend++;
                matchesThisWeekend = 0;
                usedStadiums.clear();
            }
        }
    }
}

void saveFixturesToFile(const vector<Game>& fixtures, const char* filename) {
    ofstream file(filename);
    if (!file.is_open()) {
        cerr << "Could not open the file: " << filename << endl;
        return;
    }

    file << "Home,Away,Town,Stadium,Leg,Weekend\n";
    for (const auto& match : fixtures) {
        file << match.home << ',' << match.away << ',' << match.town << ',' << match.stadium << ','
             << match.leg << ',' << match.weekend << '\n';
    }
    file.close();
}

void displayWeekendMatches(const vector<Game>& fixtures, int selected_weekend) {
    for (const auto& match : fixtures) {
        if (match.weekend == selected_weekend) {
            cout << match.home << " vs " << match.away << " at " << match.stadium
                 << " (Leg: " << match.leg << ")\n";
        }
    }
}

int main() {
    vector<Team> teams;
    readTeamsFromFile("teams.csv", teams);

    vector<Game> fixtures;
    generateFixtures(teams, fixtures);
    saveFixturesToFile(fixtures, "fixtures.csv");

    cout << "Welcome\n";
    cout << "Fixtures have been generated and saved to fixtures.csv\n";

    while (true) {
        int selectedWeekend;
        cout << "enter the weekend number (1 to 78) to view the fixtures, or 0 to exit: ";
        cin >> selectedWeekend;

        if (selectedWeekend == 0) {
            cout << "Thank you\n";
            break;
        }

        if (selectedWeekend < 1 || selectedWeekend > 78) {
            cout << "Invalid enter a number between 1 and 78.\n";
        } else {
            displayWeekendMatches(fixtures, selectedWeekend);
            char choice;
            cout << "view fixtures for another weekend? (y/n): ";
            cin >> choice;
            if (choice != 'y' && choice != 'Y') {
                cout << "Thank you\n";
                break;
            }
        }
    }

    return 0;
}

