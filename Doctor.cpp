#include "Doctor.h"
#include <iostream>
#include <limits>

Doctor::Doctor(const std::string& name, int index, VoteSystem* voteSys)
    : Player(name), nightVote(voteSys), myIndex(index) {}

Doctor::~Doctor() {}

void Doctor::nightAction() {
    if (!isAlive()) return;
    int target;
    std::cout << "[Doctor]님, 치료할 번호만 입력하세요: ";
    std::cin >> target;
    nightVote->vote(myIndex, target);
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Doctor::getRole() const {
    return "Doctor";
}