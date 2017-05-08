#include "./result.h"
#include <stdio.h>

static int ONIGRESULTS = 0;

OnigResult::OnigResult(OnigRegion *region, int indexInScanner)
    : region_(region), indexInScanner(indexInScanner)
{
  printf("+++ OnigResult: %d\n", ++ONIGRESULTS);
}

OnigResult::~OnigResult()
{
  printf("--- OnigResult: %d\n", --ONIGRESULTS);
  onig_region_free(region_, 1);
}

int OnigResult::Count()
{
  return region_->num_regs;
}

int OnigResult::LocationAt(int index)
{
  int bytes = *(region_->beg + index);
  if (bytes > 0)
    return bytes;
  else
    return 0;
}

int OnigResult::LengthAt(int index)
{
  int bytes = *(region_->end + index) - *(region_->beg + index);
  if (bytes > 0)
    return bytes;
  else
    return 0;
}

#include "nbind/nbind.h"

#ifdef NBIND_CLASS
NBIND_CLASS(OnigResult)
{
  method(Count);
  method(LocationAt);
  method(LengthAt);
  method(Index);
  method(SetIndex);
}
#endif // NBIND_CLASS