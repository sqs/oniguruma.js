#include "onig-scanner.h"
#include "onig-scanner-worker.h"

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

OnigScanner::~OnigScanner() {}

OnigNextMatchResult *OnigScanner::FindNextMatchSync(OnigString *onigString, int startLocation)
{
  shared_ptr<OnigResult> bestResult = searcher->Search(onigString, startLocation);
  if (bestResult != NULL)
  {
    OnigNextMatchResult result;
    result.index = bestResult->Index();
    result.captureIndices = CaptureIndicesForMatch(bestResult.get(), onigString);
    return &result;
  }
  return NULL;
}

std::vector<OnigCaptureIndex> OnigScanner::CaptureIndicesForMatch(OnigResult *result, OnigString *source)
{
  int resultCount = result->Count();
  std::vector<OnigCaptureIndex> captures;
  captures.reserve(resultCount);

  for (int index = 0; index < resultCount; index++)
  {
    int captureStart = source->ConvertUtf8OffsetToUtf16(result->LocationAt(index));
    int captureEnd = source->ConvertUtf8OffsetToUtf16(result->LocationAt(index) + result->LengthAt(index));

    OnigCaptureIndex capture;
    capture.index = index;
    capture.start = captureStart;
    capture.end = captureEnd;
    capture.length = captureEnd - captureStart;
    captures.push_back(capture);
  }

  return captures;
}
