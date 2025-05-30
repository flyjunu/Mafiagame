#pragma once
#include "Player.h"
#include "VoteSystem.h"

class Agent : public Player {
private:
    VoteSystem* nightVote;
    int myIndex;

public:
    Agent(const std::string& name, int index, VoteSystem* voteSys);
    virtual ~Agent();
    void nightAction() override;
    std::string getRole() const override;

};