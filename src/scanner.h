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
  OnigCaptureIndex(int index, int start, int end, int length)
      : index(index), start(start), end(end), length(length) {}
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
  OnigNextMatchResult(bool noMatch) : noMatch(noMatch) {}
  OnigNextMatchResult(bool noMatch, int index, std::vector<OnigCaptureIndex> captureIndices)
      : noMatch(noMatch), index(index), captureIndices(captureIndices) {}
  explicit OnigNextMatchResult(const OnigNextMatchResult *other)
      : noMatch(other->noMatch), index(other->index), captureIndices(other->captureIndices) {}

  bool noMatch;
  int index;
  std::vector<OnigCaptureIndex> captureIndices;

  bool getNoMatch() { return noMatch; }
  int getIndex() { return index; }
  std::vector<OnigCaptureIndex> getCaptureIndices() { return captureIndices; }

  void toJS(nbind::cbOutput output)
  {
    output(noMatch, index, captureIndices);
  }

private:
  OnigNextMatchResult() = delete;
};

class OnigScanner
{
public:
  OnigScanner(std::vector<std::string> sources);
  OnigNextMatchResult FindNextMatchSync(OnigString &onigString, int startLocation);

private:
  vector<shared_ptr<OnigRegExp>> regExps;
  shared_ptr<OnigSearcher> searcher;

  OnigScanner(const OnigScanner &) = delete;            // Disallow copying
  OnigScanner &operator=(const OnigScanner &) = delete; // Disallow copying
};

#endif // SRC_ONIG_SCANNER_H_
