#include "Player.h"

Player::Player(const std::string& name)
    : name(name), alive(true) {}

Player::~Player() {}

std::string Player::getName() const {
    return name;
}

bool Player::isAlive() const {
    return alive;
}

void Player::kill() {
    alive = false;
}