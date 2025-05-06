#pragma once
#include "Player.h"
#include "VoteSystem.h"

// AmnesiaPlayer: 자신의 역할을 숨기고, hiddenRole에 따라 행동
class AmnesiaPlayer : public Player {
private:
    std::string hiddenRole;
    VoteSystem* nightVote;
    int myIndex;

public:
    AmnesiaPlayer(const std::string& name,
                  const std::string& initialHiddenRole,
                  int index, VoteSystem* voteSys);
    virtual ~AmnesiaPlayer();

    void setHiddenRole(const std::string& newRole);
    std::string getHiddenRole() const;

    void nightAction() override;
    std::string getRole() const override;
};