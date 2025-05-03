#include "AmnesiaPlayer.h"
#include <iostream>
using namespace std;

#include "AmnesiaPlayer.h"
#include <iostream>

AmnesiaPlayer::AmnesiaPlayer(string& name, string& initialHiddenRole)
    : Player(name), hiddenRole(initialHiddenRole)
{}

AmnesiaPlayer::~AmnesiaPlayer() {}

//역할을 새 역할로 덮어 쓰기
void AmnesiaPlayer::setHiddenRole(string& newRole) {
    hiddenRole = newRole;
}

// 디버깅용: 섞여진 직업 출력시 사용
string AmnesiaPlayer::getHiddenRole() {
    return hiddenRole;
}

// 각 역할 구현.(->이부분 그냥 3중 상속 하지말고 player만 상속받아서 기능은 복붙하자.)
// 클래스 다이어그램은 후에 수정하겠음. 
void AmnesiaPlayer::nightAction() {
    if (!isAlive()) return;

    if (hiddenRole == "Doctor") {
        // TODO: 의사 행동 구현
         cout << "[AmnesiaPlayer: 의사 행동 실행]\n";
    }
    else if (hiddenRole == "Detective") {
        // TODO: 경찰 행동 구현
         cout << "[AmnesiaPlayer: 경찰 행동 실행]\n";
    }
    else {
        // Citizen 역할: 행동 없음
    }
}

 string AmnesiaPlayer::getRole() {
    return "???";
}