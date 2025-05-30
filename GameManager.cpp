
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

GameManager::GameManager(const GameSetting& setting) {
    playerCount = setting.playerCount;
    mafiaCount = setting.mafia;
    doctorCount = setting.doctor;
    detectiveCount = setting.detective;
    amnesiaCount = setting.amnesia;
    agentCount = setting.agent;
    
    std::cout << "마피아 게임에 오신 여러분을 환영합니다. " << std::endl;
    
    std::cout << "플레이어 수: " << playerCount << ", 마피아: " << mafiaCount
        << ", 의사: " << doctorCount << ", 경찰: " << detectiveCount
        << ", 공작원: " << agentCount << ", ???: " << amnesiaCount <<", \n\n";
    
    playerName = new string[playerCount];
    players = new Player * [playerCount];
    vote = new VoteSystem(playerCount);
    turn = 1;
    lastDetectiveTarget = -1;
    for (int i = 0; i < playerCount; ++i) {
        std::cout << i+1 << "번 플레이어 이름: ";
        getline(cin, playerName[i]);
    }

    // 플레이어 이름 입력한 순서와 다르게 섞기
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(playerName[i], playerName[j]);
    }
    // 플레이어 역할 섞기(+최초 역할 부여)
    assignRoles();

    std::cout << "\n[플레이어 이름 : 번호]\n";
    for (int i = 0; i < playerCount; ++i) {
        std::string role = players[i]->getRole();
        if (role == "Mafia") cout << getMafiaArt();
        else if (role == "Doctor") cout << getDoctorArt();
        else if (role == "Detective") cout << getDetectiveArt();
        else if (role == "Agent") cout << getAgentArt();
        else if (role == "Citizen") cout << getCitizenArt();
        else if (role == "???") cout << getAmnesiaArt();
        else cout << "Unknown Role";   
     

        std::cout << playerName[i] << "님의 번호 : " << i+1 << "번, 직업:"<< role <<"\n";
        lastAlive[i] = true;

        std::cout << "\n다음 플레이어를 보려면 엔터를 누르세요...";
        while (_getch() != 13); // 엔터(ASCII 13) 입력시만 다음으로
            system("cls"); // (옵션) 다음 정보 전 화면 지우기

    }
    // agentTarget 초기화
    for (int i = 0; i < 100; ++i) agentTarget[i] = -1;
    
}

GameManager::~GameManager() {
    delete vote;
    for (int i = 0; i < playerCount; ++i)
        delete players[i];
    delete[] players;
    delete[] playerName;
}

