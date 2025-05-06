#pragma once

// ��ǥ ���� �ý���
class VoteSystem {
private:
    int numPlayers;   // �÷��̾� ��
    int* voteCounts;  // �� �÷��̾ ���� ǥ ��

public:
    VoteSystem(int n);
    ~VoteSystem();

    // from �÷��̾ to �÷��̾�� ��ǥ
    void vote(int from, int to);

    // ���� ���� ǥ�� ���� �÷��̾� ��ȣ ��ȯ
    int getMostVotedPlayer() const;

    // ��ǥ ���� �ʱ�ȭ
    void resetVotes();

    // �÷��̾ �ִ�(��ǥ��)�� ��ȯ
    int  getMaxVotes() const;
    // Ư�� ��ǥ���� ���� ��ǥ�� �ִ��� �˻�
    bool hasTie(int votes) const;
};