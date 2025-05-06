#include "VoteSystem.h"

VoteSystem::VoteSystem(int n)
    : numPlayers(n)
{
    voteCounts = new int[numPlayers];
    resetVotes();
}

VoteSystem::~VoteSystem() {
    delete[] voteCounts;
}

void VoteSystem::vote(int from, int to) {
    if (from < 0 || from >= numPlayers || to < 0 || to >= numPlayers) return;
    voteCounts[to]++;
}

int VoteSystem::getMostVotedPlayer() const {
    int maxVotes = -1, winner = -1;
    for (int i = 0; i < numPlayers; ++i) {
        if (voteCounts[i] > maxVotes) {
            maxVotes = voteCounts[i];
            winner = i;
        }
    }
    return winner;
}

void VoteSystem::resetVotes() {
    for (int i = 0; i < numPlayers; ++i)
        voteCounts[i] = 0;
}