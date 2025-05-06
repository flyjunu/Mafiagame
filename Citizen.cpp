#include "Citizen.h"

Citizen::Citizen(const std::string& name)
    : Player(name) {}

Citizen::~Citizen() {}

void Citizen::nightAction() {
    // ½Ã¹ÎÀº ¹ã¿¡ Çàµ¿ ¾øÀ½
}

std::string Citizen::getRole() const {
    return "Citizen";
}