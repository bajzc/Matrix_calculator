#define PCRE2_CODE_UNIT_WIDTH 8
#include "regex.h"

matrix_t* regex_creat_matrix(char* MatrixString) {
  matrix_t* new = malloc(sizeof(matrix_t));
  if (Colonrc > 0) {  // there is at least one colon
    PCRE2_SIZE MatrixLength = strlen(MatrixString);
    Numberre = pcre2_compile(NumberPattern, PCRE2_ZERO_TERMINATED, 0,
                             &errornumber, &erroroffset, NULL);
    NumberMatchData = pcre2_match_data_create_from_pattern(Numberre, NULL);
    NumberOutVector = pcre2_get_ovector_pointer(NumberMatchData);
    // convert into num
    PCRE2_SIZE NumberOffset = 0;
    PCRE2_SIZE ColonOffset = 0;
    // the number of digits found before the colon
    unsigned int matrix_column = 0;
    unsigned int matrix_row = 0;
    while ((Numberrc = pcre2_match(Numberre, MatrixString, MatrixLength,
                                   NumberOffset, 0, NumberMatchData, NULL)) >
               0 &&
           (int)NumberOutVector[1] <= ColonOutVector[0]) {
      NumberOutVector = pcre2_get_ovector_pointer(NumberMatchData);
      NumberOffset = NumberOutVector[1];
      matrix_column++;
    }
    while ((Colonrc = pcre2_match(Colonre, MatrixString, MatrixLength,
                                  ColonOffset, 0, ColonMatchData, NULL)) > 0) {
      ColonOutVector = pcre2_get_ovector_pointer(ColonMatchData);
      ColonOffset = ColonOutVector[1];
      matrix_row++;
    }
    matrix_row += 1;  // last row with out ';'
    new->column = matrix_column;
    new->row = matrix_row;
    matrix_malloc(new);
    if (new == NULL)
      error_exit("Memory allocate error");
    NumberOffset = ColonOffset = 0;
    for (unsigned int i = 0; i < matrix_row; i++) {
      for (unsigned int j = 0; j < matrix_column; j++) {
        Numberrc = pcre2_match(Numberre, MatrixString, MatrixLength,
                               NumberOffset, 0, NumberMatchData, NULL);
        // Check if there is number of this place
        if (Numberrc < 0)
          error_main("Check if your matrix is fully completed");
        NumberOutVector = pcre2_get_ovector_pointer(NumberMatchData);
        PCRE2_SPTR SubStringStart = MatrixString + NumberOutVector[0];
        PCRE2_SPTR SubStringEnd = MatrixString + NumberOutVector[1] - 1;
        new->matrix[i][j] = strtod(SubStringStart, &SubStringEnd);
        NumberOffset = NumberOutVector[1];
      }
    }
  } else {  // 1 by N matrix: [1 2 3 4]
    PCRE2_SIZE MatrixLength = strlen(MatrixString);
    Numberre = pcre2_compile(NumberPattern, PCRE2_ZERO_TERMINATED, 0,
                             &errornumber, &erroroffset, NULL);
    NumberMatchData = pcre2_match_data_create_from_pattern(Numberre, NULL);
    NumberOutVector = pcre2_get_ovector_pointer(NumberMatchData);
    PCRE2_SIZE NumberOffset = 0;
    unsigned int matrix_column = 0;
    while ((Numberrc = pcre2_match(Numberre, MatrixString, MatrixLength,
                                   NumberOffset, 0, NumberMatchData, NULL)) >
           0) {
      NumberOutVector = pcre2_get_ovector_pointer(NumberMatchData);
      NumberOffset = NumberOutVector[1];
      matrix_column++;
    }
    new->row = 1;
    new->column = matrix_column;
    matrix_malloc(new);
    NumberOffset = 0;
    for (unsigned int j = 0; j < matrix_column; j++) {
      Numberrc = pcre2_match(Numberre, MatrixString, MatrixLength, NumberOffset,
                             0, NumberMatchData, NULL);
      // Check if there is number of this place
      NumberOutVector = pcre2_get_ovector_pointer(NumberMatchData);
      PCRE2_SPTR SubStringStart = MatrixString + NumberOutVector[0];
      PCRE2_SPTR SubStringEnd = MatrixString + NumberOutVector[1] - 1;
      new->matrix[0][j] = strtod(SubStringStart, &SubStringEnd);
      NumberOffset = NumberOutVector[1];
    }
  }
  return new;
}

