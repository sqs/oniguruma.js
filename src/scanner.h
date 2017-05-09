#ifndef SRC_ONIG_SCANNER_H_
#define SRC_ONIG_SCANNER_H_

#include <memory>
#include "nbind/api.h"
#include "./str.h"
#include "./result.h"
#include "./regexp.h"
#include "./searcher.h"

using ::std::shared_ptr;
using ::std::vector;

class OnigCaptureIndex
{
public:
  OnigCaptureIndex() {}
  explicit OnigCaptureIndex(const OnigCaptureIndex *other)
      : index(other->index), start(other->start), end(other->end), length(other->length) {}

  int index;
  int start;
  int end;
  int length;

  int getIndex() { return index; }
  int getStart() { return start; }
  int getEnd() { return end; }
  int getLength() { return length; }

  void toJS(nbind::cbOutput output)
  {
    output(index, start, end, length);
  }
};

class OnigNextMatchResult
{
public:
  OnigNextMatchResult() {}
  explicit OnigNextMatchResult(const OnigNextMatchResult *other)
      : index(other->index), captureIndices(other->captureIndices) {}

  int index;
  std::vector<OnigCaptureIndex> captureIndices;

  int getIndex() { return index; }
  std::vector<OnigCaptureIndex> getCaptureIndices() { return captureIndices; }

  void toJS(nbind::cbOutput output)
  {
    output(index, captureIndices);
  }
};

class OnigScanner
{
public:
  OnigScanner(std::vector<std::string> sources);
  std::shared_ptr<OnigNextMatchResult> FindNextMatchSync(OnigString &onigString, int startLocation);
  static std::vector<OnigCaptureIndex> CaptureIndicesForMatch(std::shared_ptr<OnigResult>, OnigString &source);

private:
  vector<shared_ptr<OnigRegExp>> regExps;
  shared_ptr<OnigSearcher> searcher;

  OnigScanner(const OnigScanner &) = delete;            // Disallow copying
  OnigScanner &operator=(const OnigScanner &) = delete; // Disallow copying
};

#endif // SRC_ONIG_SCANNER_H_
