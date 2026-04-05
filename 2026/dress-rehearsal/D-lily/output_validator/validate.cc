#include <cstring>
#include <string>
#include <vector>

#include "validate_interactive.h"

using namespace std;

constexpr int MAX_T = 100;
constexpr int MAX_N = 100;
constexpr int MAX_QUERY = 10;

constexpr char CHAR_LILY = 'l';
constexpr char CHAR_ROSE = 'r';

const string LILY = "lily";
const string ROSE = "rose";
const string ANSWER = "answer";
const string TYPE_QUERY = "type";
const string MULTIPLY_QUERY = "multi";

int main(int argc, char** argv) {
  init_io(argc, argv);

  int t;
  if (!(judge_in >> t)) {
    judge_error("Input error: t is missing\n");
  }
  if (t < 1 || t > MAX_T) {
    judge_error("Input error: t=%d is outside [1, %d]\n", t, MAX_T);
  }

  vector<string> secret(t);
  for (int tc = 0; tc < t; tc++) {
    string s;
    if (!(judge_in >> s)) {
      judge_error("Input error: testcase %d string is missing\n", tc + 1);
    }

    int n = (int)s.size();
    if (n < 1 || n > MAX_N) {
      judge_error("Input error: testcase %d n=%d is outside [1, %d]\n", tc + 1, n, MAX_N);
    }

    for (char c : s) {
      if (c != CHAR_LILY && c != CHAR_ROSE) {
        judge_error("Input error: testcase %d contains invalid character\n", tc + 1);
      }
    }

    vector<int> l(n + 1, 0), r(n + 1, 0);
    for (int i = 1; i <= n; i++) l[i] = l[i - 1] + (s[i - 1] == CHAR_LILY ? 1 : 0);
    for (int i = n - 1; i >= 0; i--) r[i] = r[i + 1] + (s[i] == CHAR_ROSE ? 1 : 0);

    bool has_answer = false;
    for (int i = 0; i <= n; i++) has_answer |= (l[i] == r[i]);
    if (!has_answer) {
      judge_error("Input error: testcase %d has no valid answer\n", tc + 1);
    }

    secret[tc] = s;
  }

  cout << t << endl;

  for (int tc = 0; tc < t; tc++) {
    const string& s = secret[tc];
    int n = (int)s.size();

    vector<int> l(n + 1, 0), r(n + 1, 0);
    for (int i = 1; i <= n; i++) l[i] = l[i - 1] + (s[i - 1] == CHAR_LILY ? 1 : 0);
    for (int i = n - 1; i >= 0; i--) r[i] = r[i + 1] + (s[i] == CHAR_ROSE ? 1 : 0);

    cout << n << endl;

    int query_count = 0;
    while (true) {
      string query;
      if (!(author_out >> query)) {
        wrong_answer("testcase %d: expected a query or answer\n", tc + 1);
      }

      int idx;
      if (!(author_out >> idx)) {
        cout << -1 << endl;
        wrong_answer("testcase %d: missing argument after '%s'\n", tc + 1, query.c_str());
      }

      if (query == ANSWER) {
        if (idx < 0 || idx > n) {
          cout << -1 << endl;
          wrong_answer("testcase %d: answer index out of range\n", tc + 1);
        }
        if (l[idx] != r[idx]) {
          cout << -1 << endl;
          wrong_answer("testcase %d: wrong answer\n", tc + 1);
        }
        break;
      }

      if (query == TYPE_QUERY) {
        query_count++;
        if (query_count > MAX_QUERY) {
          cout << -1 << endl;
          wrong_answer("testcase %d: query limit exceeded\n", tc + 1);
        }
        if (idx < 1 || idx > n) {
          cout << -1 << endl;
          wrong_answer("testcase %d: type index out of range\n", tc + 1);
        }
        cout << (s[idx - 1] == CHAR_LILY ? LILY : ROSE) << endl;
        continue;
      }

      if (query == MULTIPLY_QUERY) {
        query_count++;
        if (query_count > MAX_QUERY) {
          cout << -1 << endl;
          wrong_answer("testcase %d: query limit exceeded\n", tc + 1);
        }
        if (idx < 0 || idx > n) {
          cout << -1 << endl;
          wrong_answer("testcase %d: multiply index out of range\n", tc + 1);
        }
        cout << l[idx] * r[idx] << endl;
        continue;
      }

      cout << -1 << endl;
      wrong_answer("testcase %d: invalid command '%s'\n", tc + 1, query.c_str());
    }
  }

  string extra;
  if (author_out >> extra) {
    cout << -1 << endl;
    wrong_answer("extra output after finishing all testcases\n");
  }

  accept();
}
