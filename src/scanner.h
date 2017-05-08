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
  OnigCaptureIndex(){};
  explicit OnigCaptureIndex(const OnigCaptureIndex *other)
      : index(other->index), start(other->start), end(other->end), length(other->length){};

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

private:
  //OnigCaptureIndex(const OnigCaptureIndex &) = delete;            // Disallow copying
  //OnigCaptureIndex &operator=(const OnigCaptureIndex &) = delete; // Disallow copying
};

class OnigNextMatchResult
{
public:
  OnigNextMatchResult(){};
  explicit OnigNextMatchResult(const OnigNextMatchResult *other)
      : index(other->index), captureIndices(other->captureIndices){};
  ~OnigNextMatchResult();

  int index;
  std::vector<OnigCaptureIndex> captureIndices;

  int getIndex() { return index; }
  std::vector<OnigCaptureIndex> getCaptureIndices() { return captureIndices; }

  void toJS(nbind::cbOutput output)
  {
    output(index, captureIndices);
  }

private:
  //OnigNextMatchResult(const OnigNextMatchResult &) = delete;            // Disallow copying
  //OnigNextMatchResult &operator=(const OnigNextMatchResult &) = delete; // Disallow copying
};

class OnigScanner
{
public:
  OnigScanner(std::vector<std::string> sources);
  OnigNextMatchResult *FindNextMatchSync(OnigString &onigString, int startLocation);
  static std::vector<OnigCaptureIndex> CaptureIndicesForMatch(OnigResult *result, OnigString &source);

private:
  vector<shared_ptr<OnigRegExp>> regExps;
  shared_ptr<OnigSearcher> searcher;

  OnigScanner(const OnigScanner &) = delete;            // Disallow copying
  OnigScanner &operator=(const OnigScanner &) = delete; // Disallow copying
};

#endif // SRC_ONIG_SCANNER_H_
