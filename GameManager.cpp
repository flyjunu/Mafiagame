#include "GameManager.h"
#include <iostream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <conio.h>
#include <cstdlib>
#include <ctime>
#include <limits>
using namespace std;

GameManager::GameManager() {
    cout << "�÷��̾� ���� �Է��ϼ���: ";
    cin >> playerCount;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    playerName = new string[playerCount];
    players    = new Player*[playerCount];
    vote       = new VoteSystem(playerCount);
    turn       = 1;

    for (int i = 0; i < playerCount; ++i) {
        cout << i << "�� �÷��̾� �̸�: ";
        getline(cin, playerName[i]);
    }

    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(playerName[i], playerName[j]);
    }

    assignRoles();

    cout << "\n[�÷��̾� �̸� : ��ȣ]\n";
    for (int i = 0; i < playerCount; ++i) {
        cout << playerName[i] << " : " << i << "\n";
        lastAlive[i] = true;
    }
}

GameManager::~GameManager() {
    delete vote;
    for (int i = 0; i < playerCount; ++i)
        delete players[i];
    delete[] players;
    delete[] playerName;
}

void GameManager::assignRoles() {
    int mafiaCount     = 2;
    int doctorCount    = 1;
    int detectiveCount = 1;
    int amnesiaCount   = 2;
    int citizenCount   = playerCount - (mafiaCount + doctorCount + detectiveCount + amnesiaCount);

    string roles[100];
    int idx = 0;
    for (int i = 0; i < mafiaCount;     ++i) roles[idx++] = "Mafia";
    for (int i = 0; i < doctorCount;    ++i) roles[idx++] = "Doctor";
    for (int i = 0; i < detectiveCount; ++i) roles[idx++] = "Detective";
    for (int i = 0; i < citizenCount;   ++i) roles[idx++] = "Citizen";

    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(roles[i], roles[j]);
    }

    int amnesiaIdx[100] = {0}, ac = 0;
    while (ac < amnesiaCount) {
        int r = rand() % playerCount;
        if (!amnesiaIdx[r] && roles[r] != "Mafia") {
            amnesiaIdx[r] = 1; ++ac;
        }
    }

    for (int i = 0; i < playerCount; ++i) {
        const string& nm = playerName[i];
        const string& rl = roles[i];
        if (amnesiaIdx[i])
            players[i] = new AmnesiaPlayer(nm, rl, i, vote);
        else if (rl == "Mafia")
            players[i] = new Mafia(nm, i, vote);
        else if (rl == "Doctor")
            players[i] = new Doctor(nm, i, vote);
        else if (rl == "Detective")
            players[i] = new Detective(nm, i, vote);
        else
            players[i] = new Citizen(nm);
    }

    cout << "\n[������ �ɼ�: ���� ���� Ȯ��]\n";
    for (int i = 0; i < playerCount; ++i) {
        cout << "  " << playerName[i] << " ("<< i <<") => ";
        if (amnesiaIdx[i]) {
            auto *ap = static_cast<AmnesiaPlayer*>(players[i]);
            cout << "???  (����: "<< ap->getHiddenRole() <<")\n";
        } else {
            cout << players[i]->getRole() << "\n";
        }
    }
}

