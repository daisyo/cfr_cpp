#include <iostream>
#include <vector>
#include <random>
#include <map>
#include <unordered_map>
#include <cassert>

using namespace std;

const int PASS = 0, BET = 1, NUM_ACTIONS = 2;

random_device rnd;
default_random_engine engine(rnd());

struct Node {
    string infoSet;
    vector<double> regretSum, strategy, strategySum;

    Node() : infoSet(""), regretSum(NUM_ACTIONS, 0), strategy(NUM_ACTIONS, 0), strategySum(NUM_ACTIONS, 0) {}

    vector<double> getStrategy(double realizationWeight) {
        double normalizingSum = 0;
        for (int a = 0; a < NUM_ACTIONS; ++a) {
            strategy[a] = regretSum[a] > 0 ? regretSum[a] : 0;
            normalizingSum += strategy[a];
        }

        for (int a = 0; a < NUM_ACTIONS; ++a) {
            if (normalizingSum > 0) {
                strategy[a] /= normalizingSum;
            }
            else {
                strategy[a] = 1.0 / NUM_ACTIONS;
            }
            strategySum[a] += realizationWeight * strategy[a];
        }

        return strategy;
    }

    vector<double> getAverageStrategy() {
        vector<double> avgStrategy(NUM_ACTIONS, 0);
        double normalizingSum = 0;
        for (int a = 0; a < NUM_ACTIONS; ++a) {
            normalizingSum += strategySum[a];
        }
        for (int a = 0; a < NUM_ACTIONS; ++a) {
            if (normalizingSum > 0) {
                avgStrategy[a] = strategySum[a] / normalizingSum;
            }
            else {
                avgStrategy[a] = 1.0 / NUM_ACTIONS;
            }
        }
        return avgStrategy;
    }

    string toString() {
        string ret = infoSet + ": ";
        auto avg = getAverageStrategy();
        for (double p : avg) {
            ret += to_string(p) + " / ";
        }
        return ret;
    }
};

unordered_map<string, Node> nodeMap;

double cfr(int* cards, string history, double p0, double p1) {
    int plays = history.length();
    int player = plays % 2;
    int opponent = 1 - player;

    if (plays > 1) {
        bool terminalPass = (history[plays-1] == 'p');
        bool doubleBet = (history.substr(plays - 2, 2) == "bb");
        bool isPlayerCardHigher = cards[player] > cards[opponent];

        if (terminalPass) {
            if (history == "pp") return isPlayerCardHigher ? 1 : -1;
            else return 1;
        }
        else if (doubleBet) {
            return isPlayerCardHigher ? 2 : -2;
        }
    }

    string infoSet = to_string(cards[player]) + history;

    if (nodeMap.find(infoSet) == nodeMap.end()) {
        Node node = Node();
        node.infoSet = infoSet;
        nodeMap[infoSet] = node;
    }

    Node & node = nodeMap[infoSet];

    vector<double> strategy = node.getStrategy(player == 0 ? p0 : p1);
    vector<double> util(NUM_ACTIONS, 0);
    double nodeUtil = 0;
    for (int a = 0; a < NUM_ACTIONS; ++a) {
        string nextHistory = history + (a == 0 ? "p" : "b");
        util[a] = player == 0 ? - cfr(cards, nextHistory, p0 * strategy[a], p1) : - cfr(cards, nextHistory, p0, p1 * strategy[a]);
        nodeUtil += util[a] * strategy[a];
    }

    for (int a = 0; a < NUM_ACTIONS; ++a) {
        double regret = util[a] - nodeUtil;
        node.regretSum[a] += (player == 0 ? p1 : p0) * regret;
    }

    return nodeUtil;
}

void train(int iterations) {
    int cards[] = { 1, 2, 3 };
    double util = 0;
    for (int i = 0; i < iterations; ++i) {

        for (int c1 = 3 - 1; c1 > 0; --c1) {
            uniform_int_distribution<> dist(0, c1);
            int c2 = dist(engine);
            int tmp = cards[c1];
            cards[c1] = cards[c2];
            cards[c2] = tmp;
        }

        util += cfr(cards, "", 1, 1);
    }

    for (auto n : nodeMap) {
        cout << n.second.toString() << endl;
    }
}

int main() {
    int iterations = 1000000;
    train(iterations);
}