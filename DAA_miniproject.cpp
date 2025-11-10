#include <iostream>
#include <vector>
#include <cmath>
#include <cfloat>
#include <ctime>
#include <cstdlib>
using namespace std;

// ======= PARAMETERS =======
#define NUM_CITIES 5
#define NUM_ANTS 10
#define ALPHA 1.0       // influence of pheromone
#define BETA 5.0        // influence of visibility (1/distance)
#define RHO 0.5         // pheromone evaporation rate
#define Q 100           // pheromone deposit factor
#define ITERATIONS 100  // number of cycles

// ======= GLOBAL VARIABLES =======
double dist[NUM_CITIES][NUM_CITIES] = {
    {0, 2, 9, 10, 7},
    {1, 0, 6, 4, 3},
    {15, 7, 0, 8, 3},
    {6, 3, 12, 0, 11},
    {9, 7, 5, 6, 0}
};

double pheromone[NUM_CITIES][NUM_CITIES];
double bestLength = DBL_MAX;
vector<int> bestTour;

// ======= ANT STRUCTURE =======
struct Ant {
    vector<int> tour;
    vector<bool> visited;
    double tourLength;

    Ant() {
        tour.resize(NUM_CITIES);
        visited.resize(NUM_CITIES, false);
        tourLength = 0.0;
    }

    void reset() {
        fill(visited.begin(), visited.end(), false);
        tour.clear();
        tourLength = 0.0;
    }
};

// ======= INITIALIZATION =======
void initialize() {
    for (int i = 0; i < NUM_CITIES; i++)
        for (int j = 0; j < NUM_CITIES; j++)
            pheromone[i][j] = 1.0; // equal pheromone initially
}

// ======= FUNCTION TO SELECT NEXT CITY =======
int selectNextCity(int currentCity, Ant &ant) {
    vector<double> probabilities(NUM_CITIES, 0.0);
    double sum = 0.0;

    for (int j = 0; j < NUM_CITIES; j++) {
        if (!ant.visited[j] && dist[currentCity][j] > 0) {
            probabilities[j] = pow(pheromone[currentCity][j], ALPHA) *
                               pow(1.0 / dist[currentCity][j], BETA);
            sum += probabilities[j];
        }
    }

    if (sum == 0.0) {
        for (int j = 0; j < NUM_CITIES; j++)
            if (!ant.visited[j]) return j;
    }

    double r = ((double)rand() / RAND_MAX) * sum;
    double cumulative = 0.0;

    for (int j = 0; j < NUM_CITIES; j++) {
        if (!ant.visited[j] && dist[currentCity][j] > 0) {
            cumulative += probabilities[j];
            if (cumulative >= r)
                return j;
        }
    }
    return -1;
}

// ======= CONSTRUCT TOUR =======
void constructTour(Ant &ant) {
    int startCity = rand() % NUM_CITIES;
    ant.tour.push_back(startCity);
    ant.visited[startCity] = true;
    int currentCity = startCity;

    for (int step = 1; step < NUM_CITIES; step++) {
        int nextCity = selectNextCity(currentCity, ant);
        ant.tour.push_back(nextCity);
        ant.visited[nextCity] = true;
        ant.tourLength += dist[currentCity][nextCity];
        currentCity = nextCity;
    }

    // return to start city
    ant.tourLength += dist[currentCity][startCity];
}

// ======= UPDATE PHEROMONE TRAILS =======
void updatePheromones(vector<Ant> &ants) {
    for (int i = 0; i < NUM_CITIES; i++)
        for (int j = 0; j < NUM_CITIES; j++)
            pheromone[i][j] *= (1.0 - RHO);

    for (auto &ant : ants) {
        for (int i = 0; i < NUM_CITIES - 1; i++) {
            int from = ant.tour[i];
            int to = ant.tour[i + 1];
            pheromone[from][to] += Q / ant.tourLength;
            pheromone[to][from] = pheromone[from][to];
        }
    }
}

// ======= MAIN FUNCTION =======
int main() {
    srand((unsigned)time(0));
    initialize();
    vector<Ant> ants(NUM_ANTS);

    for (int iter = 0; iter < ITERATIONS; iter++) {
        for (auto &ant : ants) {
            ant.reset();
            constructTour(ant);

            if (ant.tourLength < bestLength) {
                bestLength = ant.tourLength;
                bestTour = ant.tour;
            }
        }

        updatePheromones(ants);

        cout << "Iteration " << iter + 1
             << " | Best tour length so far: " << bestLength << endl;
    }

    cout << "\n=== Final Best Tour ===\n";
    for (size_t i = 0; i < bestTour.size(); i++) {
        cout << bestTour[i];
        if (i < bestTour.size() - 1) cout << " -> ";
    }
    cout << " -> " << bestTour.front() << endl;

    cout << "Minimum Distance: " << bestLength << endl;
    return 0;
}
