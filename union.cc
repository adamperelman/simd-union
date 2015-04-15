#include <algorithm>
#include <cassert>
#include <chrono>
#include <iostream>
#include <limits>
#include <omp.h>
#include <parallel/algorithm>
#include <random>
#include <set>
#include <stdexcept>
#include <unordered_set>

using namespace std;

#define MEDIUM

const int MIN_ELEM = 1;

#ifdef SMALL
const int MAX_ELEM = 10;
const int MAX_SET_SIZE = 3;
const int NUM_SETS = 3;
#endif

#ifdef MEDIUM
const int MAX_ELEM = 1 << 22;
const int MAX_SET_SIZE = 1 << 11;
const int NUM_SETS = 1 << 11;
#endif

const char* ALGORITHMS[] = {"stl",
                            "stl_parallel",
                            "intermediate_set"};

typedef chrono::high_resolution_clock Clock;
typedef chrono::milliseconds ms;

inline size_t elapsed_time(Clock::time_point start,
                           Clock::time_point end) {
  return chrono::duration_cast<ms>(end - start).count();
}

inline void print_time(const char* message,
                       Clock::time_point start,
                       Clock::time_point end) {
  cout << message << " time: "
       << elapsed_time(start, end) << "ms" << endl;
}
inline vector<int> merge_sets_stl(const vector<int>& a,
                                  const vector<int>& b) {
  vector<int> result;
  result.reserve(a.size() + b.size());
  set_union(a.begin(), a.end(),
            b.begin(), b.end(),
            back_inserter(result));

  return result;
}
vector<int> stl_set_union(vector<int> sets[], int start, int end) {
  if (start == end) return vector<int>();
  if (start == end - 1) return sets[start];

  int midpoint = start + (end - start) / 2;
  vector<int> a = stl_set_union(sets, start, midpoint);
  vector<int> b = stl_set_union(sets, midpoint, end);

  return merge_sets_stl(a, b);
}

vector<int> stl_set_union(vector<int> sets[]) {
  return stl_set_union(sets, 0, NUM_SETS);
}

vector<int> stl_set_union_parallel(vector<int> sets[]) {
  const int NUM_CHUNKS = 1 << 3;
  vector<int> partial_results[NUM_CHUNKS];
  int step = NUM_SETS / NUM_CHUNKS;
  assert(step * NUM_CHUNKS == NUM_SETS);

  Clock::time_point parallel_start = Clock::now();

  #pragma omp parallel for
  for (int i = 0; i < NUM_CHUNKS; ++i) {
    partial_results[i] = stl_set_union(sets, step*i, step*(i+1));
  }

  Clock::time_point parallel_end = Clock::now();

  print_time("parallel", parallel_start, parallel_end);

  return stl_set_union(partial_results, 0, NUM_CHUNKS);
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

vector<int> intermediate_set_union(vector<int> sets[]) {
  Clock::time_point insert_start = Clock::now();
  unordered_set<int> s;
  for (int i = 0; i < NUM_SETS; ++i) {
    s.insert(sets[i].begin(), sets[i].end());
  }

  Clock::time_point insert_end = Clock::now();

  print_time("inserting", insert_start, insert_end);

  vector<int> result;
  result.reserve(s.size());
  result.insert(result.end(), s.begin(), s.end());

  Clock::time_point sort_start = Clock::now();
  __gnu_parallel::sort(result.begin(), result.end());
  Clock::time_point sort_end = Clock::now();

  print_time("sorting", sort_start, sort_end);

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
  } else if (algorithm == "stl_parallel") {
    return stl_set_union_parallel(sets);
  } else if (algorithm == "intermediate_set") {
    return intermediate_set_union(sets);
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
    cout << "**** " << alg << " ****" << endl;

    Clock::time_point start = Clock::now();
    vector<int> output = set_union(sets, string(alg));
    Clock::time_point end = Clock::now();

    cout << endl;
    print_time("total", start, end);

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
