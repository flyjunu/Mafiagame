#include <iostream>
#include "GameManager.h"

int main() {
    GameManager gm;
    // ���� ���� �ݺ� �����ϴ� ���� ����
    while (true) {
        gm.runDayPhase();               // �� �ܰ� ����
        if (gm.checkWin()) break;       // �� �ܰ� �� �¸� ���� �˻�
        gm.runNightPhase();             // �� �ܰ� ����
        if (gm.checkWin()) break;       // �� �ܰ� �� �¸� ���� �˻�
    }
    return 0;
}