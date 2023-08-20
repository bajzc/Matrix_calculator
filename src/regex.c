//
// Created by LiZeCheng-Jason on 2023-06-25.
//
/*********************************************************
 * START                                                  *
 *   |                                                    *
 * FIND 'NAME'                                            *
 *  Y|N---exit()                                          *
 *   |                                                    *
 * FIND 'STATEMENT'                                       *
 *  Y|N---IF NAME[-1]==strlen(INPUT)                      *
 *   |                        |                           *
 *   |                       Y|N----FIND 'FUNCTION'       *
 * regex_matrix()             |              Y|N---exit() *
 *   |                        |               |           *
 * ADD to HASH                |         call 'FUNCTION'   *
 *                            |                           *
 *              matrix_print() OR key wrods               *
 *********************************************************/
#include "config.h"
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
    "(?'NAME'[a-zA-Z_][0-9a-zA-Z_]*)(\\s)*=(?'MATRIX'(\\s*\\[)(\\s*(-?\\d+(,"
    "\\d+)*(\\.\\d+)?))+(\\s*(\\;)(\\s*-?\\d+(,\\d+)*(\\.\\d+)?)+)*("
    "\\s*\\]))\\s*$";
const char* ColonPattern = "(;)";  // get the number of ';'
const char* NumberPattern = "(?'NUMBER'(-?\\d+(,\\d+)*(\\.\\d+)?))";

static pcre2_match_data** MatchData = NULL;
static PCRE2_SIZE** OutVector = NULL;
static pcre2_code** re = NULL;
static int* rc = NULL;
static char* FunctionNames[MATRIX_FUNCTION_NUMBER] = {"ADD", "SUB", "MLP", "DET",
                                               "INV"};
