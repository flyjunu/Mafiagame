#include <iostream>
#include "GameManager.h"

int main() {
    GameManager gm;
    // 낮과 밤을 반복 진행하는 게임 루프
    while (true) {
        gm.runDayPhase();               // 낮 단계 진행
        if (gm.checkWin()) break;       // 낮 단계 후 승리 조건 검사
        gm.runNightPhase();             // 밤 단계 진행
        if (gm.checkWin()) break;       // 밤 단계 후 승리 조건 검사
    }
    return 0;
}