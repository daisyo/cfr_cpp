#include <iostream>
#include <vector>
#include <random>

using namespace std;

const int ROCK = 0, PAPER = 1, SCISSORS = 2, NUM_ACTIONS = 3;
vector<double> regretSum, strategy, strategySum, oppStrategy({ 0.4, 0.3, 0.3 });

random_device rnd;
default_random_engine engine(rnd());

vector<double> getStrategy() {
    vector<double> strategy(NUM_ACTIONS);;

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
        strategySum[a] += strategy[a];
    }
    return strategy;
}

int getAction(vector<double> strategy) {
    uniform_real_distribution<> dist(0.0, 1.0);
    double r = dist(engine);
    int a = 0;
    double cumulativeProbability = 0;

    while (a < NUM_ACTIONS - 1) {
        cumulativeProbability += strategy[a];
        if (r < cumulativeProbability) {
            break;
        }
        a++;
    }

    return a;
}

vector<double> getAverageStrategy() {
    vector<double> averageStrategy(NUM_ACTIONS);
    double normalizingSum = 0;
    for (int a = 0; a < NUM_ACTIONS; ++a) {
        normalizingSum += strategySum[a];
    }
    for (int a = 0; a < NUM_ACTIONS; ++a) {
        if (normalizingSum > 0) {
            averageStrategy[a] = strategySum[a] / normalizingSum;
        }
        else {
            averageStrategy[a] = 1.0 / NUM_ACTIONS;
        }
    }
    return averageStrategy;
}

void train(int iterations) {
    vector<double> actionUtility(NUM_ACTIONS, 0);
    regretSum.resize(NUM_ACTIONS, 0);
    strategySum.resize(NUM_ACTIONS, 0);
    for (int i = 0; i < iterations; ++i) {
        strategy = getStrategy();
        int myAction = getAction(strategy);
        int opponentAction = getAction(oppStrategy);

        actionUtility[opponentAction] = 0;
        actionUtility[opponentAction == NUM_ACTIONS - 1 ? 0 : opponentAction + 1] = 1;
        actionUtility[opponentAction == 0 ? NUM_ACTIONS - 1 : opponentAction - 1] = -1;

        for (int a = 0; a < NUM_ACTIONS; ++a) {
            regretSum[a] += actionUtility[a] - actionUtility[myAction];
        }
    }
}

int main() {
    train(100000);
    vector<double> avgStrategy = getAverageStrategy();
    for (int a = 0; a < NUM_ACTIONS; ++a) {
        cout << avgStrategy[a] << endl;
    }
}