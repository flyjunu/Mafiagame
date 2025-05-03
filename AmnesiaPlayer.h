#pragma once
#include "Player.h"
#include <string>
using namespace std;

class AmnesiaPlayer : public Player {
private:
    string hiddenRole;   // ���� ����: "Doctor"/"Detective"/"Citizen"
public:
    //�÷��̾��, ���� ������
    AmnesiaPlayer(string& name, string& initialHiddenRole);
    virtual ~AmnesiaPlayer();

    void setHiddenRole(const string& newRole);
    string getHiddenRole() const;

    void nightAction() override;     // �� �ൿ (hiddenRole ���)
    string getRole() override; // �׻� "???"
};