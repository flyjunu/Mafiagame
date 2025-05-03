#include "AmnesiaPlayer.h"
#include <iostream>
using namespace std;

#include "AmnesiaPlayer.h"
#include <iostream>

AmnesiaPlayer::AmnesiaPlayer(string& name, string& initialHiddenRole)
    : Player(name), hiddenRole(initialHiddenRole)
{}

AmnesiaPlayer::~AmnesiaPlayer() {}

//������ �� ���ҷ� ���� ����
void AmnesiaPlayer::setHiddenRole(string& newRole) {
    hiddenRole = newRole;
}

// ������: ������ ���� ��½� ���
string AmnesiaPlayer::getHiddenRole() {
    return hiddenRole;
}

// �� ���� ����.(->�̺κ� �׳� 3�� ��� �������� player�� ��ӹ޾Ƽ� ����� ��������.)
// Ŭ���� ���̾�׷��� �Ŀ� �����ϰ���. 
void AmnesiaPlayer::nightAction() {
    if (!isAlive()) return;

    if (hiddenRole == "Doctor") {
        // TODO: �ǻ� �ൿ ����
         cout << "[AmnesiaPlayer: �ǻ� �ൿ ����]\n";
    }
    else if (hiddenRole == "Detective") {
        // TODO: ���� �ൿ ����
         cout << "[AmnesiaPlayer: ���� �ൿ ����]\n";
    }
    else {
        // Citizen ����: �ൿ ����
    }
}

 string AmnesiaPlayer::getRole() {
    return "???";
}