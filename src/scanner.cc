#include "./scanner.h"

static std::vector<OnigCaptureIndex> CaptureIndicesForMatch(std::shared_ptr<OnigResult> result, OnigString &source)
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

OnigNextMatchResult OnigScanner::FindNextMatchSync(OnigString &onigString, int startLocation)
{
  shared_ptr<OnigResult> bestResult = searcher->Search(onigString, startLocation);
  if (bestResult != NULL)
  {
    return OnigNextMatchResult(
        false,
        bestResult->Index(),
        CaptureIndicesForMatch(bestResult, onigString));
  }
  return OnigNextMatchResult(true);
}

#include "nbind/nbind.h"

#ifdef NBIND_CLASS
NBIND_CLASS(OnigCaptureIndex)
{
  construct<>();
  construct<int, int, int, int>();
  getter(getIndex);
  getter(getStart);
  getter(getEnd);
  getter(getLength);
}
NBIND_CLASS(OnigNextMatchResult)
{
  construct<>();
  construct<bool>();
  construct<bool, int, std::vector<OnigCaptureIndex>>();
  getter(getNoMatch);
  getter(getIndex);
  getter(getCaptureIndices);
}
NBIND_CLASS(OnigScanner)
{
  construct<std::vector<std::string>>(nbind::Strict());
  method(FindNextMatchSync, nbind::Strict());
}
#endif // NBIND_CLASS