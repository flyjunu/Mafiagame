#pragma once
#include "Player.h"
#include "VoteSystem.h"

// ���� Ŭ����: �㿡 ���� ����� ��ǥ�� ����
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