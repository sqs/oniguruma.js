#include "./regexp.h"
#include "nbind/api.h"

OnigRegExp::OnigRegExp(const std::string &source)
    : source_(source),
      regex_(NULL)
{
  lastSearchStrUniqueId = -1;
  lastSearchPosition = -1;
  lastSearchResult = NULL;

  OnigErrorInfo error;
  const UChar *sourceData = (const UChar *)source.data();
  int status = onig_new(&regex_, sourceData, sourceData + source.length(),
                        ONIG_OPTION_CAPTURE_GROUP, ONIG_ENCODING_UTF8,
                        ONIG_SYNTAX_DEFAULT, &error);

  if (status != ONIG_NORMAL)
  {
    UChar errorString[ONIG_MAX_ERROR_MESSAGE_LEN];
    onig_error_code_to_str(errorString, status, &error);
    NBIND_ERR(reinterpret_cast<char *>(errorString));
  }

  //printf("OnigRegExp::OnigRegExp\n");
}

OnigRegExp::~OnigRegExp()
{
  //printf("~OnigRegExp: %d\n");
  if (regex_)
    onig_free(regex_);
}

shared_ptr<OnigResult> OnigRegExp::Search(OnigString &str, int position)
{
  if (lastSearchStrUniqueId == str.uniqueId() && lastSearchPosition <= position)
  {
    if (lastSearchResult == NULL || lastSearchResult->LocationAt(0) >= position)
    {
      return lastSearchResult;
    }
  }

  lastSearchStrUniqueId = str.uniqueId();
  lastSearchPosition = position;
  lastSearchResult = DoSearch(str.utf8_value(), position, str.utf8_length());
  return lastSearchResult;
}

shared_ptr<OnigResult> OnigRegExp::DoSearch(const char *data,
                                            size_t position, size_t end)
{
  if (!regex_)
  {
    NBIND_ERR("RegExp is not valid");
    return shared_ptr<OnigResult>();
  }

  const UChar *searchData = reinterpret_cast<const UChar *>(data);
  OnigRegion *region = onig_region_new();
  int status = onig_search(regex_, searchData, searchData + end,
                           searchData + position, searchData + end, region,
                           ONIG_OPTION_NONE);

  if (status != ONIG_MISMATCH)
  {
    return shared_ptr<OnigResult>(new OnigResult(region, -1));
  }
  else
  {
    onig_region_free(region, 1);
    return shared_ptr<OnigResult>();
  }
}

#include "nbind/nbind.h"

#ifdef NBIND_CLASS
NBIND_CLASS(OnigRegExp)
{
  construct<std::string &>();
  method(Search);
}
#endif // NBIND_CLASS