#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>

using namespace std;

const int MAX_INT = 100000;
const int MAX_SET_SIZE = 500;
const int NUM_SETS = 100;

const char* ALGORITHMS[] = {"stl"};

typedef chrono::high_resolution_clock Clock;
typedef chrono::milliseconds ms;

vector<int> set_union(vector<int> sets[], string algorithm) {
  if (algorithm == "stl") {
    vector<int> result(sets[0]);
    vector<int> tmp;

    for (int i = 1; i < NUM_SETS; ++i) {
      set_union(result.begin(), result.end(),
                sets[i].begin(), sets[i].end(),
                back_inserter(tmp));
      result = tmp;
      tmp.clear();
    }

    return result;

  } else {
    throw runtime_error("unknown algorithm");
  }
}

void print_set(vector<int> set) {
  for (int elem : set) {
    cout << elem << "\t";
  }
  cout << endl;
}

int main(int argc, char* argv[]) {
  vector<int> sets[NUM_SETS];

  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> distribution(1, MAX_INT);

  for (int set_i = 0; set_i < NUM_SETS; ++set_i) {
    set<int> tmp;
    for (int elem_i = 0; elem_i < MAX_SET_SIZE; ++elem_i) {
      tmp.insert(distribution(gen));
    }
    sets[set_i].insert(sets[set_i].end(), tmp.begin(), tmp.end());
  }

  for (const char* alg : ALGORITHMS) {
    cout << "using algorithm " << alg << endl;
    Clock::time_point start = Clock::now();
    vector<int> output = set_union(sets, string(alg));
    Clock::time_point end = Clock::now();
    ms time = chrono::duration_cast<ms>(end - start);
    cout << "time: " << time.count() << "ms" << endl << endl;
  }
}
