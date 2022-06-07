#define _GNU_SOURCE

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include <sys/types.h>
#include <dirent.h>
#include <errno.h>

#include "builtins.h"
#include "io_helpers.h"

#define _GNU_SOURCE




// ====== Command execution =====

/* Return: index of builtin or -1 if cmd doesn't match a builtin
 */
bn_ptr check_builtin(const char *cmd) {
    ssize_t cmd_num = 0;
    while (cmd_num < BUILTINS_COUNT &&
           strncmp(BUILTINS[cmd_num], cmd, MAX_STR_LEN) != 0) {
        cmd_num += 1;
    }
    return BUILTINS_FN[cmd_num];
}


// ===== Builtins =====

/* Prereq: tokens is a NULL terminated sequence of strings.
 * Return 0 on success and -1 on error ... but there are no errors on echo. 
 */
ssize_t bn_echo(char **tokens) {
    ssize_t index = 1;


    while (tokens[index] != NULL) {
        display_message(tokens[index]);
        display_message (" ");
        
        index += 1;
    }    
    display_message("\n");

    return 0;
}


void ls_helper(char * path_in, int depth){
    DIR* dir= opendir(path_in);
    if (dir == NULL){
        closedir(dir);
        return;
    }
    // long int val = depth;
    if (depth == 0){
        closedir(dir);
        return;
    }

    struct dirent* entity;
    entity = readdir(dir);
    while (entity != NULL){
        char print_dir[100] = {0};
        strcat(print_dir, entity->d_name);
        strcat(print_dir, "\n");
        display_message(print_dir);
        

        if(entity->d_type == DT_DIR && strcmp(entity->d_name, ".") != 0 && strcmp(entity->d_name, "..") != 0){
            char path[100] = {0};
            strcat(path, path_in);
            strcat(path, "/");
            strcat(path, entity->d_name);

            // long int* val2 = &val -1;
            display_message("\n");
            ls_helper(path, depth-1); 
            
        }
        entity = readdir(dir);
    }
    closedir(dir);

}

int check_dir(char * path){
    DIR* dir = opendir(path);
    if (dir) {
        closedir(dir);
        return 0;
    } else if (ENOENT == errno){
        return -1;
    }else{
        return 1;
    }
}

int check_file(char *fname){
    FILE *file;
    if ((file = fopen(fname, "r"))){
        fclose(file);
        return 0;
    }
    return 1;
}

