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
              << "��, �ൿ�� �÷��̾� ��ȣ�� �Է��ϼ��� (�� ����: ?" << "?" << "?" <<"):\n";
    if (std::cin >> target) {
        nightVote->vote(myIndex, target);
        std::cout << "[Amnesia] ��ǥ�� ���� ��ϵǾ����ϴ�.\n";
    }
    std::cin.clear();
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
}

std::string AmnesiaPlayer::getRole() const {
    return "???";
}