matrix_t* regex_creat_matrix(char* MatrixString) {
  PCRE2_SIZE MatrixStringLength = strlen(MatrixString);
  re[COLON] = pcre2_compile((PCRE2_SPTR)ColonPattern, PCRE2_ZERO_TERMINATED, 0,
                            &errornumber, &erroroffset, NULL);
  MatchData[COLON] = pcre2_match_data_create_from_pattern(re[COLON], NULL);
  rc[COLON] = pcre2_match(re[COLON], (PCRE2_SPTR)MatrixString,
                          MatrixStringLength, 0, 0, MatchData[COLON], NULL);
  if (rc[COLON] >= 0)
    OutVector[COLON] = pcre2_get_ovector_pointer(MatchData[COLON]);
  matrix_t* new = malloc(sizeof(matrix_t));
  if (rc[COLON] > 0) {  // there is at least one colon
    PCRE2_SIZE MatrixLength = strlen(MatrixString);
    re[NUMBER] = pcre2_compile((const unsigned char*)NumberPattern,
                               PCRE2_ZERO_TERMINATED, 0, &errornumber,
                               &erroroffset, NULL);
    MatchData[NUMBER] = pcre2_match_data_create_from_pattern(re[NUMBER], NULL);
    OutVector[NUMBER] = pcre2_get_ovector_pointer(MatchData[NUMBER]);
    // convert into num
    PCRE2_SIZE NumberOffset = 0;
    PCRE2_SIZE ColonOffset = 0;
    // the number of digits found before the colon
    unsigned int matrix_column = 0;
    unsigned int matrix_row = 0;
    while ((rc[NUMBER] =
                pcre2_match(re[NUMBER], (PCRE2_SPTR)MatrixString, MatrixLength,
                            NumberOffset, 0, MatchData[NUMBER], NULL)) > 0 &&
           (int)OutVector[NUMBER][1] <= OutVector[COLON][0]) {
      OutVector[NUMBER] = pcre2_get_ovector_pointer(MatchData[NUMBER]);
      NumberOffset = OutVector[NUMBER][1];
      matrix_column++;
    }
    while ((rc[COLON] =
                pcre2_match(re[COLON], (PCRE2_SPTR)MatrixString, MatrixLength,
                            ColonOffset, 0, MatchData[COLON], NULL)) > 0) {
      OutVector[COLON] = pcre2_get_ovector_pointer(MatchData[COLON]);
      ColonOffset = OutVector[COLON][1];
      matrix_row++;
    }
    matrix_row += 1;  // last row without ';'
    new->column = matrix_column;
    new->row = matrix_row;
    matrix_calloc(new);
    printf("row:%d column:%d\n", matrix_row, matrix_column);
    if (new == NULL) {
      error_print("Memory allocate error");
      (void)getchar();
      exit(-1);
    }
    NumberOffset = ColonOffset = 0;
    for (unsigned int i = 0; i < matrix_row; i++) {
      for (unsigned int j = 0; j < matrix_column; j++) {
        rc[NUMBER] =
            pcre2_match(re[NUMBER], (PCRE2_SPTR)MatrixString, MatrixLength,
                        NumberOffset, 0, MatchData[NUMBER], NULL);
        // Check if there is number of this place
        if (rc[NUMBER] < 0) {
          error_print(
              "Check if your matrix contains the same number "
              "of elements in "
              "each row");
          exit(-1);
        }
        OutVector[NUMBER] = pcre2_get_ovector_pointer(MatchData[NUMBER]);
        PCRE2_SPTR SubStringStart =
            (PCRE2_SPTR)MatrixString + OutVector[NUMBER][0];
        PCRE2_SPTR SubStringEnd =
            (PCRE2_SPTR)MatrixString + OutVector[NUMBER][1] - 1;
        new->matrix[i][j] =
            strtod((char*)SubStringStart, (char**)&SubStringEnd);
        NumberOffset = OutVector[NUMBER][1];
      }
    }
  } else {  // 1 by N matrix: [1 2 3 4]
    PCRE2_SIZE MatrixLength = strlen(MatrixString);
    re[NUMBER] = pcre2_compile((PCRE2_SPTR)NumberPattern, PCRE2_ZERO_TERMINATED,
                               0, &errornumber, &erroroffset, NULL);
    MatchData[NUMBER] = pcre2_match_data_create_from_pattern(re[NUMBER], NULL);
    OutVector[NUMBER] = pcre2_get_ovector_pointer(MatchData[NUMBER]);
    PCRE2_SIZE NumberOffset = 0;
    unsigned int matrix_column = 0;
    while ((rc[NUMBER] =
                pcre2_match(re[NUMBER], (PCRE2_SPTR)MatrixString, MatrixLength,
                            NumberOffset, 0, MatchData[NUMBER], NULL)) > 0) {
      OutVector[NUMBER] = pcre2_get_ovector_pointer(MatchData[NUMBER]);
      NumberOffset = OutVector[NUMBER][1];
      matrix_column++;
    }
    new->row = 1;
    new->column = matrix_column;
    matrix_malloc(new);
    NumberOffset = 0;
    for (unsigned int j = 0; j < matrix_column; j++) {
      rc[NUMBER] =
          pcre2_match(re[NUMBER], (PCRE2_SPTR)MatrixString, MatrixLength,
                      NumberOffset, 0, MatchData[NUMBER], NULL);
      // Check if there is number of this place
      OutVector[NUMBER] = pcre2_get_ovector_pointer(MatchData[NUMBER]);
      PCRE2_SPTR SubStringStart =
          (PCRE2_SPTR)MatrixString + OutVector[NUMBER][0];
      PCRE2_SPTR SubStringEnd =
          (PCRE2_SPTR)MatrixString + OutVector[NUMBER][1] - 1;
      new->matrix[0][j] = strtod((char*)SubStringStart, (char**)&SubStringEnd);
      NumberOffset = OutVector[NUMBER][1];
    }
  }
  return new;
}

// register a new matrix and/or check
int regex_matrix() {
  PCRE2_SIZE MatrixLength_require = 1024 * 2;
  char* Matrix = malloc(sizeof(char) * MatrixLength_require);
  PCRE2_SIZE NameLength = 128;
  PCRE2_UCHAR* MatrixName =
      malloc(sizeof(PCRE2_UCHAR) * NameLength);  // name of matrix
  PCRE2_SPTR Require_name = (PCRE2_SPTR) "NAME";
  int NameResult = pcre2_substring_copy_byname(
      MatchData[STATEMENT], Require_name, MatrixName, &NameLength);
  if (NameResult < 0) {
    printf("ERROR CODE:%d", NameResult);
    error_print("Matrix name copy error");
    return -1;
  }

  // get substring of matrix: [ ... (;) ... ]
  int MatrixResult =
      pcre2_substring_get_byname(MatchData[STATEMENT], (PCRE2_SPTR) "MATRIX",
                                 (PCRE2_UCHAR**)&Matrix, &MatrixLength_require);
  if (MatrixResult < 0) {
    printf("ERROR CODE:%d", NameResult);
    error_print("Matrix copy error");
    return -1;
  }

  if (!hash_have_name((char*)MatrixName)) {
    // haven't been registered
    matrix_t* new = regex_creat_matrix(Matrix);
    identifier_t* NewMatrix = hash_new_matrix((char*)MatrixName, new);
    matrix_t_print(NewMatrix->matrix);
  } else {
    identifier_t* OldMatrix = hash_find_matrix((char*)MatrixName);
    matrix_t* new = regex_creat_matrix(Matrix);
    matrix_free(OldMatrix->matrix);
    OldMatrix->matrix = new;
    matrix_t_print(OldMatrix->matrix);
  }
  return 0;
}

