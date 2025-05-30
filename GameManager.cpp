
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
    
    std::cout << "���Ǿ� ���ӿ� ���� �������� ȯ���մϴ�. " << std::endl;
    
    std::cout << "�÷��̾� ��: " << playerCount << ", ���Ǿ�: " << mafiaCount
        << ", �ǻ�: " << doctorCount << ", ����: " << detectiveCount
        << ", ���ۿ�: " << agentCount << ", ???: " << amnesiaCount <<", \n\n";
    
    playerName = new string[playerCount];
    players = new Player * [playerCount];
    vote = new VoteSystem(playerCount);
    turn = 1;
    lastDetectiveTarget = -1;
    for (int i = 0; i < playerCount; ++i) {
        std::cout << i+1 << "�� �÷��̾� �̸�: ";
        getline(cin, playerName[i]);
    }

    // �÷��̾� �̸� �Է��� ������ �ٸ��� ����
    srand(static_cast<unsigned>(time(nullptr)));
    for (int i = playerCount - 1; i > 0; --i) {
        int j = rand() % (i + 1);
        swap(playerName[i], playerName[j]);
    }
    // �÷��̾� ���� ����(+���� ���� �ο�)
    assignRoles();

    std::cout << "\n[�÷��̾� �̸� : ��ȣ]\n";
    for (int i = 0; i < playerCount; ++i) {
        std::string role = players[i]->getRole();
        if (role == "Mafia") cout << getMafiaArt();
        else if (role == "Doctor") cout << getDoctorArt();
        else if (role == "Detective") cout << getDetectiveArt();
        else if (role == "Agent") cout << getAgentArt();
        else if (role == "Citizen") cout << getCitizenArt();
        else if (role == "???") cout << getAmnesiaArt();
        else cout << "Unknown Role";   
     

        std::cout << playerName[i] << "���� ��ȣ : " << i+1 << "��, ����:"<< role <<"\n";
        lastAlive[i] = true;

        std::cout << "\n���� �÷��̾ ������ ���͸� ��������...";
        while (_getch() != 13); // ����(ASCII 13) �Է½ø� ��������
            system("cls"); // (�ɼ�) ���� ���� �� ȭ�� �����

    }
    // agentTarget �ʱ�ȭ
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

    std::cout << "\n[������ �ɼ�: ���� ���� Ȯ��]\n";
    for (int i = 0; i < playerCount; ++i) {
        std::cout << "  " << playerName[i] << " (" << i+1 << ") => ";
        if (amnesiaIdx[i]) {
            auto* ap = static_cast<AmnesiaPlayer*>(players[i]);
            std::cout << "???  (����: " << ap->getHiddenRole() << ")\n";
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

    std::cout << "\n===" << turn << "��° ���� ���۵˴ϴ�. ���ǾƵ��� ȸ�Ǹ� ������ �ֽʽÿ�.(�����ð� 2:00) ===\n"
        << "[I] ������ ���  [Q] ��������\n";

    for (int sec = totalSec; sec >= 0; --sec) {
        int m = sec / 60, s = sec % 60;
        std::cout << "\r���� �ð�: "
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
                    cout << "\n[������ ���]\n";
                    for (int i = 0; i < playerCount; ++i)
                        if (players[i]->isAlive())
                            cout << " - " << playerName[i] << " (" << i+1 << ")\n";
                }
            }
            this_thread::sleep_for(milliseconds(50));
        }
        if (earlyEnd) break;
    }
    
    // 2) �� �ൿ ��� �غ�
    cout << "\n\n=== ��� ������ �� �ൿ�� ����մϴ� ===\n";
    vote->resetVotes();
    int doctorTarget = -1, detectiveTarget = -1;
    int mafiaVotes[100] = { 0 };

    // actor ����: Mafia �� Doctor �� Detective �� Amnesia
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

    // 3) �� actor�� ��� �Է� (���ڸ�)
    for (int idx = 0; idx < orderCount; ++idx) {
        int actor = actorOrder[idx];
        // ���� ����(Amnesia�� hiddenRole)
        std::string role = players[actor]->getRole();
        if (role == "???") {
            auto* ap = dynamic_cast<AmnesiaPlayer*>(players[actor]);
            role = ap->getHiddenRole();
        }
        
        // �ݺ��ؼ� ��ȿ �Է� ���� ������
        while (true) {
            cout << "[" << role << "] " << playerName[actor]
                << "��, ��� ��ȣ�� �Է��ϼ��� (I:���): ";
            std::string line;
            cin.clear();
            std::getline(std::cin, line);

            // I �� ���
            if (line == "I" || line == "i") {
                cout << "[������ ���]\n";
                for (int j = 0; j < playerCount; ++j)
                    if (players[j]->isAlive())
                        cout << " - " << playerName[j]
                        << " (" << j+1 << ")\n";
                continue;
            }

            // ���� �Ľ�
            try {
                int tgt = std::stoi(line) -1;
                if (tgt < 0 || tgt >= playerCount)
                    throw std::out_of_range("���� ����� �߸� �Է��Ͽ����ϴ�.");
                if (!players[tgt]->isAlive() && role == "Mafia") {
                    // ���Ǿƴ� ����ִ� ��� ���� ����
                    cout << "  [�ش� ��ȣ�� �����ڰ� �ƴմϴ�]\n";
                    continue;
                }
                // ���Һ� ó��
                if (role == "Mafia") {
                    mafiaVotes[tgt]++;
                    cout << "  �� ���� ���: " << tgt+1 << endl;
                }
                else if (role == "Doctor") {
                    doctorTarget = tgt;
                    cout << "  �� ġ�� ���: " << tgt+1 << endl;
                }
                else if (role == "Detective") {
                    detectiveTarget = tgt;
                    lastDetectiveActor = actor;  
                    cout << "  �� ���� ���: " << tgt+1 << endl;
                }
                else if (role == "Agent") {
                        agentTarget[actor] = tgt; // agentTarget[���ۿ��ε���] = ���ô��
                        cout << "  �� ��� ���: " << tgt+1 << endl;
                }
                else { // Citizen or Amnesia-as-Citizen
                    detectiveTarget = tgt;
                    cout << "  �� ��ǥ ���: " << tgt+1 << endl;
                }
                break;
            }
            catch (...) {
                cout << "  [�߸��� �Է��Դϴ�]\n";
            }
        }
    }

    // 4) ���Ǿ� ��ǥ ���� (��ǥ��0ǥ ó��)
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

    // 5) �ǻ� ��ȣ �� ����
    if (killIdx < 0) {
        cout << "[�� ���] �ƹ��� ���ݹ��� �ʾҽ��ϴ�.\n";
    }
    else if (killIdx == doctorTarget) {
        cout << "[�� ���] �ǻ��� ġ��� "
            << playerName[killIdx] << "���� �����Ͽ����ϴ�.\n";
    }
    else {
        cout << "[�� ���] " << playerName[killIdx]
            << "���� ���Ǿƿ��� ���ŵǾ����ϴ�.\n";
        players[killIdx]->kill();
    }

    // 6) ���� ���� ��� ���� �� ���� ����
    lastDetectiveTarget = detectiveTarget;

    // 7) �� ���� �� Amnesia ���� ����
    ++turn;
    if (turn % 2 == 0 && amnesiaCount> 1) {
        cout << "\n===����� �÷��̾��� ������ �������ϴ� ===\n";
        updateAmnesiaPlayers();
    }
}

