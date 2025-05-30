#include "Agent.h"
#include <iostream>
#include <limits>


Agent::Agent(const std::string& name, int index, VoteSystem* voteSys)
    : Player(name), nightVote(voteSys), myIndex(index) {}

Agent::~Agent() {}

void Agent::nightAction() {
    if (!isAlive()) return;
    int target;
    std::cout << "[Agent]��, ��ǥ�� ������ų �÷��̾� ��ȣ�� �Է��ϼ���: ";
    std::cin >> target;
    nightVote->vote(myIndex, target);
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string Agent::getRole() const {
    return "Agent";
}