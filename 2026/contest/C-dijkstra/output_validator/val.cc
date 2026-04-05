#include <fstream>
#include <iostream>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <cmath>
#include <cstdarg>
#include <tuple>

#include <algorithm>
#include <vector>
#include <sstream>
#include <format>
#include <queue>

const int EXIT_AC = 42;
const int EXIT_WA = 43;

std::ifstream judgein, judgeans;
FILE *judgemessage = NULL;
FILE *diffpos = NULL;

// At some point we should rewrite this to something more C++. Now that we truncate long tokens, having this
// require c_str() calls gets even messier with object lifetimes.
void wrong_answer(const char *err, ...) {
  va_list pvar;
  va_start(pvar, err);
  fprintf(judgemessage, "Wrong answer\n");
  vfprintf(judgemessage, err, pvar);
  fprintf(judgemessage, "\n");
  exit(EXIT_WA);
}

void judge_error(const char *err, ...) {
  va_list pvar;
  va_start(pvar, err);
  // If judgemessage hasn't been set up yet, write error to stderr
  if (!judgemessage) judgemessage = stderr;
  vfprintf(judgemessage, err, pvar);
  fprintf(judgemessage, "\n");
  assert(!"Judge Error");
}

template <typename Stream>
void openfile(Stream &stream, const char *file, const char *whoami) {
  stream.open(file);
  if (stream.fail()) {
    judge_error("%s: failed to open %s\n", whoami, file);
  }
}

FILE *openfeedback(const char *feedbackdir, const char *feedback, const char *whoami) {
  std::string path = std::string(feedbackdir) + "/" + std::string(feedback);
  FILE *res = fopen(path.c_str(), "w");
  if (!res) {
    judge_error("%s: failed to open %s for writing", whoami, path.c_str());
  }
  return res;
}

const char *USAGE = "Usage: %s judge_in judge_ans feedback_file [options] < user_out";
const char *IMPOSSIBLE = "impossible";
const int W_MIN = 1;
const int W_MAX = 1'000'000'000;

int main(int argc, char **argv) {
  if (argc < 4) {
    judge_error(USAGE, argv[0]);
  }
  judgemessage = openfeedback(argv[3], "judgemessage.txt", argv[0]);
  diffpos = openfeedback(argv[3], "diffposition.txt", argv[0]);
  openfile(judgein, argv[1], argv[0]);
  openfile(judgeans, argv[2], argv[0]);

  // read input
  int n, m;
  judgein >> n >> m;
  std::vector<std::vector<std::pair<int, int>>> adj(n + 1); // {vidx, eidx}
  for (int k = 1; k <= m; ++k) {
    int u, v;
    judgein >> u >> v;
    adj[u].push_back({v, k});
    adj[v].push_back({u, k});
  }
  std::vector<int> vs(n);
  for (int i = 0; i < n; ++i) judgein >> vs[i];

  // read tokens
  std::vector<std::string> judge, team;
  for (std::string token; judgeans >> token; ) judge.push_back(token);
  for (std::string token; std::cin >> token; ) team.push_back(token);

  auto verify = [&](const std::vector<std::string>& tokens) -> std::string {
    std::vector<long long> ws(m + 1);
    if (tokens.size() != static_cast<size_t>(m)) {
      return std::format("{} tokens found, which is not expected", tokens.size());
    }
    for (int k = 1; k <= m; ++k) {
      std::stringstream ss(tokens[k - 1]);
      long long x;
      if (!(ss >> x) || !ss.eof()) return std::format("Invalid token: '{}'", tokens[k - 1]);
      if (x < W_MIN || x > W_MAX) return std::format("The {}-th edge weight has invalid range", k);
      ws[k] = x;
    }

    std::priority_queue<std::pair<long long, int>> q;
    q.push({0LL, 1});
    std::vector<int> visited(n + 1);
    int current = 0;
    while (!q.empty()) {
      auto [dist, u] = q.top();
      q.pop();
      if (visited[u]) continue;
      visited[u] = 1;

      if (current >= n) {
        judge_error("Number of visited vertices must not exceed n");
      }
      if (vs[current] != u) {
        return std::format(
            "The {}-th element of s does not match; expected:{}, received:{}",
            current + 1, vs[current], u
        );
      }
      current++;
      for (auto [v, eidx] : adj[u]) {
        q.push({dist + ws[eidx], v});
      }
    }
    if (current != n) {
      judge_error("Number of visited vertices must be n");
    }

    return "";
  };

  if (judge.empty()) {
    judge_error("Judge's answer is empty");
  }
  if (team.empty()) {
    wrong_answer("Contestant's output is empty");
  }
  if (strcasecmp(judge[0].c_str(), IMPOSSIBLE) == 0) {
    if (strcasecmp(team[0].c_str(), IMPOSSIBLE) == 0 && team.size() == 1) {
      exit(EXIT_AC);
    }
    else {
      auto team_error = verify(team);
      if (team_error.empty()) {
        judge_error("Judge's answer is impossible, while team output satisfies the condition");
      } else {
        wrong_answer(team_error.c_str());
      }
    }
  } else {
    auto judge_verdict = verify(judge);
    if (!judge_verdict.empty()) {
      judge_error("Judge's answer is incorrect: %s", judge_verdict.c_str());
    }
    auto team_error = verify(team);
    if (team_error.empty()) {
      exit(EXIT_AC);
    } else {
      wrong_answer(team_error.c_str());
    }
  }
}
