#include "GameManager.h"
#include <cstdlib>   // rand(), srand()
#include <ctime>     // time()
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <conio.h>
#include <limits>
using namespace std;

GameManager::GameManager() {
    //�÷��̾� �� �Է¹ޱ�
    cout << "�÷��̾� ���� �Է��ϼ���: ";
    cin  >> playerCount;

    players = new Player*[playerCount]; // �÷��̾� ���
    playerName = new string[playerCount]; // �÷��̾� �̸� ����
    vote = new VoteSystem(playerCount); // ��ǥ �ý��� �۵�
    turn = 1; // ��+���� �ѹ� ������ turn+1
    
    cin.ignore();
    cout << "�÷��̾���� �̸��� �Է��ϼ���"<< endl;

    for (int i = 0; i < playerCount; ++i) {
        cout << i+1 << "�� �÷��̾� �̸�: ";
        getline(cin, playerName[i]);
    }
    
    // �̸� �������� ����
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(playerName[i], playerName[j]);
    }
    assignRoles();// ���� ���� �й�

    cout << "\n[�÷��̾� �̸�: �÷��̾� ��ȣ]\n";
    for (int i = 0; i < playerCount; ++i) {
        cout << playerName[i] << " : " << i << "\n";
        lastAlive[i] = true; // ù���� ��� �������� ǥ�ý�
    }

}

// �Ҹ���
GameManager::~GameManager() {
    delete vote;
    for (int i = 0; i < playerCount; ++i) {
        delete players[i];
    }
    delete[] players;
    delete[] playerName;
}