void GameManager::runDayPhase() {
    const int totalSec = 3 * 60;
    std::cout << "\n===" << turn << "��° ��ħ�� ��ҽ��ϴ�.===\n";

    if (turn != 1) {
        cout << "[�� ���� ����� �÷��̾�]\n";
        bool anyDead = false;
        for (int i = 0; i < playerCount; ++i) {

            // '�� ���� �� ����(lastAlive[i])' �̾��� '������ ����(!isAlive())' �� ���
            if (lastAlive[i] && !players[i]->isAlive()) {
                cout <<  playerName[i]
                    << " (" << i+1 << ")\n";
                anyDead = true;
            }
        }
        if (!anyDead) {
            cout << "�ƹ��� ���� �ʾҽ��ϴ�.\n";
        }
    }
    // �����㿡 ������ ��� �־����� Ȯ���ϱ�.
    if (lastDetectiveTarget >= 0 
        && lastDetectiveActor >= 0 
        && players[lastDetectiveActor]->isAlive()) 
    {
        cout << "[���� ����] ������ ������ ������ " << playerName[lastDetectiveTarget]
            << "���� ������ " << players[lastDetectiveTarget]->getRole() 
            << "�Դϴ�.\n";
        lastDetectiveTarget = -1;
        lastDetectiveActor  = -1;  // �ʱ�ȭ
    }

    for (int i = 0; i < playerCount; ++i) {
    // agentTarget[i]�� ��ϵ� ���� ������, �ش� �÷��̾� skipNextVote ����
    if (players[i]->getRole() == "Agent" && players[i]->isAlive()) {
        int tgt = agentTarget[i];
        if (tgt >= 0 && players[tgt]->isAlive()) {
            players[tgt]->setSkipNextVote(true);
            cout << "[���ۿ��� ��ǥ�� ����] " << playerName[tgt] << "���� �̹� �� ��ǥ�� ����մϴ�.\n";
        }
        agentTarget[i] = -1; // �ʱ�ȭ
    }
}

    for (int i = 0;i < playerCount;++i)
        if (players[i]->isAlive())
            std::cout << " - " << playerName[i] << " (" << i+1 << ")\n";


    std::cout << "��ɾ�: V <����ȣ> <����ȣ: -1:���>, / I(���) / Q(��������)\n";
    vote->resetVotes();

    int neededVotes = 0;
    for (int i = 0;i < playerCount;++i)
        if (players[i]->isAlive()) ++neededVotes;
    int votesCast = 0;
    int abstainCount = 0;
    int checkVoted[100] = { false }; // ��ǥ ���� �˻�
    //Ÿ�̸�
    for (int sec = totalSec;sec >= 0;--sec) {
        if (sec == 120)      std::cout << "\n>>> 2�� ���ҽ��ϴ�! <<<\n";
        else if (sec == 60)       std::cout << "\n>>> 1�� ���ҽ��ϴ�! <<<\n";
        else if (sec == 30)       std::cout << "\n>>> 30�� ���ҽ��ϴ�! <<<\n";
        else if (sec == 10)       std::cout << "\n>>> 10�� ���ҽ��ϴ�! <<<\n";

        if (sec <= 9) {
            std::cout << "\r���� �ð�: " << setw(2) << sec << "��   " << flush;
        }
        else {
            int m = sec / 60, s = sec % 60;
            std::cout << "\r���� �ð�: "
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
                std::cin.clear(); // ���� �÷��� �ʱ�ȭ
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // ���� ����
                std::cout << "�߸��� �Է��Դϴ�. �ٽ� �õ��ϼ���.\n";
                continue;
            }
            if (c == 81 || c == 113) { std::cout << "\n[���� ���� �����մϴ�]\n"; sec = -1; break; }
            if (c == 73 || c == 105) {
                std::cout << "\n[������ ���]\n";
                for (int i = 0;i < playerCount;++i)
                    if (players[i]->isAlive())
                        std::cout << " - " << playerName[i] << " (" << i+1 << ")\n";
            }
            if (c == 86 || c == 118) {
                int from, to;
                if (!(cin >> from >> to)) {
                    cout << "[�߸��� �Է��Դϴ�]\n";
                    cin.clear();
                    cin.ignore(numeric_limits<streamsize>::max(), '\n');
                    continue;
                }
                from--;
                //��Ǵ��ó��
                if(to != -1) to--;
                
                if (from >= 0 && from < playerCount && players[from]->isAlive()) {
                    if (players[from]->shouldSkipNextVote()) {
                        cout << "\n[���ۿ� ȿ��] " << playerName[from] << "���� �ڵ� ��� ó���˴ϴ�.\n";
                        ++abstainCount;
                        ++votesCast;
                        checkVoted[from] = true;
                        players[from]->setSkipNextVote(false); // �� ���� ����
                        continue;
                    }
                    if (checkVoted[from]) {
                        cout << "\n[�̹� ��ǥ�� �Ϸ��� �÷��̾��Դϴ�.]\n";
                        cin.clear();
                        cin.ignore(numeric_limits<streamsize>::max(), '\n');
                        continue;
                    }
                    
                    if (to == -1) {
                        // ��� ó��
                        ++abstainCount;
                        ++votesCast;
                        checkVoted[from] = true;
                        cout << "\n[��� ��ϵǾ����ϴ�]\n";
                    }
                    else if (to >= 0 && to < playerCount) {
                        vote->vote(from, to);
                        ++votesCast;
                        checkVoted[from] = true;
                        cout << "\n[" << from+1 << "�� ��ǥ �Ϸ�]\n";
                    }
                    else {
                        cout << "\n[�߸��� ����ȣ�Դϴ�]\n";
                    }

                    // ��� �÷��̾�(��� ����)�� ��ǥ �Ϸ� �� ����
                    if (votesCast >= neededVotes) {
                        cout << "[��� ��ǥ�� �Ϸ�Ǿ����ϴ�]\n";
                        sec = -1; break;
                    }
                }
                else {
                    cout << "\n[�̹� ����� ��ǥ���� ��ǥ �Դϴ�.]\n";
                }
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
            }            
        }
    }
    // �� ��ǥ�� ���ŵ� ���
    int maxV = vote->getMaxVotes();
    bool tiePlayers = vote->hasTie(maxV);
    if (tiePlayers || abstainCount >= maxV) {
        cout << "[���� �Ǵ� ��� �ټ�] �ƹ��� ���ŵ��� �ʽ��ϴ�.\n";
    }
    else {
        int outIdx = vote->getMostVotedPlayer();
        cout << "[�� ���] " << playerName[outIdx] << "���� ���ŵǾ����ϴ�.\n";
        players[outIdx]->kill();

        // ���ŵ� ����� ���� ����
        std::string role = players[outIdx]->getRole();
        if (auto* ap = dynamic_cast<AmnesiaPlayer*>(players[outIdx])) {
            role = ap->getHiddenRole();
        }
        cout << "[�� ����] " << playerName[outIdx]
            << "���� ������ \"" << role << "\"�����ϴ�.\n";
    }

    // �� ���� �� ���� ���� ����
    for (int i = 0; i < playerCount; ++i)
        lastAlive[i] = players[i]->isAlive();

}

bool GameManager::checkWin() {
    int mafiaAlive = 0, citizenAlive = 0;;
    for (int i = 0;i < playerCount;++i) {
        if (!players[i]->isAlive()) continue;
        if (players[i]->getRole() == "Mafia") ++mafiaAlive;
        //amnesia�÷��̾, �ù����� ����Ͽ�, �¸����ǿ� �߰�
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
