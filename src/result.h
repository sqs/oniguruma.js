#ifndef SRC_ONIG_RESULT_H_
#define SRC_ONIG_RESULT_H_

#include "oniguruma.h"

class OnigResult
{
public:
  explicit OnigResult(OnigRegion *region, int indexInScanner);
  ~OnigResult();

  int Count();
  int LocationAt(int index);
  int LengthAt(int index);
  int Index() { return indexInScanner; }
  void SetIndex(int newIndex) { indexInScanner = newIndex; }

private:
  OnigResult(const OnigResult &) = delete;   // Disallow copying
  OnigResult &operator=(const OnigResult &); // Disallow copying

  OnigRegion *region_;
  int indexInScanner;
};

#endif // SRC_ONIG_RESULT_H_