void regex_free(void) {
  for (int i = 0; i < REGEX_OBJ_NUMBER; i++) {
    if (rc[i] != 1145) {
      pcre2_match_data_free(MatchData[i]);
      pcre2_code_free(re[i]);
    }
  }
}

// need to make sure all regex pointers are freed before running this function
void regex_malloc_all(void) {
  MatchData =
      (pcre2_match_data**)malloc(sizeof(pcre2_match_data*) * REGEX_OBJ_NUMBER);
  OutVector = (PCRE2_SIZE**)malloc(sizeof(PCRE2_SIZE*) * REGEX_OBJ_NUMBER);
  re = (pcre2_code**)malloc(sizeof(pcre2_code*) * REGEX_OBJ_NUMBER);
  // rc will be change once we use any of the variable above,
  // regex_free() will free MatchData, re and rc
  // by checking the value is 1145 or not
  rc = (int*)malloc(sizeof(int) * REGEX_OBJ_NUMBER);
  for (int i = 0; i < REGEX_OBJ_NUMBER; i++) {
    rc[i] = 1145;
  }
}

int regex(const char* string) {
  bool isName = 0, isStatement = 0;
  PCRE2_SIZE StringLength = (PCRE2_SIZE)strlen((char*)string);
  regex_free();
  regex_malloc_all();

  // regex about name (may include function name)
  re[NAME] = pcre2_compile((PCRE2_SPTR)NamePattern, PCRE2_ZERO_TERMINATED, 0,
                           &errornumber, &erroroffset, NULL);
  MatchData[NAME] = pcre2_match_data_create_from_pattern(re[NAME], NULL);
  rc[NAME] = pcre2_match(re[NAME], (PCRE2_SPTR)string, StringLength, 0, 0,
                         MatchData[NAME], NULL);
  if (rc[NAME] < 0) {
    if (rc[NAME] == PCRE2_ERROR_NOMATCH) {
      error_print("Unrecognized command-line option");
      return -1;
    } else {
      error_print("Matching input error");
      return -1;
    }
  }
  isName = 1;
  OutVector[NAME] = pcre2_get_ovector_pointer(MatchData[NAME]);

  // regex about statement
  re[STATEMENT] =
      pcre2_compile((PCRE2_SPTR)StatementPattern, PCRE2_ZERO_TERMINATED, 0,
                    &errornumber, &erroroffset, NULL);
  MatchData[STATEMENT] =
      pcre2_match_data_create_from_pattern(re[STATEMENT], NULL);
  rc[STATEMENT] = pcre2_match(re[STATEMENT], (PCRE2_SPTR)string, StringLength,
                              0, 0, MatchData[STATEMENT], NULL);
  if (rc[STATEMENT] < 0) {
    if (rc[STATEMENT] == PCRE2_ERROR_NOMATCH) {
      isStatement = 0;
    } else {
      printf("ERROR CODE:%d\n", rc[STATEMENT]);
      error_print("Matching error");
      return -1;
    }
  } else {
    isStatement = 1;
  }

  // Go to add new matrix or alter the old one
  if (isStatement) {
    // Need the number of colon
    OutVector[STATEMENT] = pcre2_get_ovector_pointer(MatchData[STATEMENT]);
    regex_matrix();
    return 0;
  }

  // the entire input is ONE name, which means print the matrix
  // OR run some internal functions: quit list ...
  
  if (OutVector[NAME][1] == strlen(string)) {
    // internal functions: list clean quit(exit) help
    if (strncmp(string, "list", 4) == 0) {
      hash_print_all_matrix();
      return 0;
    }
    if (strncmp(string, "clean", 5) == 0) {
      hash_delete_all();
      return 0;
    }
    if (strncmp(string, "quit", 4) == 0) {
      exit(1);
    }
    if (strncmp(string, "exit", 4) == 0) {
      exit(1);
    }
    if (strncmp(string, "help", 4) == 0) {
      print_help_msg();
      return 0;
    }
    if (strncmp(string, "clear", 5) == 0) {
      system(CLEAR);
      return 0;
    }
    if (!hash_have_name((char*)string)) {
      error_print("No matrix found");
      return -1;
    }
    identifier_t* matrix_find = hash_find_matrix((char*)string);
    if (matrix_find->matrix == NULL) {
      printf("NO DATA\n");
      return -1;
    } else {
      matrix_t_print(matrix_find->matrix);
    }
    return 0;
  }

  // call function
  if (isName && !isStatement) {
    re[FUNCTION] =
        pcre2_compile((PCRE2_SPTR)FunctionPattern, PCRE2_ZERO_TERMINATED, 0,
                      &errornumber, &erroroffset, NULL);
    if (re[FUNCTION] == NULL) {
      PCRE2_UCHAR buffer[256];
      pcre2_get_error_message(errornumber, buffer, sizeof(buffer));
      printf("Function pattern compilation failed at offset %d: %s\n",
             (int)erroroffset, buffer);
      error_exit("PCRE2 compilation failed");
    }
    MatchData[FUNCTION] =
        pcre2_match_data_create_from_pattern(re[FUNCTION], NULL);
    rc[FUNCTION] = pcre2_match(re[FUNCTION], (PCRE2_SPTR)string, StringLength,
                               0, 0, MatchData[FUNCTION], NULL);
    if (rc[FUNCTION] < 0) {
      switch (rc[FUNCTION]) {
        case PCRE2_ERROR_NOMATCH:
          error_print("Not compliant input");
          return -1;
          break;
        default:
          printf("ERROR CODE: %d\n", rc[FUNCTION]);
          error_exit("Matching error");
          break;
      }
    }
    OutVector[FUNCTION] = pcre2_get_ovector_pointer(MatchData[FUNCTION]);

    PCRE2_SIZE namelength;  // temporary store
    int WhichFunction = 0;  // which function has been called
    while (0 != pcre2_substring_length_byname(
                    MatchData[FUNCTION],
                    (PCRE2_SPTR)FunctionNames[WhichFunction], &namelength)) {
      WhichFunction++;
    }
    PCRE2_UCHAR* name_A = NULL;
    PCRE2_UCHAR* name_B = NULL;
    // name_B is not required for det() and inv()
    if (WhichFunction >= ADD && WhichFunction <= MLP) {
      name_B = malloc(sizeof(PCRE2_UCHAR) * 128);
    }
    name_A = malloc(sizeof(PCRE2_UCHAR) * 128);
    switch (WhichFunction) {
      case ADD: {
        TWO_MATRIX((PCRE2_SPTR) "ADD_NAMEA", (PCRE2_SPTR) "ADD_NAMEB", name_A,
                   name_B, namelength, matrix_add);
        return 0;
      }
      case SUB: {
        TWO_MATRIX((PCRE2_SPTR) "SUB_NAMEA", (PCRE2_SPTR) "SUB_NAMEB", name_A,
                   name_B, namelength, matrix_sub);
        return 0;
      }
      case MLP: {
        TWO_MATRIX((PCRE2_SPTR) "MLP_NAMEA", (PCRE2_SPTR) "MLP_NAMEB", name_A,
                   name_B, namelength, matrix_times_reorder);
        return 0;
      }
      case DET: {
        (void)pcre2_substring_get_byname(
            MatchData[FUNCTION], (PCRE2_SPTR) "DET_NAME", &name_A, &namelength);
        if (!hash_have_name((char*)name_A)) {
          printf("Can't find matrix %s", (char*)name_A);
          error_print("No matrix matched");
          return -1;
        }
        identifier_t* matrix_A = hash_find_matrix((char*)name_A);
        double det;
        det = matrix_det(matrix_A->matrix);
        printf("det=%lf\n", det);
        return 0;
      }
      case INV: {
        (void)pcre2_substring_get_byname(MatchData[FUNCTION],
                                         (PCRE2_SPTR8) "INV_NAME", &name_A,
                                         &namelength);
        if (!hash_have_name((char*)name_A)) {
          printf("Can't find matrix %s", (char*)name_A);
          error_print("No matrix matched");
          return -1;
        }
        identifier_t* matrix_A = hash_find_matrix((char*)name_A);
        identifier_t* matrix_ans = hash_find_matrix("ans");
        matrix_inverse(matrix_A->matrix, matrix_ans->matrix);
        matrix_t_print(matrix_ans->matrix);
        return 0;
      }
    }
  }
  return -1;
}
