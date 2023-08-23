%{

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "syms.h"
#include "config.h"
void yyerror(char const *);
int yylex(void);

%}

%define api.value.type union
%token <char*> t_clear t_clean t_quit
%token <double> t_num
%token <symvar *>  t_name t_fun t_matrix t_det
%token <matrix_t*> t_inv
%nterm <double> exp
%nterm <array_t *> array
%nterm <vector_t *>vector
%nterm <matrix_t *>matrix m_exp
%nterm <symvar *> matrix_stmt
%nterm command

%precedence '='
%left '['
%left ';'
%left ','
%left '-' '+'
%left '*' '/'
%precedence NEG /* negation--unary minus */
%right ']'
%right '^' '%'      /* exponentiation */

/* Generate the parser description file. */
%verbose
/* Enable run-time traces (yydebug). */
%define parse.trace
 
%%

// for continuous input
input:
  %empty
| input line
;


line:
  '\n'
| command '\n'       {                                  }
| m_exp '\n'         { matrix_t_print($1);              }
| exp '\n'           { printf ("exp:%g\n", $1); 			  }
| error '\n'         { yyerrok;                				  }
;

matrix_stmt:
t_name '=' m_exp    { $1->type=t_matrix;
                       $1->value.matrix=$3;
                       $$=$1;                           }
// FIXME free memory
//| t_matrix '=' m_exp{ $1->value.matrix=$3; $$=$1; }
;


matrix:
'[' exp ']'					 { array_t *a=array_init($2,0);a->count=0;
											 $$=matrix_init(vector_init(a));	}
| '[' vector ']' 		 { $$ = matrix_init($2);            }
;

vector:
	array							 { $$ = vector_init($1);            }
| vector ';' array   { $$ = vector_add($1,$3);          }
;

array:
exp ',' exp 			 	 { $$ = array_init($1,$3); 					}
| array ',' exp		 	 { $$ = array_add($1,$3);	 					}
;

exp:
  t_num
| t_name             { $$ = $1->value.var;              }
| t_name '=' exp     { $$ = $3; $1->value.var = $3;     }
| t_fun '(' exp ')'  { $$ = $1->value.fun ($3);         }

| t_det '(' t_matrix ')'
										 {$$=matrix_det($3->value.matrix);	}
										 
| m_exp '[' exp ']' '[' exp ']'  
										 { $$=matrix_get_entity($1,$3,$6);	}
											
| m_exp '[' exp ']' '[' exp ']' '=' exp
										 { if(ceil($3)==$3&&ceil($6)==$6)
											$1->matrix[(int)$3][(int)$6]=$9;  
											$$=$9;														}
											
| exp '+' exp        { $$ = $1 + $3;                    }
| exp '-' exp        { $$ = $1 - $3;                    }
| exp '*' exp        { $$ = $1 * $3;                    }
| exp '/' exp        { $$ = $1 / $3;                    }
| exp '%' exp				 { $$ = (int)$1 % (int)$3;					}
| '-' exp %prec NEG  { $$ = -$2;                        }
| exp '^' exp        { $$ = pow ($1, $3);               }
| '(' exp ')'        { $$ = $2;                         }
;

// matrix exp
// %nterm <matrix_t *>matrix m_exp
m_exp:
  matrix
| t_matrix              { $$=$1->value.matrix;                                                        } 
| matrix_stmt           { $$=$1->value.matrix;                                                        }
| t_inv '(' m_exp ')'   { $$=matrix_temp(); matrix_inverse($3,$$);														          }
| m_exp '+' m_exp 			{ $$=matrix_temp(); matrix_add($1,$3,$$);																				}
| m_exp '*' m_exp 			{ $$=matrix_temp(); matrix_times_reorder($1,$3,$$);															}
| '(' t_matrix ')'      { $$=$2->value.matrix;																																			  }
;

command:
  t_clear               { system(CLEAR);                 }
| t_clean               { puts("coming soon :)");        }
| t_quit                { exit(1);                       }
;

%%

double matrix_get_entity(matrix_t* matrix, double x, double y){
	printf("matrix:row:%u\tcol:%u\n",matrix->row,matrix->column);
	if(ceil(x)!=x || ceil(y)!=y){
		puts("Matrix index is not an integer");
		return NAN;
	}
	if(x>matrix->row || y>matrix->column || x<0 || y<0){
		puts("Matrix index out of range");
		return NAN;
	}
	
	return matrix->matrix[(size_t)x+1][(size_t)y+1];
}