void GameManager::assignRoles() {
    int citizenCount = playerCount - (mafiaCount + doctorCount + detectiveCount+ agentCount);

    string roles[100];
    int idx = 0;
    for (int i = 0; i < mafiaCount; ++i) roles[idx++] = "Mafia";
    for (int i = 0; i < doctorCount; ++i) roles[idx++] = "Doctor";
    for (int i = 0; i < detectiveCount; ++i) roles[idx++] = "Detective";
    for (int i = 0; i < citizenCount; ++i) roles[idx++] = "Citizen";
    for (int i = 0; i < agentCount; ++i) roles[idx++] = "Agent";

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
        else if (rl == "Agent")
            players[i] = new Agent(nm, i, vote);
        else
            players[i] = new Citizen(nm);
    }

    std::cout << "\n[개발자 옵션: 섞인 역할 확인]\n";
    for (int i = 0; i < playerCount; ++i) {
        std::cout << "  " << playerName[i] << " (" << i+1 << ") => ";
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

    std::cout << "\n===" << turn << "번째 밤이 시작됩니다. 마피아들은 회의를 진행해 주십시오.(남은시간 2:00) ===\n"
        << "[I] 생존자 목록  [Q] 조기종료\n";

    for (int sec = totalSec; sec >= 0; --sec) {
        int m = sec / 60, s = sec % 60;
        std::cout << "\r남은 시간: "
            << setw(2) << setfill('0') << m << ":"
            << setw(2) << setfill('0') << s
            << "   " << std::flush;

        auto start = steady_clock::now();
        auto end = start + seconds(1);
        while (steady_clock::now() < end) {
            if (_kbhit()) {
                char c = toupper(_getch());
                if (c == 'Q') {
                    earlyEnd = true;
                    break;
                }
                if (c == 'I') {
                    cout << "\n[생존자 목록]\n";
                    for (int i = 0; i < playerCount; ++i)
                        if (players[i]->isAlive())
                            cout << " - " << playerName[i] << " (" << i+1 << ")\n";
                }
            }
            this_thread::sleep_for(milliseconds(50));
        }
        if (earlyEnd) break;
    }
    
    // 2) 밤 행동 등록 준비
    cout << "\n\n=== 모든 직업의 밤 행동을 등록합니다 ===\n";
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
        if (players[i]->isAlive() && players[i]->getRole() == "Agent")
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
            cin.clear();
            std::getline(std::cin, line);

            // I → 목록
            if (line == "I" || line == "i") {
                cout << "[생존자 목록]\n";
                for (int j = 0; j < playerCount; ++j)
                    if (players[j]->isAlive())
                        cout << " - " << playerName[j]
                        << " (" << j+1 << ")\n";
                continue;
            }

            // 숫자 파싱
            try {
                int tgt = std::stoi(line) -1;
                if (tgt < 0 || tgt >= playerCount)
                    throw std::out_of_range("공격 대상을 잘못 입력하였습니다.");
                if (!players[tgt]->isAlive() && role == "Mafia") {
                    // 마피아는 살아있는 대상만 공격 가능
                    cout << "  [해당 번호는 생존자가 아닙니다]\n";
                    continue;
                }
                // 역할별 처리
                if (role == "Mafia") {
                    mafiaVotes[tgt]++;
                    cout << "  → 공격 대상: " << tgt+1 << endl;
                }
                else if (role == "Doctor") {
                    doctorTarget = tgt;
                    cout << "  → 치료 대상: " << tgt+1 << endl;
                }
                else if (role == "Detective") {
                    detectiveTarget = tgt;
                    lastDetectiveActor = actor;  
                    cout << "  → 조사 대상: " << tgt+1 << endl;
                }
                else if (role == "Agent") {
                        agentTarget[actor] = tgt; // agentTarget[공작원인덱스] = 선택대상
                        cout << "  → 기권 대상: " << tgt+1 << endl;
                }
                else { // Citizen or Amnesia-as-Citizen
                    detectiveTarget = tgt;
                    cout << "  → 투표 대상: " << tgt+1 << endl;
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
    if (turn % 2 == 0 && amnesiaCount> 1) {
        cout << "\n===기억상실 플레이어의 역할이 섞였습니다 ===\n";
        updateAmnesiaPlayers();
    }
}

void GameManager::runDayPhase() {
    const int totalSec = 3 * 60;
    std::cout << "\n===" << turn << "번째 아침이 밝았습니다.===\n";

    if (turn != 1) {
        cout << "[밤 사이 사망한 플레이어]\n";
        bool anyDead = false;
        for (int i = 0; i < playerCount; ++i) {

            // '밤 시작 전 생존(lastAlive[i])' 이었고 '지금은 죽음(!isAlive())' 인 경우
            if (lastAlive[i] && !players[i]->isAlive()) {
                cout <<  playerName[i]
                    << " (" << i+1 << ")\n";
                anyDead = true;
            }
        }
        if (!anyDead) {
            cout << "아무도 죽지 않았습니다.\n";
        }
    }
    // 어젯밤에 경찰이 살아 있었는지 확인하기.
    if (lastDetectiveTarget >= 0 
        && lastDetectiveActor >= 0 
        && players[lastDetectiveActor]->isAlive()) 
    {
        cout << "[경찰 조사] 어젯밤 경찰이 조사한 " << playerName[lastDetectiveTarget]
            << "님의 직업은 " << players[lastDetectiveTarget]->getRole() 
            << "입니다.\n";
        lastDetectiveTarget = -1;
        lastDetectiveActor  = -1;  // 초기화
    }

    for (int i = 0; i < playerCount; ++i) {
    // agentTarget[i]에 기록된 값이 있으면, 해당 플레이어 skipNextVote 설정
    if (players[i]->getRole() == "Agent" && players[i]->isAlive()) {
        int tgt = agentTarget[i];
        if (tgt >= 0 && players[tgt]->isAlive()) {
            players[tgt]->setSkipNextVote(true);
            cout << "[공작원의 투표함 조작] " << playerName[tgt] << "님은 이번 낮 투표를 기권합니다.\n";
        }
        agentTarget[i] = -1; // 초기화
    }
}

    for (int i = 0;i < playerCount;++i)
        if (players[i]->isAlive())
            std::cout << " - " << playerName[i] << " (" << i+1 << ")\n";


    std::cout << "명령어: V <내번호> <대상번호: -1:기권>, / I(목록) / Q(조기종료)\n";
    vote->resetVotes();

    int neededVotes = 0;
    for (int i = 0;i < playerCount;++i)
        if (players[i]->isAlive()) ++neededVotes;
    int votesCast = 0;
    int abstainCount = 0;
    int checkVoted[100] = { false }; // 투표 여부 검사
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
            if (c == 81 || c == 113) { std::cout << "\n[낮을 조기 종료합니다]\n"; sec = -1; break; }
            if (c == 73 || c == 105) {
                std::cout << "\n[생존자 목록]\n";
                for (int i = 0;i < playerCount;++i)
                    if (players[i]->isAlive())
                        std::cout << " - " << playerName[i] << " (" << i+1 << ")\n";
            }
            if (c == 86 || c == 118) {
                int from, to;
                if (!(cin >> from >> to)) {
                    cout << "[잘못된 입력입니다]\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                from--;
                //기권대상처리
                if(to != -1) to--;
                
                if (from >= 0 && from < playerCount && players[from]->isAlive()) {
                    if (players[from]->shouldSkipNextVote()) {
                        cout << "\n[공작원 효과] " << playerName[from] << "님은 자동 기권 처리됩니다.\n";
                        ++abstainCount;
                        ++votesCast;
                        checkVoted[from] = true;
                        players[from]->setSkipNextVote(false); // 한 번만 적용
                        continue;
                    }
                    if (checkVoted[from]) {
                        cout << "\n[이미 투표를 완료한 플레이어입니다.]\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    
                    if (to == -1) {
                        // 기권 처리
                        ++abstainCount;
                        ++votesCast;
                        checkVoted[from] = true;
                        cout << "\n[기권 등록되었습니다]\n";
                    }
                    else if (to >= 0 && to < playerCount) {
                        vote->vote(from, to);
                        ++votesCast;
                        checkVoted[from] = true;
                        cout << "\n[" << from+1 << "번 투표 완료]\n";
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
                else {
                    cout << "\n[이미 사망한 투표자의 투표 입니다.]\n";
                }
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
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

}

bool GameManager::checkWin() {
    int mafiaAlive = 0, citizenAlive = 0;;
    for (int i = 0;i < playerCount;++i) {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getRole() == "Mafia") ++mafiaAlive;
        //amnesia플레이어도, 시민으로 취급하여, 승리조건에 추가
        else 
            ++citizenAlive;
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
