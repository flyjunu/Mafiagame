#pragma once
#include "Player.h"
#include "VoteSystem.h"

// �ǻ� Ŭ����: �㿡 ġ�� ����� ��ǥ�� ����
class Doctor : public Player {
private:
    VoteSystem* nightVote; // �� ��ǥ ���� �ý���
    int myIndex;           // �ڽ��� �÷��̾� ��ȣ

public:
    Doctor(const std::string& name, int index, VoteSystem* voteSys);
    virtual ~Doctor();

    void nightAction() override;
    std::string getRole() const override;
};