matrix_t *matrix_temp(void){
  matrix_t* temp = malloc(sizeof(matrix_t));
  if (!temp) {
    puts("matrix_temp malloc error!");
    exit(-1);
  }
  temp->column = 1;
  temp->row = 1;
  matrix_calloc(temp);
  return temp;
}

matrix_t *matrix_init(vector_t *vector){
  if(!vector_check(vector)){
    yyerror("Abort");
    return (matrix_t*)NULL;
  }
  matrix_t* matrix = malloc(sizeof(matrix_t));
  matrix->column=vector->arrays[0]->count+1;
  matrix->row=vector->count+1;
  matrix_malloc(matrix);
  //FIXME consider use memcpy instead of a loop
//  for(int i=0;i<matrix->row;i++){
//    memcpy(matrix->matrix[i],vector->arrays[i]->num,sizeof(double)*(vector->arrays[i]->count)+1);
//  }
  for(int i=0;i<matrix->row;i++){
    for(int j=0;j<matrix->column;j++){
      matrix->matrix[i][j]=vector->arrays[i]->num[j];
    }
  }
  return matrix;
}

bool vector_check(vector_t* vector){ // check if all arrays have the same size
  if(vector->count==0) 							 // only one array in vector
    return true;
  int col=vector->arrays[0]->count;
  for(int i=1;i<=vector->count;i++){
    if(col!=vector->arrays[i]->count){
      printf("ERROR: vertical dimensions mismatch (1x%d vs 1x%d)\n",col,vector->arrays[i]->count);
      return false;
    }
  }
  return true;
}

vector_t* vector_init(array_t* arr){
	vector_t *vector=malloc(sizeof(vector_t) + 4 * sizeof(array_t*));
	if(!vector){
		puts("vector malloc error");
		exit(-1);
	}
	vector->size=4;
	vector->arrays[0]=arr;
	vector->count=0;
	return vector;
}

vector_t* vector_add(vector_t *vector, array_t *arr){
  if(!vector){
    puts("vector malloc error");
    exit(-1);
  }
  if(vector->count+1==vector->size){
    if(!realloc(vector,sizeof(vector_t) + (vector->size +4 ) * sizeof(array_t*))){
      puts("realloc error");
      exit(-1);
    }
    vector->size+=4;
  }
  vector->arrays[++vector->count]=arr;
  return vector;
}

array_t* array_init(double num1,double num2){
	array_t *array=malloc(sizeof(array_t) + 16 * sizeof(double));
	if(!array){
		printf("array malloc error");
		exit(-1);
	}
	array->size = 16;
	array->count=1;
	array->num[0]=num1;
	array->num[1]=num2;
	return array;
}

array_t* array_add(array_t *array, double num1){
	if(!array){
		puts("array malloc error");
		exit(-1);
	}
	if(array->count+1==array->size){
		if(!realloc(array,sizeof(array_t) + (array->size + 16) * sizeof(double))){
			puts("realloc error");
			exit(-1);
		}
		array->size+=16;
	}
	array->num[++array->count]=num1;
	return array;
}

struct init const funs[] ={
  { "atan", .value.fun=atan , 1},
  { "cos",  .value.fun=cos  , 1},
  { "exp",  .value.fun=exp  , 1},
  { "ln",   .value.fun=log  , 1},
  { "sin",  .value.fun=sin  , 1},
  { "sqrt", .value.fun=sqrt , 1},
  { 0, 0, 0},
};

void init_table(void){
  for (int i = 0; funs[i].name; i++)
    {
      symvar *ptr = putsym(funs[i].name,t_fun);
      ptr->value.fun = funs[i].value.fun;
    }
}

symvar *sym_table;

symvar *putsym(char const *name, int sym_type){
  symvar *res = (symvar*)malloc(sizeof(symvar));
  res->name = strdup (name);
  res->type = sym_type;
  res->value.var = NAN;
  res->next = sym_table;
  sym_table = res;
  return res;
}

symvar *getsym(char const *name){
  for (symvar *p = sym_table; p; p = p->next)
    if (strcmp(p->name, name) == 0)
      return p;
  return NULL;
}
