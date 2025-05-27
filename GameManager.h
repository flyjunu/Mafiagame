#pragma once
#include "Player.h"
#include "VoteSystem.h"
#include "Citizen.h"
#include "Doctor.h"
#include "Detective.h"
#include "Mafia.h"
#include "AmnesiaPlayer.h"
#include <string>

struct GameSetting {
    int playerCount = 7;
    int mafia = 2;
    int doctor = 1;
    int detective = 1;
    int amnesia = 0;
};

class GameManager {
    private:
        Player**     players;     // 플레이어 포인터 배열
        int          playerCount; // 플레이어 수
        VoteSystem*  vote;        // 투표 시스템
        int          turn;        // 현재 턴
        std::string* playerName;  // 플레이어 이름
        bool lastAlive[100];      // 사망 플레이어 체크    
        int lastDetectiveTarget;  // 경찰이 전날 조사한 플레이어 번호호
        int mafiaCount;
        int doctorCount;
        int detectiveCount;
        int amnesiaCount;
        int lastDetectiveActor = -1; // 경찰이 조사한 플레이어 번호
        bool checkVoted[100] = {false}; //투표여부 검사

    public:
        GameManager(const GameSetting& setting);             // 생성자: 플레이어 수 입력·생성·역할 할당
        ~GameManager();            // 소멸자: 메모리 해제
   
        void assignRoles();        // 플레이어별 이름·역할 입력 및 객체 생성
        void runNightPhase();      // 밤 단계: 모든 alive 플레이어의 nightAction 호출
        void runDayPhase();        // 낮 단계: 투표 → 제거 → vote 초기화
        bool checkWin();           // 승리 조건 검사
        void updateAmnesiaPlayers();// ???(FoggedPlayer) 턴 증가·2턴마다 역할 교체
        
    };