#include "Detective.h"
#include <iostream>
#include <limits>

Detective::Detective(const std::string& name, int index, VoteSystem* voteSys)
    : Player(name), nightVote(voteSys), myIndex(index) {}

Detective::~Detective() {}

void Detective::nightAction() {
    if (!isAlive()) return;
    int target;
    std::cout << "[Detective]님, 조사할 번호만 입력하세요: ";
    std::cin >> target;
    nightVote->vote(myIndex, target);
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Detective::getRole() const {
    return "Detective";
}