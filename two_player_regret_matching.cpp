#include <iostream>
#include <vector>
#include <random>

using namespace std;

const int ROCK = 0, PAPER = 1, SCISSORS = 2, NUM_ACTIONS = 3;

random_device rnd;
default_random_engine engine(rnd());

struct Player {
    vector<double> regretSum, strategy, strategySum;

    Player(): regretSum(NUM_ACTIONS, 0), strategy(NUM_ACTIONS, 0), strategySum(NUM_ACTIONS, 0) {}

    vector<double> getStrategy() {
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
};

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

void train(int iterations) {
    vector<double> actionUtility(NUM_ACTIONS, 0);
    Player p1, p2;
    for (int i = 0; i < iterations; ++i) {
        p1.getStrategy();
        p2.getStrategy();
        int myAction = getAction(p1.strategy);
        int opponentAction = getAction(p2.strategy);

        // 自分のRegret更新
        actionUtility[opponentAction] = 0;
        actionUtility[opponentAction == NUM_ACTIONS - 1 ? 0 : opponentAction + 1] = 1;
        actionUtility[opponentAction == 0 ? NUM_ACTIONS - 1 : opponentAction - 1] = -1;

        for (int a = 0; a < NUM_ACTIONS; ++a) {
            p1.regretSum[a] += actionUtility[a] - actionUtility[myAction];
        }

        // OpponentのRegret更新
        actionUtility[myAction] = 0;
        actionUtility[myAction == NUM_ACTIONS - 1 ? 0 : myAction + 1] = 1;
        actionUtility[myAction == 0 ? NUM_ACTIONS - 1 : myAction - 1] = -1;

        for (int a = 0; a < NUM_ACTIONS; ++a) {
            p2.regretSum[a] += actionUtility[a] - actionUtility[opponentAction];
        }
    }

    vector<double> p1avg = p1.getAverageStrategy();
    vector<double> p2avg = p2.getAverageStrategy();

    cout << "p1: " << endl;
    for (int a = 0; a < NUM_ACTIONS; ++a) {
        cout << p1avg[a] << endl;
    }

    cout << "p2: " << endl;
    for (int a = 0; a < NUM_ACTIONS; ++a) {
        cout << p2avg[a] << endl;
    }
}

int main() {
    train(100000);
}