// register a new matrix and/or check
int regex_matrix() {
  char* Matrix = malloc(sizeof(char) * 1024);
  PCRE2_UCHAR* MatrixName =
      malloc(sizeof(PCRE2_UCHAR) * 128);  // name of matrix
  uint32_t namecount;
  (void)pcre2_pattern_info(Statementre, PCRE2_INFO_NAMECOUNT, &namecount);
  if (namecount == 0) {
    pcre2_code_free(Statementre);
    pcre2_match_data_free(StatementMatchData);
    error_main("Matrix register error (No matrix found in input)");
  } else {
    PCRE2_SPTR Require_name = "NAME";
    PCRE2_SIZE NameLength = sizeof MatrixName;
    int NameResult = pcre2_substring_copy_byname(
        StatementMatchData, Require_name, MatrixName, &NameLength);
    if (NameResult < 0) {
      printf("ERROR CODE:%d", NameResult);
      error_exit("Matrix name copy error");
    }
    // get substring of matrix: [ ... (;) ... ]
    PCRE2_SPTR Require_matrix = "MATRIX";
    PCRE2_SIZE MatrixLength_require = sizeof(Matrix);
    int MatrixResult = pcre2_substring_get_byname(
        StatementMatchData, Require_matrix, &Matrix, &MatrixLength_require);
    if (MatrixResult < 0) {
      printf("ERROR CODE:%d", NameResult);
      error_exit("Matrix copy error");
    }
  }
  if (!hash_have_name((char*)MatrixName)) {
    // haven't been registered
    matrix_t* new = regex_creat_matrix(Matrix);
    identifier_t* NewMatrix = hash_new_matrix(MatrixName, new);
  } else {
    identifier_t* OldMatrix = hash_find_matrix(MatrixName);
    matrix_t* new = regex_creat_matrix(Matrix);
    matrix_free(OldMatrix->matrix);
    OldMatrix->matrix = new;
  }
}

int regex(const char* string) {
  bool isName = 0, isStatement = 0, isFunction = 0;
  PCRE2_SIZE StringLength = (PCRE2_SIZE)strlen((char*)string);

  // regex about name (may include function name)
  Namere = pcre2_compile(NamePattern, PCRE2_ZERO_TERMINATED, 0, &errornumber,
                         &erroroffset, NULL);
  NameMatchData = pcre2_match_data_create_from_pattern(Namere, NULL);
  Namerc = pcre2_match(Namere, string, StringLength, 0, 0, NameMatchData, NULL);
  if (Namerc < 0) {
    pcre2_match_data_free(NameMatchData);
    pcre2_code_free(Namere);
    if (Namerc == PCRE2_ERROR_NOMATCH) {
      error_main("No name found (Not compliant input)");
      return -1;
    } else {
      error_exit("Matching input error");
    }
  }
  isName = 1;
  NameOutVector = pcre2_get_ovector_pointer(NameMatchData);

  // regex about statement
  Statementre = pcre2_compile(StatementPattern, PCRE2_ZERO_TERMINATED, 0,
                              &errornumber, &erroroffset, NULL);
  StatementMatchData = pcre2_match_data_create_from_pattern(Statementre, NULL);
  Statementrc = pcre2_match(Statementre, string, StringLength, 0, 0,
                            StatementMatchData, NULL);
  if (Statementrc < 0) {
    pcre2_match_data_free(StatementMatchData);
    pcre2_code_free(Statementre);
    if (Namerc == PCRE2_ERROR_NOMATCH) {
      isStatement = 0;
    } else {
      error_exit("Matching error");
    }
  } else {
    isStatement = 1;
  }
  if (isStatement) {
    // Need the number of colon
    Colonre = pcre2_compile(ColonPattern, PCRE2_ZERO_TERMINATED, 0,
                            &errornumber, &erroroffset, NULL);
    ColonMatchData = pcre2_match_data_create_from_pattern(Colonre, NULL);
    Colonrc =
        pcre2_match(Colonre, string, StringLength, 0, 0, ColonMatchData, NULL);
    if (Colonrc >= 0)
      ColonOutVector = pcre2_get_ovector_pointer(ColonMatchData);
    StatementOutVector = pcre2_get_ovector_pointer(StatementMatchData);
    regex_matrix();
  }

  return 0;
}