#include "./scanner.h"

OnigScanner::OnigScanner(std::vector<std::string> sources)
{
  int length = sources.size();
  regExps.resize(length);

  for (int i = 0; i < length; i++)
  {
    regExps[i] = shared_ptr<OnigRegExp>(new OnigRegExp(sources[i]));
  }

  searcher = shared_ptr<OnigSearcher>(new OnigSearcher(regExps));
}

std::shared_ptr<OnigNextMatchResult> OnigScanner::FindNextMatchSync(OnigString &onigString, int startLocation)
{
  shared_ptr<OnigResult> bestResult = searcher->Search(onigString, startLocation);
  if (bestResult != NULL)
  {
    std::shared_ptr<OnigNextMatchResult> result(new OnigNextMatchResult());
    result->index = bestResult->Index();
    result->captureIndices = CaptureIndicesForMatch(bestResult, onigString);
    return result;
  }
  return std::shared_ptr<OnigNextMatchResult>();
}

std::vector<OnigCaptureIndex> OnigScanner::CaptureIndicesForMatch(std::shared_ptr<OnigResult> result, OnigString &source)
{
  int resultCount = result->Count();
  std::vector<OnigCaptureIndex> captures;
  captures.reserve(resultCount);

  for (int index = 0; index < resultCount; index++)
  {
    int captureStart = source.ConvertUtf8OffsetToUtf16(result->LocationAt(index));
    int captureEnd = source.ConvertUtf8OffsetToUtf16(result->LocationAt(index) + result->LengthAt(index));

    OnigCaptureIndex capture;
    capture.index = index;
    capture.start = captureStart;
    capture.end = captureEnd;
    capture.length = captureEnd - captureStart;
    captures.push_back(capture);
  }

  return captures;
}

#include "nbind/nbind.h"

#ifdef NBIND_CLASS
NBIND_CLASS(OnigCaptureIndex)
{
  construct<>();
  construct<const OnigCaptureIndex *>();
  getter(getIndex);
  getter(getStart);
  getter(getEnd);
  getter(getLength);
}
NBIND_CLASS(OnigNextMatchResult)
{
  construct<>();
  construct<const OnigNextMatchResult *>();
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