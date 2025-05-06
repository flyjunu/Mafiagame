#include "VoteSystem.h"

VoteSystem::VoteSystem(int n)
  : numPlayers(n), voteCounts(new int[n])
{
    resetVotes();
}

VoteSystem::~VoteSystem() {
    delete[] voteCounts;
}

void VoteSystem::vote(int from, int to) {
    if (from<0||from>=numPlayers||to<0||to>=numPlayers) return;
    voteCounts[to]++;
}

int VoteSystem::getMostVotedPlayer() const {
    int winner = -1, maxV = -1;
    for (int i = 0; i < numPlayers; ++i) {
        if (voteCounts[i] > maxV) {
            maxV = voteCounts[i];
            winner = i;
        }
    }
    return winner;
}

void VoteSystem::resetVotes() {
    for (int i = 0; i < numPlayers; ++i)
        voteCounts[i] = 0;
}

int VoteSystem::getMaxVotes() const {
    int maxV = 0;
    for (int i = 0; i < numPlayers; ++i)
        if (voteCounts[i] > maxV)
            maxV = voteCounts[i];
    return maxV;
}

bool VoteSystem::hasTie(int votes) const {
    int cnt = 0;
    for (int i = 0; i < numPlayers; ++i)
        if (voteCounts[i] == votes)
            cnt++;
    return cnt > 1;
}