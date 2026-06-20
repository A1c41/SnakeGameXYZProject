#include "RecordManager.h"
#include "Constants.h"
#include <fstream>
#include <sstream>
#include <algorithm>

void loadRecords(Game& game) {
    game.records.clear();
    std::ifstream file(RECORDS_FILE);
    if (!file.is_open()) return;
    std::string line;
    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::string name;
        int score;
        if (std::getline(ss, name, ';') && (ss >> score)) {
            game.records.push_back({ name, score });
        }
    }
    file.close();
    std::sort(game.records.begin(), game.records.end(),
        [](const Record& a, const Record& b) { return a.score > b.score; });
}

void saveRecords(const Game& game) {
    std::ofstream file(RECORDS_FILE);
    if (!file.is_open()) return;
    for (const auto& rec : game.records) {
        file << rec.name << ";" << rec.score << "\n";
    }
    file.close();
}

bool isHighScore(const Game& game, int score) {
    if (game.records.size() < TOP_RECORDS_POPUP) return true;
    // Сравниваем с пятым по убыванию (индекс TOP_RECORDS_POPUP - 1)
    return score > game.records[TOP_RECORDS_POPUP - 1].score;
}

void updateRecords(Game& game) {
    if (!game.newRecord) return;
    game.records.push_back({ game.playerName, game.score });
    std::sort(game.records.begin(), game.records.end(),
        [](const Record& a, const Record& b) { return a.score > b.score; });
    saveRecords(game);
    game.newRecord = false;
}