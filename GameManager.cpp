
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
    std::cout << "플레이어 수를 입력하세요: ";
    cin >> playerCount;
    cin.ignore(numeric_limits<streamsize>::max(), '\n');

    playerName = new string[playerCount];
    players = new Player * [playerCount];
    vote = new VoteSystem(playerCount);
    turn = 1;
    lastDetectiveTarget = -1;
    for (int i = 0; i < playerCount; ++i) {
        std::cout << i << "번 플레이어 이름: ";
        getline(cin, playerName[i]);
    }

    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(playerName[i], playerName[j]);
    }

    assignRoles();

    std::cout << "\n[플레이어 이름 : 번호]\n";
    for (int i = 0; i < playerCount; ++i) {
        std::cout << playerName[i] << " : " << i << "\n";
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
    int mafiaCount = 1;
    int doctorCount = 1;
    int detectiveCount = 1;
    int amnesiaCount = 0;
    int citizenCount = playerCount - (mafiaCount + doctorCount + detectiveCount);

    string roles[100];
    int idx = 0;
    for (int i = 0; i < mafiaCount; ++i) roles[idx++] = "Mafia";
    for (int i = 0; i < doctorCount; ++i) roles[idx++] = "Doctor";
    for (int i = 0; i < detectiveCount; ++i) roles[idx++] = "Detective";
    for (int i = 0; i < citizenCount; ++i) roles[idx++] = "Citizen";

    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(roles[i], roles[j]);
    }

    int amnesiaIdx[100] = { 0 }, ac = 0;
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

    std::cout << "\n[개발자 옵션: 섞인 역할 확인]\n";
    for (int i = 0; i < playerCount; ++i) {
        std::cout << "  " << playerName[i] << " (" << i << ") => ";
        if (amnesiaIdx[i]) {
            auto* ap = static_cast<AmnesiaPlayer*>(players[i]);
            std::cout << "???  (실제: " << ap->getHiddenRole() << ")\n";
        }
        else {
            std::cout << players[i]->getRole() << "\n";
        }
    }
}

