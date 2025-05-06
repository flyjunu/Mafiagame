#pragma once
#include "Player.h"
#include "VoteSystem.h"
#include "Citizen.h"
#include "Doctor.h"
#include "Detective.h"
#include "Mafia.h"
#include "AmnesiaPlayer.h"
#include <string>


class GameManager {
    private:
        Player**     players;     // �÷��̾� ������ �迭
        int          playerCount; // �÷��̾� ��
        VoteSystem*  vote;        // ��ǥ �ý���
        int          turn;        // ���� ��
        std::string* playerName;  // �÷��̾� �̸�
        bool lastAlive[100];      // ��� �÷��̾� üũ    

    public:
        GameManager();             // ������: �÷��̾� �� �Է¡����������� �Ҵ�
        ~GameManager();            // �Ҹ���: �޸� ����
    
        void assignRoles();        // �÷��̾ �̸������� �Է� �� ��ü ����
        void runNightPhase();      // �� �ܰ�: ��� alive �÷��̾��� nightAction ȣ��
        void runDayPhase();        // �� �ܰ�: ��ǥ �� ���� �� vote �ʱ�ȭ
        bool checkWin();           // �¸� ���� �˻�
        void updateAmnesiaPlayers();// ???(FoggedPlayer) �� ������2�ϸ��� ���� ��ü
    };