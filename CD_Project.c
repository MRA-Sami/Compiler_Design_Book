#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#define MAX_VARS 100
#define MAX_VAR_NAME 20
typedef struct {
    char name[MAX_VAR_NAME];
    int value;
} Variable;
Variable vars[MAX_VARS];
int var_count = 0;
int get_var_index(char *name) {
    for (int i = 0; i < var_count; i++) {
        if (strcmp(vars[i].name, name) == 0)
            return i;
    }
    return -1;
}
void set_var(char *name, int value) {
    int idx = get_var_index(name);
    if (idx == -1) {
        strcpy(vars[var_count].name, name);
        vars[var_count].value = value;
        var_count++;
    } else {
        vars[idx].value = value;
    }
}

int get_var_value(char *name) {
    int idx = get_var_index(name);
    if (idx == -1) {
        printf("Error: Variable '%s' not defined.\n", name);
        exit(1);
    }
    return vars[idx].value;
}

void trim(char *str) {
    int len = strlen(str);
    while(len > 0 && isspace(str[len-1])) str[--len] = 0;
    int start = 0;
    while(str[start] && isspace(str[start])) start++;
    if (start > 0) memmove(str, str+start, len-start+1);
}

int eval_expr(char *expr) {
    trim(expr);
    if (isalpha(expr[0])) {
        return get_var_value(expr);
    } else {
        return atoi(expr);
    }
}

void execute_line(char *line) {
    trim(line);
    if (strncmp(line, "let ", 4) == 0) {
        char var_name[MAX_VAR_NAME], expr[100];
        if (sscanf(line + 4, "%s = %[^\n]", var_name, expr) == 2) {
            int val = eval_expr(expr);
            set_var(var_name, val);
        } else {
            printf("Syntax error in let statement.\n");
        }
    } else if (strncmp(line, "print ", 6) == 0) {
        char rest[100];
        strcpy(rest, line + 6);
        trim(rest);
        if (rest[0] == '"') {
            int len = strlen(rest);
            if (rest[len-1] == '"') {
                rest[len-1] = 0;
                printf("%s\n", rest+1);
            } else {
                printf("Syntax error in print statement.\n");
            }
        } else {
            int val = eval_expr(rest);
            printf("%d\n", val);
        }
    } else if (strncmp(line, "if ", 3) == 0) {
        char cond_var[MAX_VAR_NAME], op[3], cond_val_str[50], then_part[100];
        if (sscanf(line + 3, "%s %2s %s %[^\n]", cond_var, op, cond_val_str, then_part) >= 3) {
            int left = eval_expr(cond_var);
            int right = atoi(cond_val_str);
            int cond = 0;
            if (strcmp(op, ">") == 0) cond = (left > right);
            else if (strcmp(op, "<") == 0) cond = (left < right);
            else if (strcmp(op, "==") == 0) cond = (left == right);
            else {
                printf("Unsupported operator in if statement.\n");
                return;
            }
            if (cond) {
                trim(then_part);
                if (strncmp(then_part, "print ", 6) == 0) {
                    execute_line(then_part);
                } else {
                    printf("Only print allowed after if condition in this simple interpreter.\n");
                }
            }
        } else {
            printf("Syntax error in if statement.\n");
        }
    } else if (strlen(line) == 0) {
    } else {
        printf("Unknown statement: %s\n", line);
    }
}

int main() {
    printf("Mini Interpreter for Simple Language\n");
    printf("Supported commands:\n");
    printf("- let var = value\n");
    printf("- print var or print \"string\"\n");
    printf("- if var >|<|== value print ...\n");
    printf("Type 'exit' to quit\n\n");

    char line[200];
    while (1) {
        printf(">>> ");
        if (!fgets(line, sizeof(line), stdin)) break;
        trim(line);
        if (strcmp(line, "exit") == 0) break;
        execute_line(line);
    }
    return 0;
}
