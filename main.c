#include "definitions.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int power(int x, int n) {
    int result = 1;
    for (int i = 0; i < n; i++)
        result *= x;
    return result;
}


int *generate_variable_values(int x, int s, pointer_list *allocated) {
    int *values = my_malloc(s * sizeof(int), allocated);
    if (values == NULL)
        return NULL;
    for (int i = s - 1; i > -1; i--) {
        values[i] = x % 2;
        x /= 2;
    }
    return values;
}


int generate_expressions_and_variables(FILE *fp, string_list *final_expressions, string *variables, char *operators, pointer_list *allocated) {
    int open_parentheses = 0;
    string_list *expression_parts = get_string_list(allocated);
    if (expression_parts == NULL)
        return 1;
    string *first = get_string(allocated);
    if (first == NULL)
        return 1;
    int err = append_string(expression_parts, first, allocated);
    if (err)
        return 1;
    err = prepend_string(final_expressions, first, allocated);
    if (err)
        return 1;
    char c;
    char last = '\0';
    int i = 1;
    for (int s = fscanf(fp, "%c", &c); s == 1; s = fscanf(fp, "%c", &c)) {
        if (c == ' ' || c == '\n' || c == '\t')
            continue;
        string_node *first_node = expression_parts -> head;
        if (c == ')') {
            open_parentheses--;
            if (last == '&' || last == '|' || last == '!') {
                printf("%d. helyen szintaktikai hiba %c után.\nKilépés\n", i, last);
                return 1;
            }
            pull_string_from_front(expression_parts, allocated);
        }
        else if (c == '(') {
            open_parentheses++;
            err = append_character(first_node -> data, '#', allocated);
            if (err)
                return 1;
            string *new = get_string(allocated);
            if (new == NULL)
                return 1;
            err = prepend_string(expression_parts, new, allocated);
            if (err)
                return 1;
            err = prepend_string(final_expressions, new, allocated);
            if (err)
                return 1;
        }
        else {
            if ((last == '(' || last == '\0') && (c == '&' || c == '|')) {
                printf("%d. helyen szintaktikai hiba %c előtt.\nKilépés.\n", i, c);
                return 1;
            }
            else if ((last == '&' || last == '|' || last == '!') && (c == '&' || c == '|')) {
                printf("%d. helyen szintaktikai hiba: %c\nKilépés.\n", i, c);
                return 1;
            }
            else if ((c == '&' || c == '|' || c == '!') && (last == '&' || last == '|')) {
                printf("%d. helyen szintaktikai hiba: %c.\nKilépés.\n", i, c);
            }
            else if ((last != '&' && last != '|' && last != '!' && last != '(' && last != ')' && last != '\0') && (c != '&' && c != '|' && c && '!' && c != '(' && c != ')')) {
                printf("%d. helyen szintaktikai hiba: %c\nKilépés.\n", i, c);
                return 1;
            }
            err = append_character(first_node -> data, c, allocated);
            if (err)
                return 1;
        }
        if (!strf(operators, c)) {
            err = insert_character_sorted(variables, c, 0, allocated);
            if (err)
                return 1;
        }
        if (open_parentheses < 0) {
            printf("%d. helyen szintaktikai hiba: %c\nKilépés.\n", i, c);
           return 1;
        }
        last = c;
        i++;
    }
    if (last == '&' || last == '|' || last == '|') {
        printf("%d. helyen szintaktikai hiba %c után.\nkilépés.\n", i, c);
        return 1;
    }
    if ( open_parentheses != 0) {
        printf("szintaktikai hiba a zárójelezésben. kilépés.\n");
        return 1;
    }
    pull_string_from_front(expression_parts, allocated);
    my_free(expression_parts, allocated);
    return 0;
}


int get_value(char variable, char *variables, int *values) {
    for (int i = 0; 1; i++)
        if (variables[i] == variable)
            return values[i];
}


int eval(int_list *values, operation op) {
    switch (op) {
        case AND: {
                      for (int_node *ptr = values -> head; ptr != NULL; ptr = ptr -> next)
                          if (ptr -> data == 0)
                              return 0;
                      return 1;
                  }
        case OR: {
                     for (int_node *ptr = values -> head; ptr != NULL; ptr = ptr -> next)
                         if (ptr -> data == 1)
                             return 1;
                     return 0;
                 }
        case NOT: {
                      return !values -> head -> data;
                  }
    }
    return -1;
}


int eval_var(character_node *var, char *vars, int *vals, int_list *memory, pointer_list *allocated) {
    if (var -> data == '#')
        return pull_int_from_front(memory, allocated);
    return get_value(var -> data, vars, vals);
}


int eval_not(character_node *exp, char *vars, int *vals, int_list *memory, character_node *end, pointer_list *allocated) {
    int count = 0;
    character_node *ptr;
    for (ptr = exp; ptr -> data == '!'; ptr = ptr -> next)
        count++;
    end = ptr;
    if (count % 2 == 1)
        return !eval_var(end, vars, vals, memory, allocated);
    return eval_var(end, vars, vals, memory, allocated);
}


int eval_and(string *exp, char *vars, int *vals, int_list *memory, pointer_list *allocated) {
    remove_character(exp, '&', allocated);
    int_list *values = get_int_list(allocated);
    if (values == NULL)
        return -1;
    for (character_node *var = exp -> head; var != NULL; var = var -> next)
        if (var -> data == '!') {
            character_node *end = NULL;
            int err = append_int(values, eval_not(var, vars, vals, memory, end, allocated), allocated);
            if (err)
                return -1;
            var = end;
        }
        else {
            int err = append_int(values, eval_var(var, vars, vals, memory, allocated), allocated);
            if (err)
                return -1;
        }
    int result = eval(values, AND);
    free_int_list(values, allocated);
    return result;
}


