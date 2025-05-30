#pragma once
#include "Player.h"
#include "VoteSystem.h"
#include "Citizen.h"
#include "Doctor.h"
#include "Detective.h"
#include "Mafia.h"
#include "AmnesiaPlayer.h"
#include "Agent.h"
#include "AscciArt.h"
#include <string>

struct GameSetting {
    int playerCount = 7;
    int mafia = 2;
    int doctor = 1;
    int detective = 1;
    int amnesia = 0;
    int agent = 1;
};

class GameManager {
    private:
        Player**     players;     // �÷��̾� ������ �迭
        int          playerCount; // �÷��̾� ��
        VoteSystem*  vote;        // ��ǥ �ý���
        int          turn;        // ���� ��
        std::string* playerName;  // �÷��̾� �̸�
        bool lastAlive[100];      // ��� �÷��̾� üũ    
        int lastDetectiveTarget;  // ������ ���� ������ �÷��̾� ��ȣȣ
        int mafiaCount;
        int doctorCount;
        int detectiveCount;
        int amnesiaCount;
        int agentCount;
        int lastDetectiveActor = -1; // ������ ������ �÷��̾� ��ȣ
        bool checkVoted[100] = {false}; //��ǥ���� �˻�
        int agentTarget[100] = { -1 };

    public:
        GameManager(const GameSetting& setting);             // ������: �÷��̾� �� �Է¡����������� �Ҵ�
        ~GameManager();            // �Ҹ���: �޸� ����
   
        void assignRoles();        // �÷��̾ �̸������� �Է� �� ��ü ����
        void runNightPhase();      // �� �ܰ�: ��� alive �÷��̾��� nightAction ȣ��
        void runDayPhase();        // �� �ܰ�: ��ǥ �� ���� �� vote �ʱ�ȭ
        bool checkWin();           // �¸� ���� �˻�
        void updateAmnesiaPlayers();// ???(FoggedPlayer) �� ������2�ϸ��� ���� ��ü
        
    };