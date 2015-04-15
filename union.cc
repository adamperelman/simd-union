#include <algorithm>
#include <chrono>
#include <iostream>
#include <random>
#include <set>
#include <stdexcept>

using namespace std;

const int MIN_ELEM = 1;
const int MAX_ELEM = 250000;
const int MAX_SET_SIZE = 500;
const int NUM_SETS = 500;

const char* ALGORITHMS[] = {"stl", "multiway"};

typedef chrono::high_resolution_clock Clock;
typedef chrono::milliseconds ms;

vector<int> stl_set_union(vector<int> sets[]) {
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
}

vector<int> multiway_set_union(vector<int> sets[]) {
  vector<int>::iterator its[NUM_SETS];
  for (int i = 0; i < NUM_SETS; ++i) {
    its[i] = sets[i].begin();
  }

  vector<int> result;
  for (int val = MIN_ELEM; val <= MAX_ELEM; ++val) {
    for (int i = 0; i < NUM_SETS; ++i) {
      while (*its[i] < val) its[i]++;

      if (*its[i] == val) {
        result.push_back(val);
        break;
      }
    }
  }

  return result;
}

vector<int> set_union(vector<int> sets[], string algorithm) {
  if (algorithm == "stl") {
    return stl_set_union(sets);
  } else if (algorithm == "multiway") {
    return multiway_set_union(sets);
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

void populate_sets(vector<int> sets[]) {
  random_device rd;
  mt19937 gen(rd());
  uniform_int_distribution<> distribution(MIN_ELEM, MAX_ELEM);

  for (int set_i = 0; set_i < NUM_SETS; ++set_i) {
    set<int> tmp;
    for (int elem_i = 0; elem_i < MAX_SET_SIZE; ++elem_i) {
      tmp.insert(distribution(gen));
    }
    sets[set_i].insert(sets[set_i].end(), tmp.begin(), tmp.end());
  }
}

int main(int argc, char* argv[]) {
  vector<int> sets[NUM_SETS];
  cout << "building sets..." << endl;
  populate_sets(sets);

  vector<int> reference;
  for (const char* alg : ALGORITHMS) {
    cout << "using algorithm " << alg << "..." << endl;

    Clock::time_point start = Clock::now();
    vector<int> output = set_union(sets, string(alg));
    Clock::time_point end = Clock::now();

    ms time = chrono::duration_cast<ms>(end - start);
    cout << "time: " << time.count() << "ms" << endl;

    if (reference.empty()) {
      reference = output;
      cout << "output size: " << reference.size() << endl;
    } else {
      if (reference == output) {
        cout << "output is correct" << endl;
      } else {
        cout << "OUTPUT DOES NOT MATCH" << endl;
      }
    }

    cout << endl;
  }
}
