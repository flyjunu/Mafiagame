#pragma once
#include <string>

// �߻�ȭ�� �÷��̾� �⺻ Ŭ����
class Player {
protected:
    std::string name;  // �÷��̾� �̸�
    bool alive;        // ���� ����
    bool skipNextVote = false; // ���� ��ǥ�� �ǳʶ��� ����
public:
    Player(const std::string& name);
    virtual ~Player();

    std::string getName() const;
    bool isAlive() const;
    void kill();

    // �� �ൿ: �Ļ� Ŭ�������� ��ü ���� ����
    virtual void nightAction() = 0;
    // ���Ҹ� ��ȯ: �Ļ� Ŭ�������� "Mafia", "Doctor" ���� ��ȯ
    virtual std::string getRole() const = 0;

    void setSkipNextVote(bool v) { skipNextVote = v; }
    bool shouldSkipNextVote() const { return skipNextVote; }
};

