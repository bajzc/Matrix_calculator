#ifndef DEMO_REGEX_H
#define DEMO_REGEX_H
int regex(const char* string);
#ifndef PCRE2_CODE_UNIT_WIDTH 8
#define PCRE2_CODE_UNIT_WIDTH 8
#endif  // !PCRE2_CODE_UNIT_WIDTH 8
#include <pcre2.h>
#include <stdbool.h>
#include "demo.h"
#include "errorf.h"
#include "hash.h"
#include "regex.h"
int errornumber;
PCRE2_SIZE erroroffset;
// NAME may be repeated with reserved word: "add sub mlp det inv"
const char* NamePattern = "(?'NAME'[a-zA-Z_][0-9a-zA-Z_]*)";
const char* FunctionPattern =
    "((?'ADD'add)\\(\\s*(?'ADD_NAMEA'([a-zA-Z_][0-9a-zA-Z_]*))(\\s*,\\s*)(?'"
    "ADD_NAMEB'([a-zA-Z_][0-9a-zA-Z_]*)))\\s*\\)"

    "|((?'SUB'sub)\\(\\s*(?'SUB_NAMEA'([a-zA-Z_][0-9a-zA-Z_]*))(\\s*,\\s*)(?'"
    "SUB_NAMEB'([a-zA-Z_][0-9a-zA-Z_]*))\\s*\\))"

    "|((?'MLP'mlp)\\(\\s*(?'MLP_NAMEA'([a-zA-Z_][0-9a-zA-Z_]*))(\\s*,\\s*)(?'"
    "MLP_NAMEB'([a-zA-Z_][0-9a-zA-Z_]*))\\s*\\))"

    "|((?'DET'det)\\(\\s*(?'DET_NAME'([a-zA-Z_][0-9a-zA-Z_]*))\\s*\\))"
    "|((?'INV'inv)\\(\\s*(?'INV_NAME'([a-zA-Z_][0-9a-zA-Z_]*))\\s*\\))";
const char* StatementPattern =
    "(?'NAME'[a-zA-Z_][0-9a-zA-Z_]*)\\s*=(?'MATRIX'(\\s*\\[)(\\s*(-?\\d+(,"
    "\\d+)*(\\.\\d+)?))+(\\s*(\\;)(\\s*-?\\d+(,\\d+)*(\\.\\d+)?)+)*("
    "\\s*\\]))";
const char* ColonPattern = "(;)";  // get the number of ';'
const char* NumberPattern = "(?'NUMBER'(-?\\d+(,\\d+)*(\\.\\d+)?))";

pcre2_match_data *NameMatchData, *FunctionMatchData, *StatementMatchData,
    *ColonMatchData, *NumberMatchData;
PCRE2_SIZE *NameOutVector, *FunctionOutVector, *StatementOutVector,
    *ColonOutVector, *NumberOutVector;
pcre2_code *Namere, *Functionre, *Statementre, *Colonre, *Numberre;
int Namerc, Functionrc, Statementrc, Colonrc, Numberrc;
#endif  // !DEMO_REGEX_H