int eval_or(string_list *exps, char *vars, int *vals, int_list *memory, pointer_list *allocated) {
    int_list *values = get_int_list(allocated);
    if (values == NULL)
        return -1;
    for (string_node *exp = exps -> head; exp != NULL; exp = exp -> next) {
        int and = eval_and(exp -> data, vars, vals, memory, allocated);
        if (and == -1)
            return -1;
         int err = append_int(values, and, allocated);
         if (err)
             return -1;
    }
    int result = eval(values, OR);
    free_int_list(values, allocated);
    return result;
}


int eval_expression(string_list **splitted_parts, int s, char *vars, int *vals, pointer_list *allocated) {
    int_list *memory = get_int_list(allocated);
    for (int i = 0; i < s; i++) {
        int or = eval_or(splitted_parts[i], vars, vals, memory, allocated);
        if (or == -1)
            return -1;
        int err = prepend_int(memory, or, allocated);
        if (err)
            return -1;
    }
    int result = pull_int_from_front(memory, allocated);
    free_int_list(memory, allocated);
    return result;
}


int evaluate(FILE *fp, string *variables, string_list *expressions, pointer_list *allocated) {
    string_list **splitted_parts = (string_list **) my_malloc(expressions -> length * sizeof(string_list), allocated);
    if (splitted_parts == NULL)
        return 1;
    string_node *exp = expressions -> head;
    for (int i = 0; i < expressions -> length; i++) {
        splitted_parts[i] = split(exp -> data, '|', allocated);
        if (splitted_parts[i] == NULL)
            return 1;
        exp = exp -> next;
    }

    int variable_count = variables -> length;
    char *vars = generate_character_array(variables, allocated);
    if (vars == NULL)
        return 1;
    int max_index = power(2, variable_count);
    table truth_table = get_table(max_index + 1, variable_count + 1, 1, 1, fp);
    for (int i = 0; i < variable_count; i++)
        print_table(&truth_table, vars[i]);
    print_table(&truth_table, 'f');
    for (int index = 0; index < max_index; index++) {
        int *values = generate_variable_values(index, variable_count, allocated);
        int result = eval_expression(splitted_parts, expressions -> length, vars, values, allocated);
        if (result == -1)
            return 1;
        if (1) {
            for (int j = 0; j < variable_count; j++)
                print_table(&truth_table, values[j] + '0');
            print_table(&truth_table, result + '0');
        }
        else
            truth_table.table_rows--;
        my_free(values, allocated);
    }
    my_free(vars, allocated);
    print_table(&truth_table, 0);
    for (int i = 0; i < expressions -> length; i++)
        free_string_list(splitted_parts[i], allocated);
    my_free(splitted_parts, allocated);
    return 0;
}


char *generate_output_filename(char *filename, pointer_list *allocated) {
    char *addition = "_out.txt";
    int length = strlen(filename);
    char *result = (char *) my_malloc(sizeof(char) * (length + strlen(addition) - 3), allocated);
    if (result == NULL)
        return NULL;
    int i;
    for (i = 0; i < length - 4; i++)
        result[i] = filename[i];
    for (int j = 0; j < (int) strlen(addition) + 1; j++)
        result[i++] = addition[j];
    my_free(filename, allocated);
    return result;
}


int validate_filename(char *fn) {
    int length = strlen(fn);
    if (length < 4)
        return 0;
    char *extension = ".txt";
    int ext_length = strlen(extension);
    for (int i = length - ext_length; i < ext_length + 1; i++)
        if (fn[i] != extension[i - (length - ext_length)])
            return 0;
    return 1;
}


int program(pointer_list *allocated) {
    char operators[] = "&|!()";

    string_list *expressions = get_string_list(allocated);
    if (expressions == NULL)
        return 1;
    string *variables = get_string(allocated);
    if (variables == NULL)
        return 1;

    char *filename = input_string(allocated);
    if (filename == NULL)
        return 1;
    if (!validate_filename(filename)) {
        printf("\"%s\" nem \".txt\" kiterjesztésű.\nkilépés.\n", filename);
        return 1;
    }

    FILE *input = fopen(filename, "r");
    if (input == NULL) {
        printf("a fájl nem található: %s\n", filename);
        return 1;
    }
    printf("%s betöltése... \n", filename);
    int err = generate_expressions_and_variables(input, expressions, variables, operators, allocated);
    if (err)
        return 1;
    fclose(input);

    printf("Kész.\n");

    char *output = generate_output_filename(filename, allocated);
    if (output == NULL)
        return 1;
    FILE *fp = fopen(output, "w");
    if (fp == NULL) {
        printf("A fajlt nem lehet megnyitni: %s\n", output);
        return 1;
    }
    printf("Kiértékelés... \n");
    err = evaluate(fp, variables, expressions, allocated);
    if (err)
        return 1;
    printf("Kész.\n");
    fclose(fp);
    printf("A kimenet megtalálható: %s\n", output);
    return 0;
}


int main(void) {
    pointer_list *allocated = get_pointer_list();
    if (allocated == NULL) {
        printf("Elfogyott a memória. Kilépés.\n");
        return 1;
    }
    int exit_code = program(allocated);
    free_pointer_list(allocated);
    return  exit_code;
}