void GameManager::runNightPhase() {
    const int totalSec = 2 * 60;
    cout << "\n===" << turn << "��° ���� ���۵˴ϴ�. (�����ð� 2:00) ===\n"
         << "��ɾ�: V <����ȣ> <����ȣ> / I(���) / Q(��������)\n";

    int neededMafiaVotes = 0;
    for (int i = 0; i < playerCount; ++i)
        if (players[i]->isAlive() && players[i]->getRole() == "Mafia")
            ++neededMafiaVotes;
    int mafiaVotesCast = 0;
    int mafiaTargets[100];
    for (int i = 0; i < playerCount; ++i) mafiaTargets[i] = -1;

    // �� �Է�/Ÿ�̸�
    for (int sec = totalSec; sec >= 0; --sec) {
        if (sec == 60)      cout << "\n>>> 1�� ���ҽ��ϴ�! <<<\n";
        else if (sec == 30) cout << "\n>>> 30�� ���ҽ��ϴ�! <<<\n";
        else if (sec == 10) cout << "\n>>> 10�� ���ҽ��ϴ�! <<<\n";

        if (sec <= 9) {
            cout << "\r���� �ð�: " << setw(2) << sec << "��   " << flush;
        } else {
            int m = sec/60, s = sec%60;
            cout << "\r���� �ð�: "
                 << setw(2)<<setfill('0')<<m<<":"
                 << setw(2)<<setfill('0')<<s
                 << "   "<<flush;
        }

        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < chrono::seconds(1)) {
            if (!_kbhit()) continue;
            char c = _getch();
            if (c=='Q') { cout<<"\n[���� �����մϴ�]\n"; sec=-1; break; }
            if (c=='I') {
                cout<<"\n[������ ���]\n";
                for (int i=0;i<playerCount;++i)
                    if (players[i]->isAlive())
                        cout<<" - "<<playerName[i]<<" ("<<i<<")\n";
            }
            if (c=='V') {
                int from,to;
                if (cin>>from>>to) {
                    if (from>=0&&from<playerCount&&to>=0&&to<playerCount&&players[from]->isAlive()&&players[from]->getRole()=="Mafia") {
                        mafiaTargets[from]=to;
                        ++mafiaVotesCast;
                        cout<<"\n[�� �ൿ�� ��ϵǾ����ϴ�]\n";
                        if (mafiaVotesCast>=neededMafiaVotes) {
                            cout<<"[��� ���Ǿư� ��ǥ�� �Ϸ��߽��ϴ�]\n";
                            sec=-1; break;
                        }
                    } else {
                        cout<<"\n[�߸��� �Է��Դϴ�]\n";
                    }
                }
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(),'\n');
            }
        }
    }

    cout<<"\n=== ��� �÷��̾��� ���ൿ�� ����Ǿ����ϴ�. ===\n";
    for (int i=0;i<playerCount;++i)
        if (players[i]->isAlive())
            players[i]->nightAction();

    // ���Ǿ� ���� ���
    if (mafiaVotesCast>0) {
        int counts[100]={0};
        for (int i=0;i<playerCount;++i) {
            if (players[i]->isAlive()&&players[i]->getRole()=="Mafia") {
                int tgt=mafiaTargets[i];
                if (tgt>=0&&tgt<playerCount) ++counts[tgt];
            }
        }
        int maxC=0, killIdx=-1;
        for (int i=0;i<playerCount;++i) {
            if (counts[i]>maxC) { maxC=counts[i]; killIdx=i; }
        }
        if (killIdx!=-1 && players[killIdx]->isAlive()) {
            cout<<"\n[�� ���] "<<playerName[killIdx]
                <<"���� ���Ǿƿ��� ���ŵǾ����ϴ�.\n";
            players[killIdx]->kill();
        }
    }

    // �� ���� �� ���� ���� ����
    for (int i=0;i<playerCount;++i)
        lastAlive[i]=players[i]->isAlive();

    // �� ���� & Amnesia ��ȯ
    ++turn;
    if (turn%2==0) {
        cout<<"\n=== ����� �÷��̾��� ������ �������ϴ�. ===\n\n";
        updateAmnesiaPlayers();
    }
}

