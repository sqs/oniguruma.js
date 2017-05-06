    {
      'target_name': 'oniguruma',
      'type': 'static_library',
      'defines': ['ONIG_SKIP_POSIX_REGEX_H'],
      'conditions': [
        ['OS=="win"', {
          'msvs_disabled_warnings': [
            4244,  # conversion from '__int64' to 'int', possible loss of data
          ],
          'defines': [
            'ONIG_EXTERN=extern',
          ],
        }],
        ['OS=="linux"', {
          'cflags': [
            '-w',
          ],
        }],
      ],
      'direct_dependent_settings': {
        'include_dirs': [
          'onig'
        ],
      },
      'include_dirs': [
        'onig',
        'onig/enc',
      ],
      'sources': [
        'onig/oniggnu.h',
        'onig/onigposix.h',
        'onig/oniguruma.h',
        'onig/regcomp.c',
        'onig/regenc.c',
        'onig/regenc.h',
        'onig/regerror.c',
        'onig/regexec.c',
        'onig/regext.c',
        'onig/reggnu.c',
        'onig/regint.h',
        'onig/regparse.c',
        'onig/regparse.h',
        'onig/regposerr.c',
        'onig/regposix.c',
        'onig/regsyntax.c',
        'onig/regtrav.c',
        'onig/regversion.c',
        'onig/st.c',
        'onig/st.c',
        'onig/enc/ascii.c',
        'onig/enc/big5.c',
        'onig/enc/cp1251.c',
        'onig/enc/euc_jp.c',
        'onig/enc/euc_kr.c',
        'onig/enc/euc_tw.c',
        'onig/enc/gb18030.c',
        'onig/enc/iso8859_1.c',
        'onig/enc/iso8859_2.c',
        'onig/enc/iso8859_3.c',
        'onig/enc/iso8859_4.c',
        'onig/enc/iso8859_5.c',
        'onig/enc/iso8859_6.c',
        'onig/enc/iso8859_7.c',
        'onig/enc/iso8859_8.c',
        'onig/enc/iso8859_9.c',
        'onig/enc/iso8859_10.c',
        'onig/enc/iso8859_11.c',
        'onig/enc/iso8859_13.c',
        'onig/enc/iso8859_14.c',
        'onig/enc/iso8859_15.c',
        'onig/enc/iso8859_16.c',
        'onig/enc/koi8.c',
        'onig/enc/koi8_r.c',
        'onig/enc/mktable.c',
        'onig/enc/sjis.c',
        'onig/enc/unicode.c',
        'onig/enc/utf16_be.c',
        'onig/enc/utf16_le.c',
        'onig/enc/utf32_be.c',
        'onig/enc/utf32_le.c',
        'onig/enc/utf8.c',
      ],
    }