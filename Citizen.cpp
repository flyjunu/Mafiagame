#include "Citizen.h"

Citizen::Citizen(const std::string& name)
    : Player(name) {}

Citizen::~Citizen() {}

void Citizen::nightAction() {
    // �ù��� �㿡 �ൿ ����
}

std::string Citizen::getRole() const {
    return "Citizen";
}