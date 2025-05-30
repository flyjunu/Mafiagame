#pragma once
#include <string>

// 추상화된 플레이어 기본 클래스
class Player {
protected:
    std::string name;  // 플레이어 이름
    bool alive;        // 생존 여부
    bool skipNextVote = false; // 다음 투표를 건너뛸지 여부
public:
    Player(const std::string& name);
    virtual ~Player();

    std::string getName() const;
    bool isAlive() const;
    void kill();

    // 밤 행동: 파생 클래스에서 구체 로직 구현
    virtual void nightAction() = 0;
    // 역할명 반환: 파생 클래스에서 "Mafia", "Doctor" 등을 반환
    virtual std::string getRole() const = 0;

    void setSkipNextVote(bool v) { skipNextVote = v; }
    bool shouldSkipNextVote() const { return skipNextVote; }
};

