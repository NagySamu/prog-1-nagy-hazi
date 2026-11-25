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


int *generate_variable_values(int x, int s) {
    int *values = malloc(s * sizeof(int));
    for (int i = s - 1; i > -1; i--) {
        values[i] = x % 2;
        x /= 2;
    }
    return values;
}


int generate_expressions_and_variables(FILE *fp, string_list *final_expressions, string *variables, char *operators) {
    string_list *expression_parts = get_string_list();
    string *first = get_string();
    append_string(expression_parts, first);
    prepend_string(final_expressions, first);
    char c;
    char last = '\0';
    int i = 1;
    for (int s = fscanf(fp, "%c", &c); s == 1; s = fscanf(fp, "%c", &c)) {
        if (c == ' ' || c == '\n' || c == '\t')
            continue;
        string_node *first_node = expression_parts -> head;
        if (c == ')') {
            if (last == '&' || last == '|' || last == '!') {
                printf("%d. helyen szintaktikai hiba %c után.\nKilépés\n", i, last);
                return 1;
            }
            pull_string_from_front(expression_parts);
        }
        else if (c == '(') {
            append_character(first_node -> data, '#');
            string *new = get_string();
            prepend_string(expression_parts, new);
            prepend_string(final_expressions, new);
        }
        else {
            if ((last == '(' || last == '\0') && (c == '&' || c == '|')) {
                printf("%d. helyen szintaktikai hiba %c előtt.\nKilépés.\n", i, c);
                return 1;
            }
            append_character(first_node -> data, c);
        }
        if (!strf(operators, c))
            insert_character_sorted(variables, c, 0);
        last = c;
        i++;
    }
    if (last == '&' || last == '|' || last == '|') {
        printf("%d. helyen szintaktikai hiba %c után.\nKilépés.\n", i, last);
        return 1;
    }
    pull_string_from_front(expression_parts);
    free(expression_parts);
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


int eval_var(character_node *var, char *vars, int *vals, int_list *memory) {
    if (var -> data == '#')
        return pull_int_from_front(memory);
    return get_value(var -> data, vars, vals);
}


int eval_not(character_node *exp, char *vars, int *vals, int_list *memory, character_node *end) {
    int count = 0;
    character_node *ptr;
    for (ptr = exp; ptr -> data == '!'; ptr = ptr -> next)
        count++;
    end = ptr;
    if (count % 2 == 1)
        return !eval_var(end, vars, vals, memory);
    return eval_var(end, vars, vals, memory);
}


int eval_and(string *exp, char *vars, int *vals, int_list *memory) {
    remove_character(exp, '&');
    int_list *values = get_int_list();
    for (character_node *var = exp -> head; var != NULL; var = var -> next)
        if (var -> data == '!') {
            character_node *end = NULL;
            append_int(values, eval_not(var, vars, vals, memory, end));
            var = end;
        }
        else
            append_int(values, eval_var(var, vars, vals, memory));
    int result = eval(values, AND);
    free_int_list(values);
    return result;
}


int eval_or(string_list *exps, char *vars, int *vals, int_list *memory) {
    int_list *values = get_int_list();
    for (string_node *exp = exps -> head; exp != NULL; exp = exp -> next)
        append_int(values, eval_and(exp -> data, vars, vals, memory));
    int result = eval(values, OR);
    free_int_list(values);
    return result;
}


int eval_expression(string_list **splitted_parts, int s, char *vars, int *vals) {
    int_list *memory = get_int_list();
    for (int i = 0; i < s; i++)
        prepend_int(memory, eval_or(splitted_parts[i], vars, vals, memory));
    int result = pull_int_from_front(memory);
    free_int_list(memory);
    return result;
}


void evaluate(FILE *fp, string *variables, string_list *expressions) {
    string_list **splitted_parts = (string_list **) malloc(expressions -> length * sizeof(string_list));
    string_node *exp = expressions -> head;
    for (int i = 0; i < expressions -> length; i++) {
        splitted_parts[i] = split(exp -> data, '|');
        exp = exp -> next;
    }

    int variable_count = variables -> length;
    char *vars = generate_character_array(variables);
    int max_index = power(2, variable_count);
    table truth_table = get_table(max_index + 1, variable_count + 1, 1, 1, fp);
    for (int i = 0; i < variable_count; i++)
        print_table(&truth_table, vars[i]);
    print_table(&truth_table, 'F');
    for (int index = 0; index < max_index; index++) {
        int *values = generate_variable_values(index, variable_count);
        int result = eval_expression(splitted_parts, expressions -> length, vars, values);
        if (1) {
            for (int j = 0; j < variable_count; j++)
                print_table(&truth_table, values[j] + '0');
            print_table(&truth_table, result + '0');
        }
        else
            truth_table.table_rows--;
        free(values);
    }
    free(vars);
    print_table(&truth_table, 0);
    for (int i = 0; i < expressions -> length; i++)
        free_string_list(splitted_parts[i]);
    free(splitted_parts);
}


char *generate_output_filename(char *filename) {
    char *addition = "_out.txt";
    int length = strlen(filename);
    char *result = (char *) malloc(sizeof(char) * (length + strlen(addition) - 3));
    int i;
    for (i = 0; i < length - 4; i++)
        result[i] = filename[i];
    for (int j = 0; j < (int) strlen(addition) + 1; j++)
        result[i++] = addition[j];
    free(filename);
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


int main(void) {
    char operators[] = "&|!()";

    string_list *expressions = get_string_list();
    string *variables = get_string();

    char *filename = input_string();
    if (!validate_filename(filename)) {
        printf("\"%s\" nem \".txt\" kiterjesztésű.\nKilépés.\n", filename);
        return 1;
    }

    FILE *input = fopen(filename, "r");
    if (input == NULL) {
        printf("A fájl nem található: %s\n", filename);
        return 1;
    }
    printf("%s betöltése... \n", filename);
    int err = generate_expressions_and_variables(input, expressions, variables, operators);
    if (err)
        return 1;
    fclose(input);

    printf("Kész.\n");

    char *output = generate_output_filename(filename);
    FILE *fp = fopen(output, "w");
    if (fp == NULL) {
        printf("A fajlt nem lehet megnyitni: %s\n", output);
        return 1;
    }
    printf("Kiértékelés... \n");
    evaluate(fp, variables, expressions);
    printf("Kész.\n");
    fclose(fp);
    printf("A kimenet megtalálható: %s\n", output);

    free_string_list(expressions);
    free(output);
    return 0;
}
