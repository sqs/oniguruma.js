#ifndef SRC_ONIG_REG_EXP_H_
#define SRC_ONIG_REG_EXP_H_

#include <memory>
#include <string>

#include "./result.h"
#include "./str.h"

using ::std::shared_ptr;

class OnigRegExp
{
public:
  explicit OnigRegExp(const std::string &source);
  ~OnigRegExp();

  shared_ptr<OnigResult> Search(OnigString &str, int position);

private:
  OnigRegExp(const OnigRegExp &);            // Disallow copying
  OnigRegExp &operator=(const OnigRegExp &); // Disallow copying

  shared_ptr<OnigResult> DoSearch(const char *data, size_t position, size_t end);

  std::string source_;
  regex_t *regex_;

  int lastSearchStrUniqueId;
  int lastSearchPosition;
  shared_ptr<OnigResult> lastSearchResult;
};

#endif // SRC_ONIG_REG_EXP_H_
