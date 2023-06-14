#define PCRE2_CODE_UNIT_WIDTH 8
#include "regex.h"
#include "demo.h"
#include "errorf.h"
#include "hash.h"
#include "cli.h"

int errornumber;
PCRE2_SIZE erroroffset;
// NAME may be repeated with reserved word: "add sub mlp det inv"
const char *NamePattern = "(?'NAME'[a-zA-Z_][0-9a-zA-Z_]*)";
const char *FunctionPattern =
	"((?'ADD'add)\\(\\s*(?'ADD_NAMEA'([a-zA-Z_][0-9a-zA-Z_]*))(\\s*,\\s*)(?'"
	"ADD_NAMEB'([a-zA-Z_][0-9a-zA-Z_]*)))\\s*\\)"

	"|((?'SUB'sub)\\(\\s*(?'SUB_NAMEA'([a-zA-Z_][0-9a-zA-Z_]*))(\\s*,\\s*)(?'"
	"SUB_NAMEB'([a-zA-Z_][0-9a-zA-Z_]*))\\s*\\))"

	"|((?'MLP'mlp)\\(\\s*(?'MLP_NAMEA'([a-zA-Z_][0-9a-zA-Z_]*))(\\s*,\\s*)(?'"
	"MLP_NAMEB'([a-zA-Z_][0-9a-zA-Z_]*))\\s*\\))"

	"|((?'DET'det)\\(\\s*(?'DET_NAME'([a-zA-Z_][0-9a-zA-Z_]*))\\s*\\))"
	"|((?'INV'inv)\\(\\s*(?'INV_NAME'([a-zA-Z_][0-9a-zA-Z_]*))\\s*\\))";

const char *StatementPattern =
	"(?'NAME'[a-zA-Z_][0-9a-zA-Z_]*)(\\s)*=(?'MATRIX'(\\s*\\[)(\\s*(-?\\d+(,"
	"\\d+)*(\\.\\d+)?))+(\\s*(\\;)(\\s*-?\\d+(,\\d+)*(\\.\\d+)?)+)*("
	"\\s*\\]))\\s*$";
const char *ColonPattern = "(;)"; // get the number of ';'
const char *NumberPattern = "(?'NUMBER'(-?\\d+(,\\d+)*(\\.\\d+)?))";

pcre2_match_data *NameMatchData, *FunctionMatchData, *StatementMatchData,
	*ColonMatchData, *NumberMatchData;
PCRE2_SIZE *NameOutVector, *FunctionOutVector, *StatementOutVector,
	*ColonOutVector, *NumberOutVector;
pcre2_code *Namere, *Functionre, *Statementre, *Colonre, *Numberre;
int Namerc, Functionrc, Statementrc, Colonrc, Numberrc;

char *FunctionNames[] = { "ADD", "SUB", "MLP", "DET", "INV" };

