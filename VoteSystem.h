#pragma once
#include <iostream>
#include <unordered_map>
#include <string>
#include <vector>

class VoteSystem {
private:
    std::unordered_map<std::string, int> voteBox;

public:
    // ��ǥ�ϱ�
    void vote(const std::string& playerName) {
        voteBox[playerName]++;
    }

    // �ִ� ��ǥ�� ���
    std::string getTopVotedPlayer() const {
        if (voteBox.empty()) return "����";

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
            return "����";
        }
    }

    // ��ǥ�� ����
    void resetVotes() {
        voteBox.clear();
    }

    // ��ǥ ��� ���
    void printVoteResults() const {
        std::cout << "��ǥ ���:" << std::endl;
        for (const auto& pair : voteBox) {
            std::cout << pair.first << ": " << pair.second << "ǥ" << std::endl;
        }
    }
};