#include "command.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "label.h"
#include "parse.h"

/** Copy the given string to a dynamically allocated character array.
    @param str the string to copy.
    @return the dynamically allocated copy of the string.
*/
static char *copyString( char const *str )
{
  char *cpy = (char *) malloc( strlen( str ) + 1 );
  return strcpy( cpy, str );
}

////////////////////////////////////////////////////////////////////////////////
//If Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** condition to check for if statement */
  char *condition;

  /**place to jump to in code */
  char *go_to;

} IfCommand;

/**
  This function will destroy the IfCommand Struct.
  @param IfCommand cmd
*/
static void destroyIf(Command *cmd)
{
  IfCommand *this = (IfCommand *)cmd;
  free(this->condition);
  free(this->go_to);
  free(this);
}

// Execute function for the If command
static int executeIf( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  IfCommand *this = (IfCommand *)cmd;

  char *val = getenv(this->condition);

  if (val == NULL) {
    fprintf(stderr, "Undefined variable: %s (line %d)\n", this->condition, this->line);
    exit(1);
  }

  if (strcmp("", val) != 0) {
    int command_to_jump_to = findLabel(labelMap, this->go_to);
    if (command_to_jump_to == -1) {
      fprintf(stderr, "Undefined label: %s (line %d)\n", this->go_to, this->line);
      exit(1);
    }
    return command_to_jump_to;
  } else {
    return pc + 1;
  }
}

/** Make a command that runs the if statement.
    @param condition, check before entering if
    @param go_to, place to jump to in code
    @return a new Command that implements go to.
 */
