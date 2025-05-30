#include <iostream>
#include <conio.h> // _kbhit(), _getch()
#include <limits>
#include "GameManager.h"
#include "AscciArt.h"
using namespace std;

GameSetting currentSetting;

enum MenuOption { START_GAME, CHANGE_SETTING, MENU_COUNT };

void startGame(const GameSetting& setting) {
    GameManager gm(setting);
    // 낮과 밤을 반복 진행하는 게임 루프 
    while (true) {
        gm.runDayPhase();               // 낮 단계 진행
        if (gm.checkWin()) break;       // 낮 단계 후 승리 조건 검사
        gm.runNightPhase();             // 밤 단계 진행
        if (gm.checkWin()) break;       // 밤 단계 후 승리 조건 검사
    }
}

void changesetting(GameSetting& setting) {
    cout << "[게임 설정 변경]\n";
    while (true) {
        cout << "총 플레이어 수: ";    cin >> setting.playerCount;
        cout << "마피아 수: ";        cin >> setting.mafia;
        cout << "의사 수: ";        cin >> setting.doctor;
        cout << "경찰 수: ";        cin >> setting.detective;
        cout << "공작원 수: ";      cin >> setting.agent;
        cout << "암네시아 수 (0이면 미사용): "; cin >> setting.amnesia;
        
        int sum = setting.mafia + setting.doctor + setting.detective + setting.amnesia;
        if (sum > setting.playerCount) {
            cout << "직업 수가 플레이어 수보다 많습니다. 다시 시도하세요.\n\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        break;
    }
    // 설정 완료 후 남은 개행 제거
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\n플레이어 설정이 완료되었습니다.\n";
}

void showOptions() {
    cout << "옵션을 표시합니다.\n";
}

void drawMenu(int selected) {
    system("cls"); // 콘솔 화면 초기화 (윈도우 환경)
    cout << getTitleArt() << endl;
    string menu[MENU_COUNT] = { "게임 시작", "플레이어 세팅" };

    for (int i = 0; i < MENU_COUNT; ++i) {
        if (i == selected)
            cout << "> ";
        else
            cout << "  ";
        cout << menu[i] << endl;
    }
}

int main() {
    int selected = 0;
    bool running = true;

    while (running) {
        drawMenu(selected);

        char key = _getch(); // 키 입력 대기

        if (key == 72) { // ↑ 키
            selected = (selected - 1 + MENU_COUNT) % MENU_COUNT;
        }
        else if (key == 80) { // ↓ 키
            selected = (selected + 1) % MENU_COUNT;
        }
        else if (key == 13) { // Enter 키
            system("cls");
            switch (selected) {
                case START_GAME:
                    startGame(currentSetting);
                    break;
                case CHANGE_SETTING:
                    changesetting(currentSetting);
                    break;
                
            }
            cout << "\n되돌아 가려면 아무 키나 누르세요...";
            _getch();
        }
        else if (key == 27) { // ESC 키
            running = false;
        }
    }

    return 0;
}