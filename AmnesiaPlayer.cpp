#include "AmnesiaPlayer.h"
#include <iostream>
#include <limits>

AmnesiaPlayer::AmnesiaPlayer(const std::string& name,
                             const std::string& initialHiddenRole,
                             int index, VoteSystem* voteSys)
    : Player(name),
      hiddenRole(initialHiddenRole),
      nightVote(voteSys),
      myIndex(index)
{}

AmnesiaPlayer::~AmnesiaPlayer() {}

void AmnesiaPlayer::setHiddenRole(const std::string& newRole) {
    hiddenRole = newRole;
}

std::string AmnesiaPlayer::getHiddenRole() const {
    return hiddenRole;
}

void AmnesiaPlayer::nightAction() {
    if (!isAlive()) return;
    int target;
    std::cout << "[Amnesia] " << getName()
              << "님, 행동할 플레이어 번호를 입력하세요 (내 역할: ?" << "?" << "?" <<"):\n";
    if (std::cin >> target) {
        nightVote->vote(myIndex, target);
        std::cout << "[Amnesia] 투표가 정상 등록되었습니다.\n";
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string AmnesiaPlayer::getRole() const {
    return "???";
}