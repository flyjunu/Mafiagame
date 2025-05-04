#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

class VoteSystem {
private:
    std::unordered_map<std::string, int> voteBox;

public:
    // 투표하기
    void vote(const std::string& playerName) {
        voteBox[playerName]++;
    }

    // 최다 득표자 출력
    std::string getTopVotedPlayer() const {
        if (voteBox.empty()) return "없음";

        int maxVotes = 0;
        std::vector<std::string> topPlayers;

        for (const auto& pair : voteBox) {
            if (pair.second > maxVotes) {
                maxVotes = pair.second;
                topPlayers.clear();
                topPlayers.push_back(pair.first);
            }
            else if (pair.second == maxVotes) {
                topPlayers.push_back(pair.first);
            }
        }

        if (topPlayers.size() == 1) {
            return topPlayers[0];
        }
        else {
            return "동률";
        }
    }

    // 투표함 리셋
    void resetVotes() {
        voteBox.clear();
    }

    // 투표 결과 출력
    void printVoteResults() const {
        std::cout << "투표 결과:" << std::endl;
        for (const auto& pair : voteBox) {
            std::cout << pair.first << ": " << pair.second << "표" << std::endl;
        }
    }
};