matrix_t *regex_creat_matrix(char *MatrixString)
{
	PCRE2_SIZE MatrixStringLength = strlen(MatrixString);
	Colonre = pcre2_compile((PCRE2_SPTR)ColonPattern, PCRE2_ZERO_TERMINATED,
				0, &errornumber, &erroroffset, NULL);
	ColonMatchData = pcre2_match_data_create_from_pattern(Colonre, NULL);
	Colonrc = pcre2_match(Colonre, (PCRE2_SPTR)MatrixString,
			      MatrixStringLength, 0, 0, ColonMatchData, NULL);
	if (Colonrc >= 0)
		ColonOutVector = pcre2_get_ovector_pointer(ColonMatchData);
	matrix_t *new = malloc(sizeof(matrix_t));
	if (Colonrc > 0) { // there is at least one colon
		PCRE2_SIZE MatrixLength = strlen(MatrixString);
		Numberre = pcre2_compile((const unsigned char *)NumberPattern,
					 PCRE2_ZERO_TERMINATED, 0, &errornumber,
					 &erroroffset, NULL);
		NumberMatchData =
			pcre2_match_data_create_from_pattern(Numberre, NULL);
		NumberOutVector = pcre2_get_ovector_pointer(NumberMatchData);
		// convert into num
		PCRE2_SIZE NumberOffset = 0;
		PCRE2_SIZE ColonOffset = 0;
		// the number of digits found before the colon
		unsigned int matrix_column = 0;
		unsigned int matrix_row = 0;
		while ((Numberrc = pcre2_match(Numberre,
					       (PCRE2_SPTR)MatrixString,
					       MatrixLength, NumberOffset, 0,
					       NumberMatchData, NULL)) > 0 &&
		       (int)NumberOutVector[1] <= ColonOutVector[0]) {
			NumberOutVector =
				pcre2_get_ovector_pointer(NumberMatchData);
			NumberOffset = NumberOutVector[1];
			matrix_column++;
		}
		while ((Colonrc = pcre2_match(Colonre, (PCRE2_SPTR)MatrixString,
					      MatrixLength, ColonOffset, 0,
					      ColonMatchData, NULL)) > 0) {
			ColonOutVector =
				pcre2_get_ovector_pointer(ColonMatchData);
			ColonOffset = ColonOutVector[1];
			matrix_row++;
		}
		matrix_row += 1; // last row without ';'
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
				Numberrc = pcre2_match(Numberre,
						       (PCRE2_SPTR)MatrixString,
						       MatrixLength,
						       NumberOffset, 0,
						       NumberMatchData, NULL);
				// Check if there is number of this place
				if (Numberrc < 0) {
					error_print(
						"Check if your matrix contains the same number of elements in each row");
					exit(-1);
				}
				NumberOutVector = pcre2_get_ovector_pointer(
					NumberMatchData);
				PCRE2_SPTR SubStringStart =
					(PCRE2_SPTR)MatrixString +
					NumberOutVector[0];
				PCRE2_SPTR SubStringEnd =
					(PCRE2_SPTR)MatrixString +
					NumberOutVector[1] - 1;
				new->matrix[i][j] =
					strtod((char *)SubStringStart,
					       (char **)&SubStringEnd);
				NumberOffset = NumberOutVector[1];
			}
		}
	} else { // 1 by N matrix: [1 2 3 4]
		PCRE2_SIZE MatrixLength = strlen(MatrixString);
		Numberre = pcre2_compile((PCRE2_SPTR)NumberPattern,
					 PCRE2_ZERO_TERMINATED, 0, &errornumber,
					 &erroroffset, NULL);
		NumberMatchData =
			pcre2_match_data_create_from_pattern(Numberre, NULL);
		NumberOutVector = pcre2_get_ovector_pointer(NumberMatchData);
		PCRE2_SIZE NumberOffset = 0;
		unsigned int matrix_column = 0;
		while ((Numberrc = pcre2_match(Numberre,
					       (PCRE2_SPTR)MatrixString,
					       MatrixLength, NumberOffset, 0,
					       NumberMatchData, NULL)) > 0) {
			NumberOutVector =
				pcre2_get_ovector_pointer(NumberMatchData);
			NumberOffset = NumberOutVector[1];
			matrix_column++;
		}
		new->row = 1;
		new->column = matrix_column;
		matrix_malloc(new);
		NumberOffset = 0;
		for (unsigned int j = 0; j < matrix_column; j++) {
			Numberrc = pcre2_match(Numberre,
					       (PCRE2_SPTR)MatrixString,
					       MatrixLength, NumberOffset, 0,
					       NumberMatchData, NULL);
			// Check if there is number of this place
			NumberOutVector =
				pcre2_get_ovector_pointer(NumberMatchData);
			PCRE2_SPTR SubStringStart =
				(PCRE2_SPTR)MatrixString + NumberOutVector[0];
			PCRE2_SPTR SubStringEnd = (PCRE2_SPTR)MatrixString +
						  NumberOutVector[1] - 1;
			new->matrix[0][j] = strtod((char *)SubStringStart,
						   (char **)&SubStringEnd);
			NumberOffset = NumberOutVector[1];
		}
	}
	return new;
}

// register a new matrix and/or check
int regex_matrix()
{
	char *Matrix = malloc(sizeof(char) * 1024 * 2);
	PCRE2_UCHAR *MatrixName =
		malloc(sizeof(PCRE2_UCHAR) * 128); // name of matrix
	uint32_t namecount;
	(void)pcre2_pattern_info(Statementre, PCRE2_INFO_NAMECOUNT, &namecount);

	PCRE2_SPTR Require_name = (PCRE2_SPTR) "NAME";
	PCRE2_SIZE NameLength = sizeof(MatrixName);
	int NameResult = pcre2_substring_copy_byname(
		StatementMatchData, Require_name, MatrixName, &NameLength);
	if (NameResult < 0) {
		printf("ERROR CODE:%d", NameResult);
		error_print("Matrix name copy error");
		return -1;
	}
	// get substring of matrix: [ ... (;) ... ]
	PCRE2_SIZE MatrixLength_require = sizeof(Matrix);
	int MatrixResult = pcre2_substring_get_byname(StatementMatchData,
						      (PCRE2_SPTR) "MATRIX",
						      (PCRE2_UCHAR **)&Matrix,
						      &MatrixLength_require);
	if (MatrixResult < 0) {
		printf("ERROR CODE:%d", NameResult);
		error_print("Matrix copy error");
		return -1;
	}

	if (!hash_have_name((char *)MatrixName)) {
		// haven't been registered
		matrix_t *new = regex_creat_matrix(Matrix);
		identifier_t *NewMatrix =
			hash_new_matrix((char *)MatrixName, new);
		matrix_t_print(NewMatrix->matrix);
	} else {
		identifier_t *OldMatrix = hash_find_matrix((char *)MatrixName);
		matrix_t *new = regex_creat_matrix(Matrix);
		matrix_free(OldMatrix->matrix);
		OldMatrix->matrix = new;
		matrix_t_print(OldMatrix->matrix);
	}
	return 0;
}

