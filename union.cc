#include <algorithm>
#include <chrono>
#include <iostream>
#include <limits>
#include <random>
#include <set>
#include <stdexcept>

using namespace std;

#define MEDIUM

const int MIN_ELEM = 1;

#ifdef SMALL
const int MAX_ELEM = 10;
const int MAX_SET_SIZE = 3;
const int NUM_SETS = 3;
#endif

#ifdef MEDIUM
const int MAX_ELEM = 250000;
const int MAX_SET_SIZE = 500;
const int NUM_SETS = 500;
#endif

const char* ALGORITHMS[] = {"stl", "multiway", "min"};

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

vector<int> min_union(vector<int> sets[]) {
  vector<int>::iterator its[NUM_SETS];
  for (int i = 0; i < NUM_SETS; ++i) {
    its[i] = sets[i].begin();
  }

  vector<int> result;
  bool more_values = true;
  while (more_values) {
    int min = numeric_limits<int>::max();
    for (int i = 0; i < NUM_SETS; ++i) {
      if (its[i] < sets[i].end() && *its[i] < min) {
        min = *its[i];
      }
    }

    result.push_back(min);

    more_values = false;
    for (int i = 0; i < NUM_SETS; ++i) {
      while (its[i] < sets[i].end() && *its[i] <= min) {
        ++its[i];
      }
      if (its[i] < sets[i].end()) {
        more_values = true;
      }
    }
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
  } else if (algorithm == "min") {
    return min_union(sets);
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

  #ifdef SMALL
  cout << "input:" << endl;
  for (int i = 0; i < NUM_SETS; ++i) {
    print_set(sets[i]);
  }
  cout << endl;
  #endif

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
        #ifdef SMALL
        cout << "reference:" << endl;
        print_set(reference);
        cout << "actual:" << endl;
        print_set(output);
        #endif
      }
    }

    cout << endl;
  }
}
