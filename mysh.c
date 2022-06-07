#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "builtins.h"
#include "io_helpers.h"
#include "variables.h"




int main(int argc, char* argv[]) {
    char *prompt = "mysh$ "; // TODO Step 1, Uncomment this.

    char input_buf[MAX_STR_LEN + 1];
    input_buf[MAX_STR_LEN] = '\0';
    char *token_arr[MAX_STR_LEN] = {NULL};

    // pointer for comparing strings
    char *var;

    // variable Linked list pointer
    Variable *front = NULL;
    char *var_name;
    char *var_value;

   

    while (1) {
        // Prompt and input tokenization

        // TODO Step 2:

        display_message(prompt);
        // Display the prompt via the display_message function.


        int ret = get_input(input_buf);
        size_t token_count = tokenize_input(input_buf, token_arr);
        // Clean exitmak
        // Продлил до 60, может потом поменяю макс число.
        // if (ret != -1 && ((strncmp("exit", token_arr[0], 64) == 0))) {

        if (token_count == 0){
                continue;
            }
        if (ret != -1 && ((strncmp("exit", token_arr[0], 64) == 0))) {
            	
		while(front != NULL){
			Variable *tmp_2 = front;
			front = (Variable *)front->next;
			free(tmp_2);
		}
		break;
        }


        // Command execution
        if (token_count >= 1) {
            bn_ptr builtin_fn = check_builtin(token_arr[0]);

            for (size_t i = 0; i < token_count; i++){
                    char *token = token_arr[i];
                    if (token[0] == '$'){
                        if (token[1]){
                            char *tmp_tok = strtok(token, "$");

                            Variable *tmp = ret_variab(tmp_tok, front);
                            if (tmp){
                                token = tmp->value;
                                token_arr[i] = token;
                            }
                        }
                        
                    }
            }
            //if it is a builtin function;
            if (builtin_fn != NULL) {
                ssize_t err = builtin_fn(token_arr);            
                if (err == - 1) {
                    display_error("ERROR: Builtin failed: ", token_arr[0]);
                }
            } 

            else {
                    char *token = token_arr[0];

                    char token2[MAX_STR_LEN];
                    strcpy(token2, token);
                    //  = *token_arr[0];
                    // 
                    if (token[0] == '='){
                        display_error("ERROR: Unrecognized command: ", token_arr[0]);
                    }
                    else{
                        var = strstr(token_arr[0], "=");
                        if (var){
                            
                            var_name = strtok(token_arr[0],"=");
                            
                            // var_value = strtok(NULL, "=");
                            var_value = strchr(token2, '=');
                            // var_value = token[tmp_tmp];
                            // var_value = strstr()
                            // display_message(tmp_tmp);
                            if (var_value == NULL){
                                var_value = " ";
                            }
                            front = create_var(var_name, var_value+1, front);
                
                        }
                        else {
                            display_error("ERROR: Unrecognized command: ", token_arr[0]);
                        }

                    }
            
            }
        }

    }

    return 0;
}
