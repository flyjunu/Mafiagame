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
    //플레이어 수 입력받기
    cout << "플레이어 수를 입력하세요: ";
    cin  >> playerCount;

    players = new Player*[playerCount]; // 플레이어 등록
    playerName = new string[playerCount]; // 플레이어 이름 저장
    vote = new VoteSystem(playerCount); // 투표 시스템 작동
    turn = 1; // 낮+밤이 한번 지나면 turn+1
    
    cin.ignore();
    cout << "플레이어들의 이름을 입력하세요"<< endl;

    for (int i = 0; i < playerCount; ++i) {
        cout << i+1 << "번 플레이어 이름: ";
        getline(cin, playerName[i]);
    }
    
    // 이름 무작위로 섞기
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(playerName[i], playerName[j]);
    }
    assignRoles();// 역할 랜덤 분배

    cout << "\n[플레이어 이름: 플레이어 번호]\n";
    for (int i = 0; i < playerCount; ++i) {
        cout << playerName[i] << " : " << i << "\n";
        lastAlive[i] = true; // 첫턴전 모두 생존으로 표시시
    }

}

// 소멸자
GameManager::~GameManager() {
    delete vote;
    for (int i = 0; i < playerCount; ++i) {
        delete players[i];
    }
    delete[] players;
    delete[] playerName;
}

// 역할 지정
void GameManager::assignRoles() {
    int mafiaCount     = 2; // 마피아 역할 수
    int doctorCount    = 1; // 의사 수
    int detectiveCount = 1; // 경찰 수
    int amnesiaCount   = 2; // ???(기억상실증)의 수
    int citizenCount   = playerCount - (mafiaCount + doctorCount + detectiveCount);

    string roles[100]; // 플레이어수만큼 역할배열 생성
    int idx = 0;
    for (int i = 0; i < mafiaCount;     ++i) roles[idx++] = "Mafia";
    for (int i = 0; i < doctorCount;    ++i) roles[idx++] = "Doctor";
    for (int i = 0; i < detectiveCount; ++i) roles[idx++] = "Detective";
    for (int i = 0; i < citizenCount;   ++i) roles[idx++] = "Citizen";

    // 1) 역할 랜덤으로 섞기
    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(roles[i], roles[j]);
    }

    
    // 2) Amnesia 대상 결정 (마피아는 제외)
    int amnesiaIdx[100] = {0};
    int ac = 0;
    while (ac < amnesiaCount) {
        int r = rand() % playerCount;
        if (amnesiaIdx[r]==0 && roles[r]!="Mafia") {
            amnesiaIdx[r] = 1;
            ++ac;
        }
    }


    // 3) 섞인 역할대로 players[]에 서브클래스 생성
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

    // 4) (디버깅용)섞인 결과 출력
    cout << "\n[개발자 옵션: 섞인 역할 확인]\n";
    for (int i = 0; i < playerCount; ++i) {
        cout << "  " << playerName[i] << " ("<< i <<") => ";
        if (amnesiaIdx[i]) {
            auto *ap = static_cast<AmnesiaPlayer*>(players[i]);
            cout << "???  (실제: "<< ap->getRealRole() <<")\n";
        } else {
            cout << players[i]->getRole() << "\n";
        }
    }
};
// 밤 실행 
void GameManager::runNightPhase() {
    int totalSec = 2 * 60;  // 2분
    cout << "\n===" << turn << "번째 밤이 시작됩니다. (남은시간 2:00) ===\n";
    cout << "명령어: V <내번호> <대상번호> / I(목록) / Q(조기종료)\n";
    int m, s;
    bool earlyEnd = false;

    for (int sec = totalSec; sec >= 0; --sec) {
        if (sec == 60) {
            cout << "\n>>> 1분 남았습니다! <<<\n";
        } else if (sec == 30) {
            cout << "\n>>> 30초 남았습니다! <<<\n";
        } else if (sec == 10) {
            cout << "\n>>> 10초 남았습니다! <<<\n";
        }
        if (sec <= 9) {
            cout << "\r남은 시간: " << setw(2) << sec << "초 " << flush;
        }
        else {
            m = sec / 60, s = sec % 60;
            cout << "\r남은 시간: " << setw(2) << setfill('0') << m << ":"
                 << setw(2) << setfill('0') << s << "   " << flush;
        }

        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < chrono::seconds(1)) {
            if (_kbhit()) {
                char c = _getch();

                if (c == 'Q') {
                    cout << "\n[밤을 종료합니다]\n";
                    sec = -1;
                    break;
                }

                if (c == 'I') {
                    cout << "\n[플레이어 목록]\n";
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
                            cout << "\n[밤 행동이 등록되었습니다.]\n";

                        } else {
                            cout << "\n[잘못된 입력입니다]\n";
                        }
                    }
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
        }
    }

    // 밤 행동 실제 실행
    cout << "\n=== 모든 플레이어의 밤행동이 실행되었습니다. ===\n";
    for (int i = 0; i < playerCount; ++i) {
        if (players[i]->isAlive()) {
            players[i]->nightAction();  // nihgt Action을 하면 각 역할별 시스템이 작동해야함.
        }
    }

    // 마지막에 죽은 플레이어 있는지 체크
    for (int i = 0; i < playerCount; ++i){
        lastAlive[i] = players[i]->isAlive();
    }

    // 2턴마다 역할 섞기
    if (turn % 2 == 0) {
        cout <<"\n=== 기억상실 플레이어의 역할이 섞였습니다. ===\n"<<endl;
        updateAmnesiaPlayers();
    }
};