ssize_t bn_ls(char **tokens){
    DIR *dir;
    struct dirent *sd;
    char* ret;

    ssize_t index = 1;
    int succ = 0;
    int printed =0;
    int valid_path = 0;

// To find the path and open the dir.
    while (tokens[index] != NULL){
        char * open = tokens[index];
        // if ((dir = opendir(open))){
        if (check_dir(open) == 0){
            dir = opendir(open);
            succ = 1;
            index += 1;
            break;
        }
        index += 1;
    }
    // if (index == 2){
    //     display_message("ERROR: Invalid path\n");
    //     succ = -1;
    // }
    if (index > 1){
        if (succ == 0){
            // dir = opendir(".");
            valid_path = 1;
            display_error("ERROR: Invalid path", "");
            display_error("ERROR: Builtin failed: ", tokens[0]);
        }
    }else{
        succ = 1;
        dir = opendir(".");
    }

// by this point the directory was already opened
    index = 1;
    if (succ != -1 && valid_path == 0){
        while (tokens[index] != NULL){
            char * open = tokens[index];
            char * open2 = tokens[index+1];
            char * open3 = tokens[index+2];
            char * open4 = tokens[index+3];

            if ((strncmp(open, "--f", 4)) == 0){
                while ((sd = readdir(dir))!= NULL){
                    if ((ret = strstr(sd->d_name, open2))){
                        char buff_tmp1[100] = {0};
                        strcat(buff_tmp1, sd->d_name);
                        strcat(buff_tmp1, "\n");
                        display_message(buff_tmp1);
                        printed = 1; 
                    }

                }
            } else if (strncmp(open, "--rec", 6) == 0){
                if (open2){
                    if (check_dir(open2) == 0){
                        if (open3){
                            if ((strncmp(open3, "--d", 4) == 0)){
                                // recursion happens
                                char *ptr;
                                // long int dep1 = strtol(open4, &ptr, 10);
                                // long int *dep = &dep1;

                                int dep1 = strtol(open4, &ptr, 10);
                                
                                // used to be dep
                                ls_helper(open2, dep1);
                                printed = 1;
                                break;
                            } else{
                                printed = -1;
                            }
                        }else{
                            printed = -1;
                            // display_message("ERROR: Invalid path\n");
                        }
                    } else{
                        printed = -1;
                        // display_message("ERROR: Invalid path\n");
                    }
                }
            } else if ((strncmp(open, "--d", 4) == 0)){
                if (open2){
                    if (strncmp(open3, "--rec", 6) == 0){
                        if (open4){
                            if (check_dir(open4) == 0){
                                //recursion happens
                                char *ptr;
                                int dep1 = strtol(open2, &ptr, 10);
                                // long int *dep = &dep1;

                                ls_helper(open4, dep1);
                                printed =1;
                                break;
                            }else{
                                printed = -1;
                                // display_message("ERROR: Invalid path\n");
                            }
                        }else{
                            printed = -1;
                            // display_message("ERROR: Invalid path\n");
                        }
                    }else{
                        printed = -1;
                    }
                } else{
                    printed = -1;
                }
            }
            index +=1;
            
        }

        if (printed ==0 && succ != 0){
            while ((sd = readdir(dir))!= NULL){
                    char buff_tmp2[100] = {0};
                    strcat(buff_tmp2, sd->d_name);
                    strcat(buff_tmp2, "\n");
                    display_message(buff_tmp2);
                }
        }else if (printed == -1){
            display_error("ERROR: Invalid path", "");
        }
    }

    if (succ != 0){
        closedir(dir);
    }
    return 0;
    
}



ssize_t bn_cd(char **tokens){
    char* ptr = tokens[1];
    if (ptr){
        if (strcmp(ptr, "...") == 0){
            chdir("../..");
        } else if (strcmp(ptr, "....")== 0){
            chdir("../../..");
        }
        else if (chdir(ptr) == 0){
            return 0;
        }
        else{
           display_error("ERROR: Invalid path", "");
        }
    }
    
    else {
        chdir(getenv("HOME"));
    } 
    return 0;

}

ssize_t bn_cat(char** tokens){
    FILE* file;
    char* ptr = tokens[1];
    file = fopen(ptr, "r");

    if(ptr == NULL){
        display_error("ERROR: No input source provided", "");
        display_error("ERROR: Builtin failed: ", tokens[0]);
        // printf("ERROR: Cannot open file");
    }
    if (!file){
        display_error("ERROR: Cannot open file", "");
    } 
    else{
        char *ch;
        while ((ch =fgets(ch, sizeof(file), file))){
            display_message(ch);
            // free(ch2)
        }
        fclose(file);
    }
    
    return 0;
}

ssize_t bn_wc(char **tokens){
    FILE *f1;
    char ch;
    char *ptr = tokens[1];
    if (ptr){
        if (check_file(ptr)==0){

            int lines = 0;
            int words = 0;
            int chars = 0;

            char tmp;
            int tmp2 =0;

            f1 = fopen(tokens[1], "r");
            while((ch=getc(f1))!=  EOF){
                if (ch=='\n'){
                    lines++;
                }
                if (tmp2 > 0 && tmp !='\n' && tmp != ' '){
                    if (ch=='\n' || ch ==' '){
                        words++;
                    }
                }
                
                if (ch !='\n' || ch !=' '){
                    chars++;
                }
                tmp = ch;
                tmp2 +=1;
            }

            char *buffer;
            asprintf(&buffer, "word count %d\ncharacter count %d\nnewline count %d\n", words, chars, lines);
            display_message(buffer);
            free(buffer);
            
        }
        else{
            display_error("ERROR: Cannot open file", "");
        }
    }

    else{
        display_error("ERROR: No input source provided", "");
        display_error("ERROR: Builtin failed: ", tokens[0]);
    }
    return 0;
}