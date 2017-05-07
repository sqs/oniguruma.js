#ifndef SRC_ONIG_SCANNER_H_
#define SRC_ONIG_SCANNER_H_

#include <memory>
#include "./str.h"
#include "./result.h"
#include "./regexp.h"
#include "./searcher.h"

using ::std::shared_ptr;
using ::std::vector;

class OnigCaptureIndex
{
public:
  OnigCaptureIndex(){};
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
  OnigNextMatchResult(){};
  int index;
  std::vector<OnigCaptureIndex> captureIndices;

  int getIndex() { return index; }
  std::vector<OnigCaptureIndex> getCaptureIndices() { return captureIndices; }
};

class OnigScanner
{
public:
  OnigScanner(std::vector<std::string> sources);
  ~OnigScanner();
  OnigNextMatchResult *FindNextMatchSync(OnigString *onigString, int startLocation);
  static std::vector<OnigCaptureIndex> CaptureIndicesForMatch(OnigResult *result, OnigString *source);

private:
  vector<shared_ptr<OnigRegExp>> regExps;
  shared_ptr<OnigSearcher> searcher;
};

#endif // SRC_ONIG_SCANNER_H_
