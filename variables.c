// #include <variables.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>




typedef struct variable{
    char name[64];
    char value[64];
    struct variable *next;
    
}Variable;


Variable *create_var(char *name, char *val, Variable *next){
    Variable *new_var = malloc(sizeof(Variable));
    strncpy(new_var->name, name, 64);
    strncpy(new_var->value, val, 64);
    new_var->next = next;

    
    return new_var;

}

Variable* ret_variab(char *name, Variable* var){
    Variable*curr = var;

    while (curr != NULL){
        if (strncmp(curr->name, name, 64)==0){
            return curr;
        }
        curr = curr->next;
    }

    return 0;
}