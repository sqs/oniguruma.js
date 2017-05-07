#ifndef SRC_ONIG_RESULT_H_
#define SRC_ONIG_RESULT_H_

#include "oniguruma.h"

class OnigResult {
 public:
  explicit OnigResult(OnigRegion* region, int indexInScanner);
  ~OnigResult();

  int Count();
  int LocationAt(int index);
  int LengthAt(int index);
  int Index() { return indexInScanner; }
  void SetIndex(int newIndex) { indexInScanner = newIndex; }

 private:
  OnigResult(const OnigResult&);  // Disallow copying
  OnigResult &operator=(const OnigResult&);  // Disallow copying

  OnigRegion *region_;
  int indexInScanner;
};

#include "nbind/nbind.h"

#ifdef NBIND_CLASS
NBIND_CLASS(OnigResult) {
  construct<OnigRegion*, int>();
  method(Count);
  method(LocationAt);
  method(LengthAt);
  method(Index);
  method(SetIndex);
}
#endif

#endif  // SRC_ONIG_RESULT_H_
