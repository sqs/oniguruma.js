#ifndef SRC_ONIG_SCANNER_H_
#define SRC_ONIG_SCANNER_H_

#include <memory>
#include "nan.h"
#include "onig-string.h"
#include "onig-result.h"
#include "onig-reg-exp.h"
#include "onig-searcher.h"
#include "nbind/api.h"
#include "nbind/nbind.h"

using ::std::shared_ptr;
using ::std::vector;

class OnigCaptureIndex
{
public:
  OnigCaptureIndex();
  int index;
  int start;
  int end;
  int length;

  int getIndex() { return index; }
  int getStart() { return start; }
  int getEnd() { return end; }
  int getLength() { return length; }
};

class OnigNextMatchResult
{
public:
  OnigNextMatchResult();
  int index;
  std::vector<OnigCaptureIndex> captureIndices;

  int getIndex() { return index; }
  std::vector<OnigCaptureIndex> getCaptureIndices() { return captureIndices; }
};

class OnigScanner
{
public:
  OnigScanner(std::vector<string> sources);
  ~OnigScanner();
  OnigNextMatchResult *FindNextMatchSync(OnigString *onigString, int startLocation);
  static std::vector<OnigCaptureIndex> CaptureIndicesForMatch(OnigResult *result, OnigString *source);

private:
  vector<shared_ptr<OnigRegExp>> regExps;
  shared_ptr<OnigSearcher> searcher;
};

#ifdef NBIND_CLASS

NBIND_CLASS(OnigCaptureIndex)
{
  construct<>();
  getter(getIndex);
  getter(getStart);
  getter(getEnd);
  getter(getLength);
}

NBIND_CLASS(OnigNextMatchResult)
{
  construct<>();
  getter(getIndex);
  getter(getCaptureIndices);
}

NBIND_CLASS(OnigScanner)
{
  construct<std::vector<std::string>>();
  method(FindNextMatchSync);
  method(CaptureIndicesForMatch);
}

#endif // NBIND_CLASS

#endif // SRC_ONIG_SCANNER_H_