// ���� ����
void GameManager::assignRoles() {
    int mafiaCount     = 2; // ���Ǿ� ���� ��
    int doctorCount    = 1; // �ǻ� ��
    int detectiveCount = 1; // ���� ��
    int amnesiaCount   = 2; // ???(�������)�� ��
    int citizenCount   = playerCount - (mafiaCount + doctorCount + detectiveCount);

    string roles[100]; // �÷��̾����ŭ ���ҹ迭 ����
    int idx = 0;
    for (int i = 0; i < mafiaCount;     ++i) roles[idx++] = "Mafia";
    for (int i = 0; i < doctorCount;    ++i) roles[idx++] = "Doctor";
    for (int i = 0; i < detectiveCount; ++i) roles[idx++] = "Detective";
    for (int i = 0; i < citizenCount;   ++i) roles[idx++] = "Citizen";

    // 1) ���� �������� ����
    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(roles[i], roles[j]);
    }

    
    // 2) Amnesia ��� ���� (���Ǿƴ� ����)
    int amnesiaIdx[100] = {0};
    int ac = 0;
    while (ac < amnesiaCount) {
        int r = rand() % playerCount;
        if (amnesiaIdx[r]==0 && roles[r]!="Mafia") {
            amnesiaIdx[r] = 1;
            ++ac;
        }
    }


    // 3) ���� ���Ҵ�� players[]�� ����Ŭ���� ����
    for (int i = 0; i < playerCount; ++i) {
        string& nm = playerName[i];
        string& rl = roles[i];

        if (amnesiaIdx[i]) {
            players[i] = new AmnesiaPlayer(nm, rl);
        }
        else if (rl == "Mafia") {
            players[i] = new Mafia(nm);
        }
        else if (rl == "Doctor") {
            players[i] = new Doctor(nm);
        }
        else if (rl == "Detective") {
            players[i] = new Detective(nm);
        }
        else {
            players[i] = new Citizen(nm);
        }
    }

    // 4) (������)���� ��� ���
    cout << "\n[������ �ɼ�: ���� ���� Ȯ��]\n";
    for (int i = 0; i < playerCount; ++i) {
        cout << "  " << playerName[i] << " ("<< i <<") => ";
        if (amnesiaIdx[i]) {
            auto *ap = static_cast<AmnesiaPlayer*>(players[i]);
            cout << "???  (����: "<< ap->getRealRole() <<")\n";
        } else {
            cout << players[i]->getRole() << "\n";
        }
    }
};
// �� ���� 
void GameManager::runNightPhase() {
    int totalSec = 2 * 60;  // 2��
    cout << "\n===" << turn << "��° ���� ���۵˴ϴ�. (�����ð� 2:00) ===\n";
    cout << "��ɾ�: V <����ȣ> <����ȣ> / I(���) / Q(��������)\n";
    int m, s;
    bool earlyEnd = false;

    for (int sec = totalSec; sec >= 0; --sec) {
        if (sec == 60) {
            cout << "\n>>> 1�� ���ҽ��ϴ�! <<<\n";
        } else if (sec == 30) {
            cout << "\n>>> 30�� ���ҽ��ϴ�! <<<\n";
        } else if (sec == 10) {
            cout << "\n>>> 10�� ���ҽ��ϴ�! <<<\n";
        }
        if (sec <= 9) {
            cout << "\r���� �ð�: " << setw(2) << sec << "�� " << flush;
        }
        else {
            m = sec / 60, s = sec % 60;
            cout << "\r���� �ð�: " << setw(2) << setfill('0') << m << ":"
                 << setw(2) << setfill('0') << s << "   " << flush;
        }

        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < chrono::seconds(1)) {
            if (_kbhit()) {
                char c = _getch();

                if (c == 'Q') {
                    cout << "\n[���� �����մϴ�]\n";
                    sec = -1;
                    break;
                }

                if (c == 'I') {
                    cout << "\n[�÷��̾� ���]\n";
                    for (int i = 0; i < playerCount; ++i) {
                        if (players[i]->isAlive()) {
                            cout << players[i]->getName() << " : " << i << "\n";
                        }
                    }
                }

                if (c == 'V') {
                    int from, to;
                    if (cin >> from >> to) {
                        if (from >= 0 && from < playerCount &&
                            to   >= 0 && to   < playerCount &&
                            players[from]->isAlive()) {
                            cout << "\n[�� �ൿ�� ��ϵǾ����ϴ�.]\n";

                        } else {
                            cout << "\n[�߸��� �Է��Դϴ�]\n";
                        }
                    }
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }
    }

    // �� �ൿ ���� ����
    cout << "\n=== ��� �÷��̾��� ���ൿ�� ����Ǿ����ϴ�. ===\n";
    for (int i = 0; i < playerCount; ++i) {
        if (players[i]->isAlive()) {
            players[i]->nightAction();  // nihgt Action�� �ϸ� �� ���Һ� �ý����� �۵��ؾ���.
        }
    }

    // �������� ���� �÷��̾� �ִ��� üũ
    for (int i = 0; i < playerCount; ++i){
        lastAlive[i] = players[i]->isAlive();
    }

    // 2�ϸ��� ���� ����
    if (turn % 2 == 0) {
        cout <<"\n=== ����� �÷��̾��� ������ �������ϴ�. ===\n"<<endl;
        updateAmnesiaPlayers();
    }
};

//�� ����
void GameManager::runDayPhase() {
    int totalsec = 3*60;// 3�а� ��ħ����
    cout << "\n===" << turn << "��° ��ħ�� ��ҽ��ϴ�.(�����ð� 3��)===\n";
    int m, s;

    // �㿡 ����� �÷��̾� ���
    if (turn != 1) {
        cout << "[�� ���� ����� �÷��̾�]\n";
        bool anyDead = false;
        for (int i = 0; i < playerCount; ++i) {
            if (!players[i]->isAlive() && lastAlive[i]) {
                cout << " ? " << players[i]->getName() << " (" << i << "��)\n";
                anyDead = true;
            }
        }
        if (!anyDead) {
            cout << " �ǻ簡 ġ�ῡ �����߽��ϴ�.\n";
        }
    }

    cout << "��ɾ�: V <����ȣ> <����ȣ> / I(���) / Q(��������)\n";
    vote->resetVotes(); //��ǥ�� �ʱ�ȭ

    // ����ִ� �÷��̾� ����ŭ ��ǥ�� ������ ���� ����
    int neededVotes = 0; // �ʿ��� ��ǥ ��
    
    for (int i = 0; i < playerCount; ++i) 
        if (players[i]->isAlive())
            ++neededVotes;
    int votesCast = 0; // ���� ��ǥ��

    // ���� �ð� Ÿ�̸�
    for(int sec = totalsec; sec >=0; --sec){
        // 1��(60��), 30��, 10�� ������ �� �˸�
        if (sec == 120) {
            cout << "\n>>> 2�� ���ҽ��ϴ�! <<<\n";
        } else if (sec == 60) {
            cout << "\n>>> 1�� ���ҽ��ϴ�! <<<\n";
        }
          else if (sec == 30) {
            cout << "\n>>> 30�� ���ҽ��ϴ�! <<<\n";
        } else if (sec == 10) {
            cout << "\n>>> 10�� ���ҽ��ϴ�! <<<\n";
        }
        if (sec <= 9) {
            cout << "\r���� �ð�: " << setw(2) << sec << "�� " << flush;
        } 
        else {
            m = sec / 60, s = sec % 60;
            cout << "\r���� �ð�: " << setw(2) << setfill('0') << m << ":"
                 << setw(2) << setfill('0') << s << "   " << flush;
        }

        // ��ɾ� ���� �ý��� (V: ��ǥ�ϱ�, Q: �����ڿ�-�� ����, I: �÷��̾� ��ȣ Ȯ��)
        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < chrono::seconds(1)) {
            if (_kbhit()) {
                char c = _getch();
                if (c == 'Q') {
                    cout << "\n[���� ���� �����մϴ�]\n";
                    sec = -1;  // �ܺ� for�� Ż��
                    break;
                }
                if (c == 'V') {
                    int from, to;
                    if (cin >> from >> to) { //from: N���� to:M������ ��ǥ��
                        if (from >= 0 && from < playerCount
                         && to   >= 0 && to   < playerCount
                         && players[from]->isAlive()) {
                
                            vote->vote(from, to); 
                            ++votesCast;
                
                            // �޽��� ���
                            cout << "\n[��ǥ�� ���������� ��ϵǾ����ϴ�.]\n";
                            cout << from << "�� ��ǥ �Ϸ�\n";
                
                            if (votesCast >= neededVotes) {
                                cout << "[��� ��ǥ�� �Ϸ�Ǿ����ϴ�]\n";
                                sec = -1; //���� �����Ѵ�. 
                                break;
                            }
                        } else {
                            cout << "\n[�߸��� ��ǥ�Դϴ�]\n";
                        }
                    }
                
                    // ���� �Է� ����
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                }
            }
        }
    }


    // ��ǥ ��� ó��   
    int outIdx = vote->getMostVotedPlayer();
    cout << players[outIdx]->getName() << "���� ���ŵǾ����ϴ�.\n";
    players[outIdx]->kill();

    vote->resetVotes(); 
    ++turn;
    
}
// �¸� Ȯ��(true: �¸��� Ȯ��, false:�¸��� ����)
bool GameManager::checkWin() {
    int mafiaAlive = 0, citizenAlive = 0;
    for (int i = 0; i < playerCount; ++i) {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getRole() == "Mafia") ++mafiaAlive;
        else ++citizenAlive;
    }
    if (mafiaAlive == 0) { // ���Ǿư� 0���̸�
        cout << "[Citizens Win]\n";
        return true; //�ù��� �¸�
    }
    if (mafiaAlive >= citizenAlive) { //���Ǿư� �ùκ��� ��������
        cout << "[Mafia Win]\n";
        return true; // ���Ǿ� �¸�
    }
    return false;// ���� �¸��ڰ� �������� �ʾҽ��ϴ�.
}

