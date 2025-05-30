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
    // ���� ���� �ݺ� �����ϴ� ���� ���� 
    while (true) {
        gm.runDayPhase();               // �� �ܰ� ����
        if (gm.checkWin()) break;       // �� �ܰ� �� �¸� ���� �˻�
        gm.runNightPhase();             // �� �ܰ� ����
        if (gm.checkWin()) break;       // �� �ܰ� �� �¸� ���� �˻�
    }
}

void changesetting(GameSetting& setting) {
    cout << "[���� ���� ����]\n";
    while (true) {
        cout << "�� �÷��̾� ��: ";    cin >> setting.playerCount;
        cout << "���Ǿ� ��: ";        cin >> setting.mafia;
        cout << "�ǻ� ��: ";        cin >> setting.doctor;
        cout << "���� ��: ";        cin >> setting.detective;
        cout << "���ۿ� ��: ";      cin >> setting.agent;
        cout << "�ϳ׽þ� �� (0�̸� �̻��): "; cin >> setting.amnesia;
        
        int sum = setting.mafia + setting.doctor + setting.detective + setting.amnesia;
        if (sum > setting.playerCount) {
            cout << "���� ���� �÷��̾� ������ �����ϴ�. �ٽ� �õ��ϼ���.\n\n";
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            continue;
        }
        break;
    }
    // ���� �Ϸ� �� ���� ���� ����
    cin.clear();
    cin.ignore(numeric_limits<streamsize>::max(), '\n');
    cout << "\n�÷��̾� ������ �Ϸ�Ǿ����ϴ�.\n";
}

void showOptions() {
    cout << "�ɼ��� ǥ���մϴ�.\n";
}

void drawMenu(int selected) {
    system("cls"); // �ܼ� ȭ�� �ʱ�ȭ (������ ȯ��)
    cout << getTitleArt() << endl;
    string menu[MENU_COUNT] = { "���� ����", "�÷��̾� ����" };

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

        char key = _getch(); // Ű �Է� ���

        if (key == 72) { // �� Ű
            selected = (selected - 1 + MENU_COUNT) % MENU_COUNT;
        }
        else if (key == 80) { // �� Ű
            selected = (selected + 1) % MENU_COUNT;
        }
        else if (key == 13) { // Enter Ű
            system("cls");
            switch (selected) {
                case START_GAME:
                    startGame(currentSetting);
                    break;
                case CHANGE_SETTING:
                    changesetting(currentSetting);
                    break;
                
            }
            cout << "\n�ǵ��� ������ �ƹ� Ű�� ��������...";
            _getch();
        }
        else if (key == 27) { // ESC Ű
            running = false;
        }
    }

    return 0;
}