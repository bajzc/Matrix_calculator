#include <stdio.h>
#include <stdlib.h>
#define PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <string.h>
int regex()
{
		int errornumber;
		pcre2_code *re;
		PCRE2_SPTR pattern;
		PCRE2_SPTR subject;
		PCRE2_SPTR name_table;
		PCRE2_SIZE erroroffset;
		PCRE2_SIZE *ovector;

		pattern = (PCRE2_SPTR) "(\\[)((\\s)*\\d)+(\\])";
		subject = (PCRE2_SPTR) "[1 2 3]";
		size_t subject_length = strlen((char *)subject);

		re = pcre2_compile(pattern, PCRE2_ZERO_TERMINATED, 0, &errornumber,
						   &erroroffset, NULL);
		if (re == NULL) {
				PCRE2_UCHAR buffer[256];
				pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
				printf("ERROR at offset %d :\n%s\n", (int)erroroffset, buffer);
				exit(-1);
		}

		pcre2_match_data *match_data =
				pcre2_match_data_create_from_pattern(re, NULL);

		int rc = pcre2_match(re, subject, subject_length, 0, 0, match_data,
							 NULL);
		if (rc < 0) {
				switch (rc) {
				case PCRE2_ERROR_NOMATCH:
						puts("NOMATCH");
						break;
				default:
						puts("Matching error");
						break;
				}
				pcre2_match_data_free(match_data);
				pcre2_code_free(re);
				exit(-1);
		}

		ovector = pcre2_get_ovector_pointer(match_data);
		printf("Match succeeded at offset %d\n", (int)ovector[0]);

		if (rc == 0)
				printf("ovector was not big enough for all the captured substrings\n");

		for (int i = 0; i < rc; i++) {
				PCRE2_SPTR substring_start = subject + ovector[2 * i];
				size_t substring_length = ovector[2 * i + 1] - ovector[2 * i];
				printf("%2d: %.*s\n", i, (int)substring_length,
					   (char *)substring_start);
		}
		pcre2_match_data_free(match_data);
		pcre2_code_free(re);
		return 0;
}