#pragma once
#include "Player.h"
#include "VoteSystem.h"

// 마피아 클래스: 밤에 공격 대상을 투표로 결정
class Mafia : public Player {
private:
    VoteSystem* nightVote;
    int myIndex;

public:
    Mafia(const std::string& name, int index, VoteSystem* voteSys);
    virtual ~Mafia();

    void nightAction() override;
    std::string getRole() const override;
};