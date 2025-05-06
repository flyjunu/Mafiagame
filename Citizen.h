#pragma once
#include "Player.h"

// 일반 시민 클래스: 밤에 별도 행동 없음
class Citizen : public Player {
public:
    Citizen(const std::string& name);
    virtual ~Citizen();

    void nightAction() override;
    std::string getRole() const override;
};