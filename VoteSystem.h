#pragma once

// 투표 집계 시스템
class VoteSystem {
private:
    int numPlayers;   // 플레이어 수
    int* voteCounts;  // 각 플레이어가 받은 표 수

public:
    VoteSystem(int n);
    ~VoteSystem();

    // from 플레이어가 to 플레이어에게 투표
    void vote(int from, int to);

    // 가장 많은 표를 받은 플레이어 번호 반환
    int getMostVotedPlayer() const;

    // 투표 집계 초기화
    void resetVotes();
};