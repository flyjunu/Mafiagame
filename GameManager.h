#pragma once
#include "Player.h"
#include "Mafia.h"
#include "Doctor.h"
#include "Detective.h"
#include "Citizen.h"
#include "AmnesiaPlayer.h"
#include "VoteSystem.h"


class GameManager {
    private:
        Player**     players;     // �÷��̾� ������ �迭
        int          playerCount; // �÷��̾� ��
        VoteSystem*  vote;        // ��ǥ �ý���
        int          turn;        // ���� ��
        string*      playerName;  // �÷��̾� �̸�
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