int regex(const char *string)
{
	bool isName = 0, isStatement = 0, isFunction = 0;
	PCRE2_SIZE StringLength = (PCRE2_SIZE)strlen((char *)string);

	// regex about name (may include function name)
	Namere = pcre2_compile((PCRE2_SPTR)NamePattern, PCRE2_ZERO_TERMINATED,
			       0, &errornumber, &erroroffset, NULL);
	NameMatchData = pcre2_match_data_create_from_pattern(Namere, NULL);
	Namerc = pcre2_match(Namere, (PCRE2_SPTR)string, StringLength, 0, 0,
			     NameMatchData, NULL);
	if (Namerc < 0) {
		pcre2_match_data_free(NameMatchData);
		pcre2_code_free(Namere);
		if (Namerc == PCRE2_ERROR_NOMATCH) {
			error_print("No name found (Not compliant input)");
			return -1;
		} else {
			error_print("Matching input error");
			return -1;
		}
	}
	isName = 1;
	NameOutVector = pcre2_get_ovector_pointer(NameMatchData);

	// regex about statement
	Statementre = pcre2_compile((PCRE2_SPTR)StatementPattern,
				    PCRE2_ZERO_TERMINATED, 0, &errornumber,
				    &erroroffset, NULL);
	StatementMatchData =
		pcre2_match_data_create_from_pattern(Statementre, NULL);
	Statementrc = pcre2_match(Statementre, (PCRE2_SPTR)string, StringLength,
				  0, 0, StatementMatchData, NULL);
	if (Statementrc < 0) {
		pcre2_match_data_free(StatementMatchData);
		pcre2_code_free(Statementre);
		if (Statementrc == PCRE2_ERROR_NOMATCH) {
			isStatement = 0;
		} else {
			printf("ERROR CODE:%d\n", Statementrc);
			error_print("Matching error");
			return -1;
		}
	} else {
		isStatement = 1;
	}

	// Go to add new matrix or alter the old one
	if (isStatement) {
		// Need the number of colon
		StatementOutVector =
			pcre2_get_ovector_pointer(StatementMatchData);
		regex_matrix();
		return 0;
	}

	// the entire input is ONE name, which means print the matrix
	// OR run some internal functions: quit list ...
	if (NameOutVector[1] == strlen(string)) {
		// internal functions: list clean quit(exit) help
		if (StringLength == 4 || StringLength == 5) {
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
				return 0;
			}
			if (strncmp(string, "exit", 4) == 0) {
				exit(1);
				return 0;
			}
			if (strncmp(string, "help", 4) == 0) {
				print_help_msg();
				return 0;
			}
			if (strncmp(string, "clear", 5) == 0) {
				system(CLEAR);
				return 0;
			}
		}
		if (!hash_have_name((char *)string)) {
			error_print("Matrix have not been initialized");
			return -1;
		}
		identifier_t *matrix_find = hash_find_matrix((char *)string);
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
		Functionre = pcre2_compile((PCRE2_SPTR)FunctionPattern,
					   PCRE2_ZERO_TERMINATED, 0,
					   &errornumber, &erroroffset, NULL);
		if (Functionre == NULL) {
			PCRE2_UCHAR buffer[256];
			pcre2_get_error_message(errornumber, buffer,
						sizeof(buffer));
			printf("Function pattern compilation failed at offset %d: %s\n",
			       (int)erroroffset, buffer);
			error_exit("PCRE2 compilation failed");
		}
		FunctionMatchData =
			pcre2_match_data_create_from_pattern(Functionre, NULL);
		Functionrc = pcre2_match(Functionre, string, StringLength, 0, 0,
					 FunctionMatchData, NULL);
		if (Functionrc < 0) {
			switch (Functionrc) {
			case PCRE2_ERROR_NOMATCH:
				error_print("Not compliant input");
				return -1;
				break;
			default:
				printf("ERROR CODE: %d\n", Functionrc);
				pcre2_match_data_free(FunctionMatchData);
				pcre2_code_free(Functionre);
				error_exit("Matching error");
				break;
			}
		}
		FunctionOutVector =
			pcre2_get_ovector_pointer(FunctionMatchData);

		PCRE2_SIZE namelength; // temporary store
		int WhichFunction = 0; // which function has been called
		while (0 != pcre2_substring_length_byname(
				    FunctionMatchData,
				    FunctionNames[WhichFunction],
				    &namelength)) {
			WhichFunction++;
		}
		PCRE2_UCHAR *name_A = NULL;
		PCRE2_UCHAR *name_B = NULL;
		// name_B is not needed for det() and inv()
		if (WhichFunction >= ADD && WhichFunction <= MLP) {
			name_B = malloc(sizeof(PCRE2_UCHAR) * 128);
		}
		name_A = malloc(sizeof(PCRE2_UCHAR) * 128);
		switch (WhichFunction) {
		case ADD: {
			int error_code_a = pcre2_substring_get_byname(
				FunctionMatchData, "ADD_NAMEA", &name_A,
				&namelength);
			int error_code_b = pcre2_substring_get_byname(
				FunctionMatchData, "ADD_NAMEB", &name_B,
				&namelength);
			if (!hash_have_name((char *)name_A)) {
				printf("Can't find matrix %s", (char *)name_A);
				error_print("No matrix matched");
				return -1;
			}
			if (!hash_have_name((char *)name_B)) {
				printf("Can't find matrix %s", (char *)name_B);
				error_print("No matrix matched");
				return -1;
			}
			identifier_t *matrix_A = hash_find_matrix(name_A);
			identifier_t *matrix_B = hash_find_matrix(name_B);
			identifier_t *matrix_ans = hash_find_matrix("ans");
			matrix_plus(matrix_A->matrix, matrix_B->matrix,
				    matrix_ans->matrix);
			matrix_t_print(matrix_ans->matrix);
			return 0;
		}
		case SUB: {
			int error_code_a = pcre2_substring_get_byname(
				FunctionMatchData, "SUB_NAMEA", &name_A,
				&namelength);
			int error_code_b = pcre2_substring_get_byname(
				FunctionMatchData, "SUB_NAMEB", &name_B,
				&namelength);
			if (!hash_have_name((char *)name_A)) {
				printf("Can't find matrix %s", (char *)name_A);
				error_print("No matrix matched");
				return -1;
			}
			if (!hash_have_name((char *)name_B)) {
				printf("Can't find matrix %s", (char *)name_B);
				error_print("No matrix matched");
				return -1;
			}
			identifier_t *matrix_A = hash_find_matrix(name_A);
			identifier_t *matrix_B = hash_find_matrix(name_B);
			identifier_t *matrix_ans = hash_find_matrix("ans");
			matrix_minus(matrix_A->matrix, matrix_B->matrix,
				     matrix_ans->matrix);
			matrix_t_print(matrix_ans->matrix);
			return 0;
		}
		case MLP: {
			int error_code_a = pcre2_substring_get_byname(
				FunctionMatchData, "MLP_NAMEA", &name_A,
				&namelength);
			int error_code_b = pcre2_substring_get_byname(
				FunctionMatchData, "MLP_NAMEB", &name_B,
				&namelength);
			if (!hash_have_name((char *)name_A)) {
				printf("Can't find matrix %s", (char *)name_A);
				error_print("No matrix matched");
				return -1;
			}
			if (!hash_have_name((char *)name_B)) {
				printf("Can't find matrix %s", (char *)name_B);
				error_print("No matrix matched");
				return -1;
			}
			identifier_t *matrix_A = hash_find_matrix(name_A);
			identifier_t *matrix_B = hash_find_matrix(name_B);
			identifier_t *matrix_ans = hash_find_matrix("ans");
			matrix_times_reorder(matrix_A->matrix, matrix_B->matrix,
					     matrix_ans->matrix);
			matrix_t_print(matrix_ans->matrix);
			return 0;
		}
		case DET: {
			int error_code_a = pcre2_substring_get_byname(
				FunctionMatchData, "DET_NAME", &name_A,
				&namelength);
			if (!hash_have_name((char *)name_A)) {
				printf("Can't find matrix %s", (char *)name_A);
				error_print("No matrix matched");
				return -1;
			}
			identifier_t *matrix_A = hash_find_matrix(name_A);
			double det;
			det = matrix_det(matrix_A->matrix);
			printf("det=%lf\n", det);
			return 0;
		}
		case INV: {
			int error_code_a = pcre2_substring_get_byname(
				FunctionMatchData, "INV_NAME", &name_A,
				&namelength);
			if (!hash_have_name((char *)name_A)) {
				printf("Can't find matrix %s", (char *)name_A);
				error_print("No matrix matched");
				return -1;
			}
			identifier_t *matrix_A = hash_find_matrix(name_A);
			identifier_t *matrix_ans = hash_find_matrix("ans");
			matrix_inverse(matrix_A->matrix, matrix_ans->matrix);
			matrix_t_print(matrix_ans->matrix);
			return 0;
		}
		}
	}
	return -1;
}