// ???(�������) �÷��̾�� ���� �ٲٱ�
void GameManager::updateAmnesiaPlayers() {
    AmnesiaPlayer* amnesias[100];
    string oldRoles[100], newRoles[100];
    int count = 0;

    // 1. ���� ���� ����
    for (int i = 0; i < playerCount; ++i) {
        auto* ap = dynamic_cast<AmnesiaPlayer*>(players[i]);
        if (ap && players[i]->isAlive()) {
            amnesias[count]      = ap;
            oldRoles[count]      = ap->getHiddenRole();
            newRoles[count]      = oldRoles[count];
            ++count;
        }
    }
    if (count < 2) return;

    // 2. �������� ��� ������ �ο�
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(newRoles[i], newRoles[j]);
    }

    // 3. ����ȭ ���� ��족 ����: newRoles == oldRoles �� ��
    bool anyChanged = false;
    for (int i = 0; i < count; ++i) {
        if (newRoles[i] != oldRoles[i]) {
            anyChanged = true;
            break;
        }
    }
    if (!anyChanged) {
        // ���Ƿ� �� ���� ���� swap
        swap(newRoles[0], newRoles[1]);
    }

    // 4. ���� ���Ҵ�
    for (int i = 0; i < count; ++i) {
        amnesias[i]->setHiddenRole(newRoles[i]);
    }

    cout << "[AmnesiaPlayer���� ������ ��ȯ�Ǿ����ϴ�]\n";
}