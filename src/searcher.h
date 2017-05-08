#ifndef SRC_ONIG_SEARCHER_H_
#define SRC_ONIG_SEARCHER_H_

#include <vector>
#include "./str.h"
#include "./regexp.h"
#include "./result.h"

using ::std::shared_ptr;
using ::std::vector;

class OnigSearcher
{
public:
  explicit OnigSearcher(vector<shared_ptr<OnigRegExp>> regExps)
      : regExps(regExps) {}

  shared_ptr<OnigResult> Search(OnigString &source, int charOffset);

private:
  vector<shared_ptr<OnigRegExp>> regExps;

  OnigSearcher(const OnigSearcher &) = delete;            // Disallow copying
  OnigSearcher &operator=(const OnigSearcher &) = delete; // Disallow copying
};

#endif // SRC_ONIG_SEARCHER_H_
