#ifndef DEMO_REGEX_H
#define DEMO_REGEX_H

int regex(const char *string);

#ifdef __GNUC__
#define CLEAR "clear"
#elif defined _MSC_VER
#define CLEAR "cls"
#endif
#ifndef PCRE2_CODE_UNIT_WIDTH
#define PCRE2_CODE_UNIT_WIDTH 8
#endif // !PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <stdbool.h>
#include <stdio.h>
#define ADD 0
#define SUB 1
#define MLP 2
#define DET 3
#define INV 4
#endif // !DEMO_REGEX_H