void GameManager::runDayPhase() {
    const int totalSec = 3 * 60;
    cout<<"\n==="<<turn<<"��° ��ħ�� ��ҽ��ϴ�.===\n"
        <<"[������ ���]\n";
    for (int i=0;i<playerCount;++i)
        if (players[i]->isAlive())
            cout<<" - "<<playerName[i]<<" ("<<i<<")\n";

    if (turn!=1) {
        cout<<"[�� ���� ����� �÷��̾�]\n";
        bool anyDead=false;
        for (int i=0;i<playerCount;++i) {
            if (!players[i]->isAlive()&&lastAlive[i]) {
                cout<<" ? "<<playerName[i]<<" ("<<i<<")\n";
                anyDead=true;
            }
        }
        if (!anyDead)
            cout<<" �ǻ簡 ġ�ῡ �����߽��ϴ�.\n";
    }

    cout<<"��ɾ�: V <����ȣ> <����ȣ> / I(���) / Q(��������)\n";
    vote->resetVotes();

    int neededVotes=0;
    for (int i=0;i<playerCount;++i)
        if (players[i]->isAlive()) ++neededVotes;
    int votesCast=0;

    for (int sec=totalSec;sec>=0;--sec) {
        if      (sec==120) cout<<"\n>>> 2�� ���ҽ��ϴ�! <<<\n";
        else if (sec==60)  cout<<"\n>>> 1�� ���ҽ��ϴ�! <<<\n";
        else if (sec==30)  cout<<"\n>>> 30�� ���ҽ��ϴ�! <<<\n";
        else if (sec==10)  cout<<"\n>>> 10�� ���ҽ��ϴ�! <<<\n";

        if (sec<=9) {
            cout<<"\r���� �ð�: "<<setw(2)<<sec<<"��   "<<flush;
        } else {
            int m=sec/60,s=sec%60;
            cout<<"\r���� �ð�: "
                <<setw(2)<<setfill('0')<<m<<":"
                <<setw(2)<<setfill('0')<<s
                <<"   "<<flush;
        }

        auto start=chrono::steady_clock::now();
        while (chrono::steady_clock::now()-start<chrono::seconds(1)) {
            if (!_kbhit()) continue;
            char c=_getch();
            if (c=='Q') { cout<<"\n[���� ���� �����մϴ�]\n"; sec=-1; break;}
            if (c=='I') {
                cout<<"\n[������ ���]\n";
                for (int i=0;i<playerCount;++i)
                    if (players[i]->isAlive())
                        cout<<" - "<<playerName[i]<<" ("<<i<<")\n";
            }
            if (c=='V') {
                int from,to;
                if (cin>>from>>to) {
                    if (from>=0&&from<playerCount&&to>=0&&to<playerCount&&players[from]->isAlive()) {
                        vote->vote(from,to);
                        ++votesCast;
                        cout<<"\n[��ǥ�� ���� ��ϵǾ����ϴ�]\n"
                            <<from<<"�� ��ǥ �Ϸ�\n";
                        if (votesCast>=neededVotes) {
                            cout<<"[��� ��ǥ �Ϸ�]\n";
                            sec=-1; break;
                        }
                    } else {
                        cout<<"\n[�߸��� ��ǥ�Դϴ�]\n";
                    }
                }
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(),'\n');
            }
        }
    }

    int outIdx=vote->getMostVotedPlayer();
    cout<<playerName[outIdx]<<"���� ���ŵǾ����ϴ�.\n";
    players[outIdx]->kill();

    // �� ���� �� ���� ���� ���
    for (int i=0;i<playerCount;++i)
        lastAlive[i]=players[i]->isAlive();
}

bool GameManager::checkWin() {
    int mafiaAlive=0, citizenAlive=0;
    for (int i=0;i<playerCount;++i) {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getRole()=="Mafia") ++mafiaAlive;
        else ++citizenAlive;
    }
    if (mafiaAlive==0) { cout<<"[Citizens Win]\n"; return true; }
    if (mafiaAlive>=citizenAlive) { cout<<"[Mafia Win]\n"; return true; }
    return false;
}

void GameManager::updateAmnesiaPlayers() {
    AmnesiaPlayer* amnesias[100];
    string roles[100];
    int count=0;
    for (int i=0;i<playerCount;++i) {
        auto* ap = dynamic_cast<AmnesiaPlayer*>(players[i]);
        if (ap && players[i]->isAlive()) {
            amnesias[count]=ap;
            roles[count]=ap->getHiddenRole();
            ++count;
        }
    }
    if (count<2) return;
    for (int i=count-1;i>0;--i) {
        int j=rand()%(i+1);
        swap(roles[i],roles[j]);
    }
    for (int i=0;i<count;++i)
        amnesias[i]->setHiddenRole(roles[i]);
}