//낮 실행
void GameManager::runDayPhase() {
    int totalsec = 3*60;// 3분간 아침실행
    cout << "\n===" << turn << "번째 아침이 밝았습니다.(남은시간 3분)===\n";
    int m, s;

    // 밤에 사망한 플레이어 출력
    if (turn != 1) {
        cout << "[밤 사이 사망한 플레이어]\n";
        bool anyDead = false;
        for (int i = 0; i < playerCount; ++i) {
            if (!players[i]->isAlive() && lastAlive[i]) {
                cout << " ? " << players[i]->getName() << " (" << i << "번)\n";
                anyDead = true;
            }
        }
        if (!anyDead) {
            cout << " 의사가 치료에 성공했습니다.\n";
        }
    }

    cout << "명령어: V <내번호> <대상번호> / I(목록) / Q(조기종료)\n";
    vote->resetVotes(); //투표함 초기화

    // 살아있는 플레이어 수만큼 투표가 끝나면 조기 종료
    int neededVotes = 0; // 필요한 투표 수
    
    for (int i = 0; i < playerCount; ++i) 
        if (players[i]->isAlive())
            ++neededVotes;
    int votesCast = 0; // 현재 투표수

    // 남은 시간 타이머
    for(int sec = totalsec; sec >=0; --sec){
        // 1분(60초), 30초, 10초 남았을 때 알림
        if (sec == 120) {
            cout << "\n>>> 2분 남았습니다! <<<\n";
        } else if (sec == 60) {
            cout << "\n>>> 1분 남았습니다! <<<\n";
        }
          else if (sec == 30) {
            cout << "\n>>> 30초 남았습니다! <<<\n";
        } else if (sec == 10) {
            cout << "\n>>> 10초 남았습니다! <<<\n";
        }
        if (sec <= 9) {
            cout << "\r남은 시간: " << setw(2) << sec << "초 " << flush;
        } 
        else {
            m = sec / 60, s = sec % 60;
            cout << "\r남은 시간: " << setw(2) << setfill('0') << m << ":"
                 << setw(2) << setfill('0') << s << "   " << flush;
        }

        // 명령어 감지 시스템 (V: 투표하기, Q: 개발자용-턴 종료, I: 플레이어 번호 확인)
        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < chrono::seconds(1)) {
            if (_kbhit()) {
                char c = _getch();
                if (c == 'Q') {
                    cout << "\n[낮을 조기 종료합니다]\n";
                    sec = -1;  // 외부 for문 탈출
                    break;
                }
                if (c == 'V') {
                    int from, to;
                    if (cin >> from >> to) { //from: N번이 to:M번에게 투표함
                        if (from >= 0 && from < playerCount
                         && to   >= 0 && to   < playerCount
                         && players[from]->isAlive()) {
                
                            vote->vote(from, to); 
                            ++votesCast;
                
                            // 메시지 출력
                            cout << "\n[투표가 정상적으로 등록되었습니다.]\n";
                            cout << from << "번 투표 완료\n";
                
                            if (votesCast >= neededVotes) {
                                cout << "[모든 투표가 완료되었습니다]\n";
                                sec = -1; //낮을 종료한다. 
                                break;
                            }
                        } else {
                            cout << "\n[잘못된 투표입니다]\n";
                        }
                    }
                
                    // 남은 입력 비우기
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');

                }
            }
        }
    }


    // 투표 결과 처리   
    int outIdx = vote->getMostVotedPlayer();
    cout << players[outIdx]->getName() << "님이 제거되었습니다.\n";
    players[outIdx]->kill();

    vote->resetVotes(); 
    ++turn;
    
}
// 승리 확인(true: 승리자 확인, false:승리자 없음)
bool GameManager::checkWin() {
    int mafiaAlive = 0, citizenAlive = 0;
    for (int i = 0; i < playerCount; ++i) {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getRole() == "Mafia") ++mafiaAlive;
        else ++citizenAlive;
    }
    if (mafiaAlive == 0) { // 마피아가 0명이면
        cout << "[Citizens Win]\n";
        return true; //시민의 승리
    }
    if (mafiaAlive >= citizenAlive) { //마피아가 시민보다 많아지면
        cout << "[Mafia Win]\n";
        return true; // 마피아 승리
    }
    return false;// 아직 승리자가 정해지지 않았습니다.
}

// ???(기억상실증) 플레이어들 역할 바꾸기
void GameManager::updateAmnesiaPlayers() {
    AmnesiaPlayer* amnesias[100];
    string oldRoles[100], newRoles[100];
    int count = 0;

    // 1. 기존 역할 저장
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

    // 2. 랜덤으로 섞어서 새역할 부여
    for (int i = count - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(newRoles[i], newRoles[j]);
    }

    // 3. “변화 없는 경우” 보장: newRoles == oldRoles 일 때
    bool anyChanged = false;
    for (int i = 0; i < count; ++i) {
        if (newRoles[i] != oldRoles[i]) {
            anyChanged = true;
            break;
        }
    }
    if (!anyChanged) {
        // 임의로 두 개만 역할 swap
        swap(newRoles[0], newRoles[1]);
    }

    // 4. 역할 재할당
    for (int i = 0; i < count; ++i) {
        amnesias[i]->setHiddenRole(newRoles[i]);
    }

    cout << "[AmnesiaPlayer들의 역할이 교환되었습니다]\n";
}