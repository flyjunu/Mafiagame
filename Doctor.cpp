#include "Doctor.h"
#include <iostream>
#include <limits>

Doctor::Doctor(const std::string& name, int index, VoteSystem* voteSys)
    : Player(name), nightVote(voteSys), myIndex(index) {}

Doctor::~Doctor() {}

void Doctor::nightAction() {
    if (!isAlive()) return;
    int target;
    std::cout << "[Doctor] 님, 치료할 플레이어 번호를 입력하세요: ";
    if (std::cin >> target) {
        nightVote->vote(myIndex, target);
        std::cout << "[Doctor] 투표가 정상 등록되었습니다.\n";
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Doctor::getRole() const {
    return "Doctor";
}