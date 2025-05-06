#pragma once
#include "Player.h"
#include "VoteSystem.h"

// ���Ǿ� Ŭ����: �㿡 ���� ����� ��ǥ�� ����
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