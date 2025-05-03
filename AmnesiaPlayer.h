#pragma once
#include "Player.h"
#include <string>
using namespace std;

class AmnesiaPlayer : public Player {
private:
    string hiddenRole;   // 실제 역할: "Doctor"/"Detective"/"Citizen"
public:
    //플레이어명, 실제 직업명
    AmnesiaPlayer(string& name, string& initialHiddenRole);
    virtual ~AmnesiaPlayer();

    void setHiddenRole(const string& newRole);
    string getHiddenRole() const;

    void nightAction() override;     // 밤 행동 (hiddenRole 기반)
    string getRole() override; // 항상 "???"
};