#include <string>
#include <cstring>
#include <vector>
#include <cmath>
#include <set>
#include <algorithm>
#include <numeric>
#include "validate_interactive.h"

using namespace std;

const int query_limit = 6260;

vector<int> suffix_array_doubling(const vector<int>& s) {
  int n = int(s.size());
  vector<int> sa(n), rnk = s, tmp(n);
  iota(sa.begin(), sa.end(), 0);
  for (int k = 1; k < n; k *= 2) {
    auto cmp = [&](int x, int y) {
      if (rnk[x] != rnk[y]) return rnk[x] < rnk[y];
      int rx = x + k < n ? rnk[x + k] : -1;
      int ry = y + k < n ? rnk[y + k] : -1;
      return rx < ry;
    };
    sort(sa.begin(), sa.end(), cmp);
    tmp[sa[0]] = 0;
    for (int i = 1; i < n; i++) {
      tmp[sa[i]] = tmp[sa[i - 1]] + (cmp(sa[i - 1], sa[i]) ? 1 : 0);
    }
    swap(tmp, rnk);
  }
  return sa;
}

int main(int argc, char **argv) {
  init_io(argc, argv);

  string s;
  if (!(judge_in >> s)) {
    judge_error("Input error: Unable to read S\n");
  }
  int n = ssize(s);
  vector<int> s_vec(n);
  for (int i = 0; i < n; ++i) s_vec[i] = static_cast<int>(s[i]);
  vector<int> sa = suffix_array_doubling(s_vec), ord(n);
  for (int i = 0; i < n; ++i) ord[sa[i]] = i;

  // Check the correctness of the suffix array
  for (int i = 0; i < n - 1; ++i) if (!(s.substr(sa[i]) < s.substr(sa[i + 1]))) {
    judge_error("Judge error: suffix array is not correctly calculated\n");
  }

  cout << n << endl;

  int query_count = 0;
  while (true) {
    string op;
    if (!(author_out >> op)) {
      wrong_answer("operation is not given\n");
    }
    if (strcasecmp(op.c_str(), "query") == 0) {
      int i, j;
      if (!(author_out >> i)) {
        wrong_answer("i is not given\n");
      }
      if (i < 1 || i > n) {
        wrong_answer("i is out of range\n");
      }
      if (!(author_out >> j)) {
        wrong_answer("j is not given\n");
      }
      if (j < 1 || j > n) {
        wrong_answer("j is out of range\n");
      }
      if (i == j) {
        wrong_answer("i = j is not allowed\n");
      }

      query_count++;
      if (query_count > query_limit) {
        wrong_answer("query limit exceeded\n");
      }

      if (ord[i - 1] < ord[j - 1]) {
        cout << "first" << endl;
      } else {
        cout << "second" << endl;
      }
    }
    else if (strcasecmp(op.c_str(), "answer") == 0) {
      vector<int> guess(n);
      for (int i = 0; i < n; ++i) {
        if (!(author_out >> guess[i])) {
          wrong_answer("guess is not fully given\n");
        }
        if (guess[i] < 1 || guess[i] > n) {
          wrong_answer("guess %d is out of range\n", i + 1);
        }
      }

      for (int i = 0; i < n; ++i) {
        if (guess[i] != sa[i] + 1) {
          wrong_answer("guess is wrong; %d-th guess (1-indexed) should be %d\n", i + 1, sa[i] + 1);
        }
      }

      string trash;
      if (author_out >> trash) {
        wrong_answer("extra output: '%s'\n", trash.c_str());
      }

      accept();
    }
    else {
      wrong_answer("invalid operation type: %s\n", op.c_str());
    }
  }
}
