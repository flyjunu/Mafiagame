#pragma once
#include "Player.h"

// �Ϲ� �ù� Ŭ����: �㿡 ���� �ൿ ����
class Citizen : public Player {
public:
    Citizen(const std::string& name);
    virtual ~Citizen();

    void nightAction() override;
    std::string getRole() const override;
};