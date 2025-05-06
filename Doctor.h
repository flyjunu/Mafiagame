#pragma once
#include "Player.h"
#include "VoteSystem.h"

// 의사 클래스: 밤에 치료 대상을 투표로 결정
class Doctor : public Player {
private:
    VoteSystem* nightVote; // 밤 투표 집계 시스템
    int myIndex;           // 자신의 플레이어 번호

public:
    Doctor(const std::string& name, int index, VoteSystem* voteSys);
    virtual ~Doctor();

    void nightAction() override;
    std::string getRole() const override;
};