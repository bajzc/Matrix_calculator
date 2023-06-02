#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include "demo.h"
int regex() {
  const char* string = "[1.23 -4.56 7.89]";
  const char* pattern = "-?\\d+\\.\\d+";

  pcre2_code* re;
  int errorcode;
  PCRE2_SIZE erroroffset;
  re = pcre2_compile((PCRE2_SPTR)pattern, PCRE2_ZERO_TERMINATED, 0, &errorcode,
                     &erroroffset, NULL);
  if (re == NULL) {
    PCRE2_UCHAR buffer[256];
    pcre2_get_error_message(errorcode, buffer, sizeof(buffer));
    printf("Regular expression compilation failed: %s\n", buffer);
    return 1;
  }

  pcre2_match_data* match_data;
  match_data = pcre2_match_data_create_from_pattern(re, NULL);

  PCRE2_SPTR subject = (PCRE2_SPTR)string;
  PCRE2_SIZE subject_length = (PCRE2_SIZE)strlen(string);
  PCRE2_SIZE start_offset = 0;

  int rc;
  while ((rc = pcre2_match(re, subject, subject_length, start_offset, 0,
                           match_data, NULL)) > 0) {
    PCRE2_SIZE* ovector = pcre2_get_ovector_pointer(match_data);
    PCRE2_SPTR substring_start = subject + ovector[0];
    PCRE2_SIZE substring_length = ovector[1] - ovector[0];

    printf("Matched signed floating-point number: ");
    for (int i = 0; i < substring_length; i++) {
      printf("%c", substring_start[i]);
    }
    printf("\n");
    start_offset = ovector[1];
  }

  pcre2_match_data_free(match_data);
  pcre2_code_free(re);

  return 0;
}