void GameManager::runNightPhase() {
    using namespace std::chrono;
    const int totalSec = 2 * 60;
    bool earlyEnd = false;
    cout << "\n===" << turn << "번째 밤이 시작됩니다. (남은시간 2:00) ===\n"
        << "[I] 생존자 목록  [Q] 조기종료\n";
    for (int sec = totalSec; sec >= 0; --sec) {
        int m = sec / 60, s = sec % 60;
        std::cout << "\r남은 시간: "
            << setw(2) << setfill('0') << m << ":"
            << setw(2) << setfill('0') << s
            << "   " << std::flush;

        auto start = steady_clock::now();
        while (steady_clock::now() - start < seconds(1)) {
            if (_kbhit()) {
                char c = toupper(_getche());
                if (c == 'Q') {
                    earlyEnd = true;
                    break;
                }
                if (c == 'I') {
                    // 목록 출력
                    cout << "\n[생존자 목록]\n";
                    for (int i = 0; i < playerCount; ++i) {
                        if (players[i]->isAlive())
                            cout << " - " << playerName[i]
                            << " (" << i << ")\n";
                    }
                    cout << "[계속하려면 아무 키나 누르세요]" << flush;
                    _getche();
                    cout << '\n';
                }
            }
        }
        if (earlyEnd) break;
    }

    // 2) 밤 행동 등록 준비
    cout << "\n\n=== 밤 행동을 등록합니다 ===\n";
    vote->resetVotes();
    int doctorTarget = -1, detectiveTarget = -1;
    int mafiaVotes[100] = { 0 };

    // actor 순서: Mafia → Doctor → Detective → Amnesia
    int actorOrder[100], orderCount = 0;
    for (int i = 0; i < playerCount; ++i)
        if (players[i]->isAlive() && players[i]->getRole() == "Mafia")
            actorOrder[orderCount++] = i;
    for (int i = 0; i < playerCount; ++i)
        if (players[i]->isAlive() && players[i]->getRole() == "Doctor")
            actorOrder[orderCount++] = i;
    for (int i = 0; i < playerCount; ++i)
        if (players[i]->isAlive() && players[i]->getRole() == "Detective")
            actorOrder[orderCount++] = i;
    for (int i = 0; i < playerCount; ++i)
        if (players[i]->isAlive() && players[i]->getRole() == "???")
            actorOrder[orderCount++] = i;

    // 3) 각 actor별 대상 입력 (숫자만)
    for (int idx = 0; idx < orderCount; ++idx) {
        int actor = actorOrder[idx];
        // 실제 역할(Amnesia는 hiddenRole)
        std::string role = players[actor]->getRole();
        if (role == "???") {
            auto* ap = dynamic_cast<AmnesiaPlayer*>(players[actor]);
            role = ap->getHiddenRole();
        }

        // 반복해서 유효 입력 받을 때까지
        while (true) {
            cout << "[" << role << "] " << playerName[actor]
                << "님, 대상 번호만 입력하세요 (I:목록): ";
            std::string line;
            std::getline(std::cin, line);

            // I → 목록
            if (line == "I" || line == "i") {
                cout << "[생존자 목록]\n";
                for (int j = 0; j < playerCount; ++j)
                    if (players[j]->isAlive())
                        cout << " - " << playerName[j]
                        << " (" << j << ")\n";
                continue;
            }

            // 숫자 파싱
            try {
                int tgt = std::stoi(line);
                if (tgt < 0 || tgt >= playerCount)
                    throw std::out_of_range("bad");
                if (!players[tgt]->isAlive() && role == "Mafia") {
                    // 마피아는 살아있는 대상만 공격 가능
                    cout << "  [해당 번호는 생존자가 아닙니다]\n";
                    continue;
                }
                // 역할별 처리
                if (role == "Mafia") {
                    mafiaVotes[tgt]++;
                    cout << "  → 공격 대상: " << tgt << "\n";
                }
                else if (role == "Doctor") {
                    doctorTarget = tgt;
                    cout << "  → 치료 대상: " << tgt << "\n";
                }
                else if (role == "Detective") {
                    detectiveTarget = tgt;
                    cout << "  → 조사 대상: " << tgt << "\n";
                }
                else { // Citizen or Amnesia-as-Citizen
                    detectiveTarget = tgt;
                    cout << "  → 투표 대상: " << tgt << "\n";
                }
                break;
            }
            catch (...) {
                cout << "  [잘못된 입력입니다]\n";
            }
        }
    }

    // 4) 마피아 득표 집계 (동표·0표 처리)
    int maxVote = 0;
    for (int i = 0; i < playerCount; ++i)
        if (mafiaVotes[i] > maxVote)
            maxVote = mafiaVotes[i];

    int numMax = 0;
    for (int i = 0; i < playerCount; ++i)
        if (mafiaVotes[i] == maxVote)
            numMax++;

    int killIdx = -1;
    if (maxVote > 0 && numMax == 1) {
        for (int i = 0; i < playerCount; ++i)
            if (mafiaVotes[i] == maxVote)
                killIdx = i;
    }

    // 5) 의사 보호 및 제거
    if (killIdx < 0) {
        cout << "[밤 결과] 아무도 공격받지 않았습니다.\n";
    }
    else if (killIdx == doctorTarget) {
        cout << "[밤 결과] 의사의 치료로 "
            << playerName[killIdx] << "님이 생존하였습니다.\n";
    }
    else {
        cout << "[밤 결과] " << playerName[killIdx]
            << "님이 마피아에게 제거되었습니다.\n";
        players[killIdx]->kill();
    }

    // 6) 경찰 조사 결과 저장 → 낮에 공개
    lastDetectiveTarget = detectiveTarget;


    // 7) 턴 증가 및 Amnesia 역할 섞기
    ++turn;
    if (turn % 2 == 0) {
        cout << "\n=== 기억상실 플레이어의 역할이 섞였습니다 ===\n";
        updateAmnesiaPlayers();
    }
}

