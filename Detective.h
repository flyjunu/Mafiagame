#pragma once
#include "Player.h"
#include "VoteSystem.h"

// 경찰 클래스: 밤에 조사 대상을 투표로 결정
class Detective : public Player {
private:
    VoteSystem* nightVote;
    int myIndex;

public:
    Detective(const std::string& name, int index, VoteSystem* voteSys);
    virtual ~Detective();

    void nightAction() override;
    std::string getRole() const override;
};