#include "Mafia.h"
#include <iostream>
#include <limits>

Mafia::Mafia(const std::string& name, int index, VoteSystem* voteSys)
    : Player(name), nightVote(voteSys), myIndex(index) {}

Mafia::~Mafia() {}

void Mafia::nightAction() {
    if (!isAlive()) return;
    int target;
    std::cout << "[Mafia]님, 살해할 플레이어 번호를 입력하세요: ";
    std::cin >> target;
    nightVote->vote(myIndex, target);

    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Mafia::getRole() const {
    return "Mafia";
}