void GameManager::runDayPhase() {
    const int totalSec = 3 * 60;
    std::cout << "\n===" << turn << "번째 아침이 밝았습니다.===\n"
        << "[생존자 목록]\n";

    if (turn != 1) {
        cout << "[밤 사이 사망한 플레이어]\n";
        bool anyDead = false;
        for (int i = 0; i < playerCount; ++i) {

            // '밤 시작 전 생존(lastAlive[i])' 이었고 '지금은 죽음(!isAlive())' 인 경우
            if (lastAlive[i] && !players[i]->isAlive()) {
                cout << " ? " << playerName[i]
                    << " (" << i << ")\n";
                anyDead = true;
            }
        }
        if (!anyDead) {
            cout << "아무도 죽지 않았습니다.\n";
        }
    }



    if (lastDetectiveTarget >= 0) {
        cout << "[경찰 정보] 어젯밤 경찰이 조사한 "
            << playerName[lastDetectiveTarget] << "님의 직업은 "
            << players[lastDetectiveTarget]->getRole() << "입니다.\n";
        // 한 번 공개 후 초기화
        lastDetectiveTarget = -1;
    }


    for (int i = 0;i < playerCount;++i)
        if (players[i]->isAlive())
            std::cout << " - " << playerName[i] << " (" << i << ")\n";


    std::cout << "명령어: V <내번호> <대상번호: -1:기권>, / I(목록) / Q(조기종료)\n";
    vote->resetVotes();

    int neededVotes = 0;
    for (int i = 0;i < playerCount;++i)
        if (players[i]->isAlive()) ++neededVotes;
    int votesCast = 0;
    int abstainCount = 0;
    //타이머
    for (int sec = totalSec;sec >= 0;--sec) {
        if (sec == 120)      std::cout << "\n>>> 2분 남았습니다! <<<\n";
        else if (sec == 60)       std::cout << "\n>>> 1분 남았습니다! <<<\n";
        else if (sec == 30)       std::cout << "\n>>> 30초 남았습니다! <<<\n";
        else if (sec == 10)       std::cout << "\n>>> 10초 남았습니다! <<<\n";

        if (sec <= 9) {
            std::cout << "\r남은 시간: " << setw(2) << sec << "초   " << flush;
        }
        else {
            int m = sec / 60, s = sec % 60;
            std::cout << "\r남은 시간: "
                << setw(2) << setfill('0') << m << ":"
                << setw(2) << setfill('0') << s
                << "   " << flush;
        }

        auto start = chrono::steady_clock::now();
        while (chrono::steady_clock::now() - start < chrono::seconds(1)) {
            int c;
            if (!_kbhit()) continue;
            c = _getch();
            if (std::cin.fail()) {
                std::cin.clear(); // 에러 플래그 초기화
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // 버퍼 비우기
                std::cout << "잘못된 입력입니다. 다시 시도하세요.\n";
                continue;
            }
            if (c == 81 or c == 113) { std::cout << "\n[낮을 조기 종료합니다]\n"; sec = -1; break; }
            if (c == 73 or c == 105) {
                std::cout << "\n[생존자 목록]\n";
                for (int i = 0;i < playerCount;++i)
                    if (players[i]->isAlive())
                        std::cout << " - " << playerName[i] << " (" << i << ")\n";
            }
            if (c == 86 or c == 118) {
                int from, to;
                if (cin >> from >> to) {
                    if (from >= 0 && from < playerCount && to >= 0 && to < playerCount && players[from]->isAlive()) {
                        if (to == -1) {
                            // 기권 처리
                            ++abstainCount;
                            ++votesCast;
                            cout << "\n[기권 등록되었습니다]\n";
                        }
                        else if (to >= 0 && to < playerCount) {
                            vote->vote(from, to);
                            ++votesCast;
                            cout << "\n[" << from << "번 투표 완료]\n";
                        }
                        else {
                            cout << "\n[잘못된 대상번호입니다]\n";
                        }

                        // 모든 플레이어(기권 포함)가 투표 완료 시 종료
                        if (votesCast >= neededVotes) {
                            cout << "[모든 투표가 완료되었습니다]\n";
                            sec = -1; break;
                        }
                    }
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                }
            }
            
            
        }
    }
    // 낮 투표시 제거될 사람
    int maxV = vote->getMaxVotes();
    bool tiePlayers = vote->hasTie(maxV);
    if (tiePlayers || abstainCount >= maxV) {
        cout << "[동점 또는 기권 다수] 아무도 제거되지 않습니다.\n";
    }
    else {
        int outIdx = vote->getMostVotedPlayer();
        cout << "[낮 결과] " << playerName[outIdx] << "님이 제거되었습니다.\n";
        players[outIdx]->kill();

        // 제거된 사람의 직업 공개
        std::string role = players[outIdx]->getRole();
        if (auto* ap = dynamic_cast<AmnesiaPlayer*>(players[outIdx])) {
            role = ap->getHiddenRole();
        }
        cout << "[낮 정보] " << playerName[outIdx]
            << "님의 직업은 \"" << role << "\"였습니다.\n";
    }

    // 낮 종료 시 생존 상태 갱신
    for (int i = 0; i < playerCount; ++i)
        lastAlive[i] = players[i]->isAlive();



    // 낮 종료 시 생존 상태 기록
    for (int i = 0;i < playerCount;++i)
        lastAlive[i] = players[i]->isAlive();
}

bool GameManager::checkWin() {
    int mafiaAlive = 0, citizenAlive = 0;
    for (int i = 0;i < playerCount;++i) {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getRole() == "Mafia") ++mafiaAlive;
        else ++citizenAlive;
    }
    if (mafiaAlive == 0) { std::cout << "[Citizens Win]\n"; return true; }
    if (mafiaAlive >= citizenAlive) { std::cout << "[Mafia Win]\n"; return true; }
    return false;
}

void GameManager::updateAmnesiaPlayers() {
    AmnesiaPlayer* amnesias[100];
    string roles[100];
    int count = 0;
    for (int i = 0;i < playerCount;++i) {
        auto* ap = dynamic_cast<AmnesiaPlayer*>(players[i]);
        if (ap && players[i]->isAlive()) {
            amnesias[count] = ap;
            roles[count] = ap->getHiddenRole();
            ++count;
        }
    }
    if (count < 2) return;
    for (int i = count - 1;i > 0;--i) {
        int j = rand() % (i + 1);
        swap(roles[i], roles[j]);
    }
    for (int i = 0;i < count;++i)
        amnesias[i]->setHiddenRole(roles[i]);
}