static Command *makeIf(char const *condition, char const *go_to)
{
  // Allocate space for the IfCommand object
  IfCommand *this = (IfCommand *) malloc(sizeof(IfCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeIf;
  this->line = getLineNumber();
  this->destroy = destroyIf;

  // Make a copy of the arguments.
  this->condition = copyString(condition);
  this->go_to = copyString(go_to);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Goto Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of label to go to */
  char *label;

} GoToCommand;

/**
  This function will destroy the GoToCommand Struct.
  @param GoToCommand cmd
*/
static void destroyGoTo(Command *cmd)
{
  GoToCommand *this = (GoToCommand *)cmd;
  free(this->label);
  free(this);
}

// Execute function for the GoTo command
static int executeGoTo( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  GoToCommand *this = (GoToCommand *)cmd;

  int command_to_jump_to = findLabel(labelMap, this->label);
  
  if (command_to_jump_to == -1) {
    fprintf(stderr, "Undefined label: %s (line %d)\n", this->label, this->line);
    exit(1);
  }

  return command_to_jump_to;
}

/** Makes the goto command.
    @param label, the place to jump to.
    @return a new Command that implements go to.
 */
static Command *makeGoTo(char const *label)
{
  // Allocate space for the GoToCommand object
  GoToCommand *this = (GoToCommand *) malloc(sizeof(GoToCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeGoTo;
  this->line = getLineNumber();
  this->destroy = destroyGoTo;

  // Make a copy of the arguments.
  this->label = copyString(label);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Less Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of variable the sum will be stored in */
  char *var;
  
  /** Value of 1st arg */
  char *val_1;
  
  /** Value of 2nd arg */
  char *val_2;
  
} LessCommand;

/**
  This function will destroy the LessCommand Struct.
  @param LessCommand cmd
*/
static void destroyLess(Command *cmd)
{
  LessCommand *this = (LessCommand *)cmd;
  free(this->var);
  free(this->val_1);
  free(this->val_2);
  free(this);
}

// Execute function for the less than command
static int executeLess( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  LessCommand *this = (LessCommand *)cmd;

  int value = 0;
  long value_1 = 0;
  long value_2 = 0;
  
  long less = 0;
  
  if (this->val_1[0] == '"') {
    if (sscanf((this->val_1 + 1), "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_1 = getenv(this->val_1);
    if (str_1 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_1, this->line);
      exit(1);
    }
    if (sscanf(str_1, "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  if (this->val_2[0] == '"') {
    if (sscanf((this->val_2 + 1), "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_2 = getenv(this->val_2);
    if (str_2 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_2, this->line);
      exit(1);
    }
    if (sscanf(str_2, "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  char less_str[MAX_TOKEN + 1] = "";
  
  if (value_1 < value_2) {
    less = 1;
    sprintf(less_str, "%ld", less);
  }
  
  value = setenv(this->var, less_str, 1);

  return pc + 1;
}

/** Make a command that sees if the first value is less than the second one.
    @param var, the variable to store the result in
    @param val_1, the first value to compare
    @param val_2, the second value
    @return a new Command that implements less than.
 */
static Command *makeLess(char const *var, char const *val_1, char const *val_2)
{
  // Allocate space for the LessCommand object
  LessCommand *this = (LessCommand *) malloc(sizeof(LessCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeLess;
  this->line = getLineNumber();
  this->destroy = destroyLess;

  // Make a copy of the arguments.
  this->var = copyString(var);
  this->val_1 = copyString(val_1);
  this->val_2 = copyString(val_2);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Eq Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of variable the sum will be stored in */
  char *var;
  
  /** Value of 1st arg */
  char *val_1;
  
  /** Value of 2nd arg */
  char *val_2;
  
} EqCommand;

/**
  This function will destroy the EqCommand Struct.
  @param EqCommand cmd
*/
static void destroyEq(Command *cmd)
{
  EqCommand *this = (EqCommand *)cmd;
  free(this->var);
  free(this->val_1);
  free(this->val_2);
  free(this);
}

// Execute function for the equals command
static int executeEq( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  EqCommand *this = (EqCommand *)cmd;

  int value = 0;
  long value_1 = 0;
  long value_2 = 0;
  
  long eq = 0;
  
  if (this->val_1[0] == '"') {
    if (sscanf((this->val_1 + 1), "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_1 = getenv(this->val_1);
    if (str_1 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_1, this->line);
      exit(1);
    }
    if (sscanf(str_1, "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  if (this->val_2[0] == '"') {
    if (sscanf((this->val_2 + 1), "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_2 = getenv(this->val_2);
    if (str_2 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_2, this->line);
      exit(1);
    }
    if (sscanf(str_2, "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  char eq_str[MAX_TOKEN + 1] = "";
  
  if (value_1 == value_2) {
    eq = 1;
    sprintf(eq_str, "%ld", eq);
  }
  
  value = setenv(this->var, eq_str, 1);

  return pc + 1;
}

/** Make a command that implements the equals command.
    @param var, the variable to store the result in.
    @param val_1, the first value
    @param val_2, the second value
    @return a new Command that implements equals.
 */
static Command *makeEq(char const *var, char const *val_1, char const *val_2)
{
  // Allocate space for the EqCommand object
  EqCommand *this = (EqCommand *) malloc(sizeof(EqCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeEq;
  this->line = getLineNumber();
  this->destroy = destroyEq;

  // Make a copy of the arguments.
  this->var = copyString(var);
  this->val_1 = copyString(val_1);
  this->val_2 = copyString(val_2);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Mod Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of variable the sum will be stored in */
  char *var;
  
  /** Value of 1st arg */
  char *val_1;
  
  /** Value of 2nd arg */
  char *val_2;
  
} ModCommand;

/**
  This function will destroy the ModCommand Struct.
  @param ModCommand cmd
*/
static void destroyMod(Command *cmd)
{
  ModCommand *this = (ModCommand *)cmd;
  free(this->var);
  free(this->val_1);
  free(this->val_2);
  free(this);
}

// Execute function for the modular command
static int executeMod( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  ModCommand *this = (ModCommand *)cmd;

  int value = 0;
  long value_1 = 0;
  long value_2 = 0;
  
  long mod = 0;
  
  if (this->val_1[0] == '"') {
    if (sscanf((this->val_1 + 1), "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_1 = getenv(this->val_1);
    if (str_1 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_1, this->line);
      exit(1);
    }
    if (sscanf(str_1, "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  if (this->val_2[0] == '"') {
    if (sscanf((this->val_2 + 1), "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_2 = getenv(this->val_2);
    if (str_2 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_2, this->line);
      exit(1);
    }
    if (sscanf(str_2, "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  mod = value_1 % value_2;
  
  char mod_str[MAX_TOKEN + 1];
  
  sprintf(mod_str, "%ld", mod);
  
  value = setenv(this->var, mod_str, 1);

  return pc + 1;
}

/** Make a command that implements the mod command.
    @param var, the variable to store the result in.
    @param val_1, the first value
    @param val_2, the second value
    @return a new Command that implements modular.
 */
static Command *makeMod(char const *var, char const *val_1, char const *val_2)
{
  // Allocate space for the ModCommand object
  ModCommand *this = (ModCommand *) malloc(sizeof(ModCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeMod;
  this->line = getLineNumber();
  this->destroy = destroyMod;

  // Make a copy of the arguments.
  this->var = copyString(var);
  this->val_1 = copyString(val_1);
  this->val_2 = copyString(val_2);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Div Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of variable the sum will be stored in */
  char *var;
  
  /** Value of 1st arg */
  char *val_1;
  
  /** Value of 2nd arg */
  char *val_2;
  
} DivCommand;

/**
  This function will destroy the DivCommand Struct.
  @param DivCommand cmd
*/
static void destroyDiv(Command *cmd)
{
  DivCommand *this = (DivCommand *)cmd;
  free(this->var);
  free(this->val_1);
  free(this->val_2);
  free(this);
}

// Execute function for the divide command
static int executeDiv( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  DivCommand *this = (DivCommand *)cmd;

  int value = 0;
  long value_1 = 0;
  long value_2 = 0;
  
  long quotient = 0;
  
  if (this->val_1[0] == '"') {
    if (sscanf((this->val_1 + 1), "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_1 = getenv(this->val_1);
    if (str_1 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_1, this->line);
      exit(1);
    }
    if (sscanf(str_1, "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  if (this->val_2[0] == '"') {
    if (sscanf((this->val_2 + 1), "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_2 = getenv(this->val_2);
    if (str_2 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_2, this->line);
      exit(1);
    }
    if (sscanf(str_2, "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  if (value_2 == 0) {
    fprintf(stderr, "Divide by zero (line %d)\n", this->line);
    exit(1);
  }
  
  quotient = value_1 / value_2;
  
  char quotient_str[MAX_TOKEN + 1];
  
  sprintf(quotient_str, "%ld", quotient);
  
  value = setenv(this->var, quotient_str, 1);

  return pc + 1;
}

/** Make a command that implements the divide command.
    @param var, the variable to store the result in.
    @param val_1, the first value
    @param val_2, the second value
    @return a new Command that implements divide.
 */
static Command *makeDiv(char const *var, char const *val_1, char const *val_2)
{
  // Allocate space for the DivCommand object
  DivCommand *this = (DivCommand *) malloc(sizeof(DivCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeDiv;
  this->line = getLineNumber();
  this->destroy = destroyDiv;

  // Make a copy of the arguments.
  this->var = copyString(var);
  this->val_1 = copyString(val_1);
  this->val_2 = copyString(val_2);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Mult Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of variable the sum will be stored in */
  char *var;
  
  /** Value of 1st arg */
  char *val_1;
  
  /** Value of 2nd arg */
  char *val_2;
  
} MultCommand;

/**
  This function will destroy the MultCommand Struct.
  @param MultCommand cmd
*/
static void destroyMult(Command *cmd)
{
  MultCommand *this = (MultCommand *)cmd;
  free(this->var);
  free(this->val_1);
  free(this->val_2);
  free(this);
}

// Execute function for the multiply command
static int executeMult( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  MultCommand *this = (MultCommand *)cmd;

  int value = 0;
  long value_1 = 0;
  long value_2 = 0;
  
  long product = 0;
  
  if (this->val_1[0] == '"') {
    if (sscanf((this->val_1 + 1), "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_1 = getenv(this->val_1);
    if (str_1 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_1, this->line);
      exit(1);
    }
    if (sscanf(str_1, "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  if (this->val_2[0] == '"') {
    if (sscanf((this->val_2 + 1), "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_2 = getenv(this->val_2);
    if (str_2 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_2, this->line);
      exit(1);
    }
    if (sscanf(str_2, "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  product = value_1 * value_2;
  
  char prod_str[MAX_TOKEN + 1];
  
  sprintf(prod_str, "%ld", product);
  
  value = setenv(this->var, prod_str, 1);

  return pc + 1;
}

/** Make a command that implements the multiply command.
    @param var, the variable to store the result in.
    @param val_1, the first value
    @param val_2, the second value
    @return a new Command that implements multiply.
 */
static Command *makeMult(char const *var, char const *val_1, char const *val_2)
{
  // Allocate space for the MultCommand object
  MultCommand *this = (MultCommand *) malloc(sizeof(MultCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeMult;
  this->line = getLineNumber();
  this->destroy = destroyMult;

  // Make a copy of the arguments.
  this->var = copyString(var);
  this->val_1 = copyString(val_1);
  this->val_2 = copyString(val_2);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Sub Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of variable the sum will be stored in */
  char *var;
  
  /** Value of 1st arg */
  char *val_1;
  
  /** Value of 2nd arg */
  char *val_2;
  
} SubCommand;

/**
  This function will destroy the SubCommand Struct.
  @param SubCommand cmd
*/
static void destroySub(Command *cmd)
{
  SubCommand *this = (SubCommand *)cmd;
  free(this->var);
  free(this->val_1);
  free(this->val_2);
  free(this);
}

// Execute function for the subtract command
static int executeSub( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  SubCommand *this = (SubCommand *)cmd;

  int value = 0;
  long value_1 = 0;
  long value_2 = 0;
  
  long difference = 0;
  
  if (this->val_1[0] == '"') {
    if (sscanf((this->val_1 + 1), "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_1 = getenv(this->val_1);
    if (str_1 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_1, this->line);
      exit(1);
    }
    if (sscanf(str_1, "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  if (this->val_2[0] == '"') {
    if (sscanf((this->val_2 + 1), "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_2 = getenv(this->val_2);
    if (str_2 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_2, this->line);
      exit(1);
    }
    if (sscanf(str_2, "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  difference = value_1 - value_2;
  
  char diff_str[MAX_TOKEN + 1];
  
  sprintf(diff_str, "%ld", difference);
  
  value = setenv(this->var, diff_str, 1);

  return pc + 1;
}

/** Make a command that implements the subtract command.
    @param var, the variable to store the result in.
    @param val_1, the first value
    @param val_2, the second value
    @return a new Command that implements subtract.
 */
static Command *makeSub(char const *var, char const *val_1, char const *val_2)
{
  // Allocate space for the SubCommand object
  SubCommand *this = (SubCommand *) malloc(sizeof(SubCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeSub;
  this->line = getLineNumber();
  this->destroy = destroySub;

  // Make a copy of the argument.
  this->var = copyString(var);
  this->val_1 = copyString(val_1);
  this->val_2 = copyString(val_2);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Add Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of variable the sum will be stored in */
  char *var;
  
  /** Value of 1st arg */
  char *val_1;
  
  /** Value of 2nd arg */
  char *val_2;
  
} AddCommand;

/**
  This function will destroy the AddCommand Struct.
  @param AddCommand cmd
*/
static void destroyAdd(Command *cmd)
{
  AddCommand *this = (AddCommand *)cmd;
  free(this->var);
  free(this->val_1);
  free(this->val_2);
  free(this);
}

// Execute function for the add command
static int executeAdd( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  AddCommand *this = (AddCommand *)cmd;

  int value = 0;
  long value_1 = 0;
  long value_2 = 0;
  
  long sum = 0;
  
  if (this->val_1[0] == '"') {
    if (sscanf((this->val_1 + 1), "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_1 = getenv(this->val_1);
    if (str_1 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_1, this->line);
      exit(1);
    }
    if (sscanf(str_1, "%ld", &value_1) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  if (this->val_2[0] == '"') {
    if (sscanf((this->val_2 + 1), "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  } else {
    char *str_2 = getenv(this->val_2);
    if (str_2 == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->val_2, this->line);
      exit(1);
    }
    if (sscanf(str_2, "%ld", &value_2) != 1) {
      fprintf(stderr, "Invalid number (line %d)\n", this->line);
      exit(1);
    }
  }
  
  sum = value_1 + value_2;
  
  char sum_str[MAX_TOKEN + 1];
  
  sprintf(sum_str, "%ld", sum);
  
  value = setenv(this->var, sum_str, 1);

  return pc + 1;
}

/** Make a command that implements the add command.
    @param var, the variable to store the result in.
    @param val_1, the first value
    @param val_2, the second value
    @return a new Command that implements add.
 */
static Command *makeAdd(char const *var, char const *val_1, char const *val_2)
{
  // Allocate space for the AddCommand object
  AddCommand *this = (AddCommand *) malloc(sizeof(AddCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeAdd;
  this->line = getLineNumber();
  this->destroy = destroyAdd;

  // Make a copy of the arguments.
  this->var = copyString(var);
  this->val_1 = copyString(val_1);
  this->val_2 = copyString(val_2);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
//Set Command

typedef struct {
  //documented in the superclass.
  int (*execute)(Command *cmd, LabelMap *labelMap, int pc);
  
  void (*destroy)(Command *cmd);
  
  int line;
  
  /** Name of variable to be set */
  char *arg;
  
  /** Value of variable */
  char *val;
} SetCommand;

/**
  This function will destroy the SetCommand Struct.
  @param SetCommand cmd
*/
static void destroySet(Command *cmd)
{
  SetCommand *this = (SetCommand *)cmd;
  free(this->arg);
  free(this->val);
  free(this);
}

// Execute function for the set command
static int executeSet( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  SetCommand *this = (SetCommand *)cmd;

  int value = 0;
  
  if (this->val[0] == '"') {
    value = setenv(this->arg, this->val + 1, 1);
  } else {
    value = setenv(this->arg, this->val, 1);
  }

  return pc + 1;
}

/** Make a command that sets the given argument to an environment variable.
    @param arg The argument to set, the name of an environment variable.
    @return a new Command that implements set.
 */
static Command *makeSet(char const *arg, char const *val)
{
  // Allocate space for the SetCommand object
  SetCommand *this = (SetCommand *) malloc(sizeof(SetCommand));

  // Remember pointers to our overridable methods and line number.
  this->execute = executeSet;
  this->line = getLineNumber();
  this->destroy = destroySet;

  // Make a copy of the arguments.
  this->arg = copyString(arg);
  this->val = copyString(val);

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////
// Print Command

// Representation for a print command, derived from Command.
typedef struct {
  // Documented in the superclass.
  int (*execute)( Command *cmd, LabelMap *labelMap, int pc );

  void (*destroy)(Command *cmd);

  int line;

  /** Argument we're supposed to print. */
  char *arg;
} PrintCommand;

/**
  This function will destroy the PrintCommand Struct.
  @param PrintCommand cmd
*/
static void destroyPrint(Command *cmd) {
  PrintCommand *this = (PrintCommand *)cmd;
  free(this->arg);
  free(this);
}

// execute function for the print command
static int executePrint( Command *cmd, LabelMap *labelMap, int pc )
{
  // Cast the this pointer to the struct type it really points to.
  PrintCommand *this = (PrintCommand *)cmd;

  // Right now, we're assuming the argument is a literal value, but
  // you'll need to add support for variables.
  
  if (this->arg[0] == '"') {
    printf( "%s", this->arg + 1 );
  } else {
    //Get the value of the variable to print
    char *str = getenv(this->arg);
    
    if (str == NULL) {
      fprintf(stderr, "Undefined variable: %s (line %d)\n", this->arg, this->line);
      exit(1);
    }
    
    //But let's check to make sure that the variable isn't linked to another var
    char *var = getenv(str);
    if (var != NULL) {
      char *str2 = copyString(var);
      while (var != NULL) {
        var = getenv(var);
        if (var != NULL) {
          free(str2);
          char *str2 = copyString(var);
        }
      }
      printf("%s", str2);
      free(str2);
    } else {
      if (str == NULL) {
        fprintf(stderr, "Undefined variable: %s (line %d)\n", this->arg, this->line);
        exit(1);
      }
      printf("%s", str);
    }
  }
  
  return pc + 1;
}

/** Make a command that prints the given argument to the terminal.
    @param arg The argument to print, either a string literal or the
    name of an environment variable.
    @return a new Command that implements print.
 */
static Command *makePrint( char const *arg )
{
  // Allocate space for the PrintCommand object
  PrintCommand *this = (PrintCommand *) malloc( sizeof( PrintCommand ) );

  // Remember pointers to our overridable methods and line number.
  this->execute = executePrint;
  this->line = getLineNumber();
  this->destroy = destroyPrint;

  // Make a copy of the argument.
  this->arg = copyString( arg );

  // Return the result, as an instance of the Command interface.
  return (Command *) this;
}

////////////////////////////////////////////////////////////////////////////////

/**
  This function will parse commands from input.
  @param cmdName the name of the command
  @param fp the FILE we are reading
  @return the Command to execute
*/
Command *parseCommand( char *cmdName, FILE *fp )
{
  // Read the first token.
  char tok1[MAX_TOKEN + 1];
  
  //Read the second token.
  char tok2[MAX_TOKEN + 1];
  
  //Read the third token.
  char tok3[MAX_TOKEN + 1];

  // Figure out what kind of command it is.
  if (strcmp(cmdName, "print") == 0) {
    // Parse the one argument to print.
    expectToken( tok1, fp );
    requireToken( ";", fp );
    return makePrint( tok1 );
  } else if (strcmp(cmdName, "set") == 0) {
    //Parse two arguments to set.
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    requireToken(";", fp);
    return makeSet(tok1, tok2);
  } else if (strcmp(cmdName, "add") == 0) {
    //Parse three arguments to be used in add.
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    expectToken(tok3, fp);
    requireToken(";", fp);
    return makeAdd(tok1, tok2, tok3);
  } else if (strcmp(cmdName, "sub") == 0) {
    //Parse three arguments to be used in subtract.
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    expectToken(tok3, fp);
    requireToken(";", fp);
    return makeSub(tok1, tok2, tok3);
  } else if (strcmp(cmdName, "mult") == 0) {
    //Parse three arguments to be used in multiply.
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    expectToken(tok3, fp);
    requireToken(";", fp);
    return makeMult(tok1, tok2, tok3);
  } else if (strcmp(cmdName, "div") == 0) {
    //Parse three arguments to be used in divide.
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    expectToken(tok3, fp);
    requireToken(";", fp);
    return makeDiv(tok1, tok2, tok3);
  } else if (strcmp(cmdName, "mod") == 0) {
    //Parse three arguments to be used in modular.
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    expectToken(tok3, fp);
    requireToken(";", fp);
    return makeMod(tok1, tok2, tok3);
  } else if (strcmp(cmdName, "eq") == 0) {
    //Parse three arguments to be used in equals.
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    expectToken(tok3, fp);
    requireToken(";", fp);
    return makeEq(tok1, tok2, tok3);
  } else if (strcmp(cmdName, "less") == 0) {
    //Parse three arguments to be used in less than.
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    expectToken(tok3, fp);
    requireToken(";", fp);
    return makeLess(tok1, tok2, tok3);
  } else if (strcmp(cmdName, "goto") == 0) {
    expectToken(tok1, fp);
    requireToken(";", fp);
    return makeGoTo(tok1);
  } else if (strcmp(cmdName, "if") == 0) {
    expectToken(tok1, fp);
    expectToken(tok2, fp);
    requireToken(";", fp);
    return makeIf(tok1, tok2);
  } else {
    syntaxError();
  }

  // Never reached.
  return NULL;
}
