#ifndef __VARIABLES_H__
#define __VARIABLES_H__

typedef struct {
    char name[64];
    char value[64];
    struct variable *next;
    
}Variable;

//creates a variable
Variable *create_var(char *name, char *val, Variable *next);


//returns a variable
Variable* ret_variab(char *name, Variable* var);



#endif