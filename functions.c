#include "definitions.h"
#include <stdlib.h>
#include <stdio.h>

character_node *get_character_node(char c) {
    character_node *n = (character_node *)malloc(sizeof(character_node));
    n -> data = c;
    n -> next = NULL;
    return n;
}

string *get_string() {
    string *s = (string *) malloc(sizeof(string));
    s -> length = 0;
    s -> head = NULL;
    s -> tail = NULL;
    return s;
}

string *create_string(char *s) {
    string *result = get_string();
    for (char *c = s; *c != '\0'; c++) {
        append_character(result, *c);
    }
    return result;
}

void free_string(string *s) {
    character_node *next = NULL;
    for (character_node *ptr = s -> head; ptr != NULL; ptr = next) {
        next = ptr -> next;
        free(ptr);
    }
    free(s);
}

string_node *get_string_node(string *s) {
    string_node *n = (string_node *)malloc(sizeof(string_node));
    n -> data = s;
    n -> next = NULL;
    return n;
}

void free_string_node(string_node *n) {
    free_string(n -> data);
    free(n);
}

string_list *get_string_list() {
    string_list *list = (string_list *)malloc(sizeof(string_list));
    list -> length = 0;
    list -> head = NULL;
    list -> tail = NULL;
    return list;
}

void free_string_list(string_list *list) {
    string_node *next = NULL;
    for (string_node *ptr = list -> head; ptr != NULL; ptr = next) {
        next = ptr -> next;
        free_string_node(ptr);
    }
    free(list);
}

void print_string(string s) {
    for (character_node *c = s.head; c != NULL; c = c -> next)
        printf("%c", c -> data);
}

void print_string_list(string_list list) {
    printf("[");
    for (string_node *ptr = list.head; ptr != NULL; ptr = ptr -> next) {
        printf("\"");
        print_string(*ptr -> data);
        printf("\"");
        if (ptr -> next != NULL)
            printf(", ");
    }
    printf("]\n");
}

int strf(char s[], char c) {
    for (int i = 0; s[i] != '\0'; i++)
        if (s[i] == c)
            return 1;
    return 0;
}

void append_character(string *s, char c) {
    character_node *n = get_character_node(c);
    if (s -> tail == NULL) {
        s -> tail = n;
        s -> head = n;
    }
    else {
        s -> tail -> next = n;
        s -> tail = n;
    }
    s -> length++;
}

void prepend_character(string *s, char c) {
    character_node *n = get_character_node(c);
    if (s -> head == NULL) {
        s -> head = n;
        s -> tail = n;
    }
    else {
        n -> next = s -> head;
        s -> head = n;
    }
    s -> length++;
}

void insert_character_sorted(string *s, char c, int allow_duplicates) {
    if (s -> head == NULL) {
        character_node *n = get_character_node(c);
        s -> head = n;
        s -> tail = n;
        s -> length++;
    }
    else if (c < s -> head -> data)
        prepend_character(s, c);
    else if (s -> tail -> data < c)
        append_character(s, c);
    else
        for (character_node *ptr = s -> head; ptr != NULL; ptr = ptr -> next) {
            if (ptr -> data == c && !allow_duplicates)
                break;
            if (ptr -> data <= c && c < ptr -> next -> data) {
                character_node *n = get_character_node(c);
                n -> next = ptr -> next;
                ptr -> next = n;
                s -> length++;
                break;
            }
        }
}

void remove_character(string *s, char c) {
    character_node *prev = NULL;
    for (character_node *ptr = s -> head; ptr != NULL; ) {
        character_node *next = ptr -> next;
        if (ptr -> data == c) {
            if (prev == NULL)
                s -> head = next;
            else
                prev -> next = next;
            ptr = next;
            s -> length--;
        }
        else {
            prev = ptr;
            ptr = next;
        }
    }
}

char *generate_character_array(string *s) {
    char *array = (char *)malloc((s -> length + 1) * sizeof(char));
    int i;
    for (i = 0; i < s -> length; i++) {
        character_node *first = s -> head;
        array[i] = first -> data;
        s -> head = first -> next;
        free(first);
    }
    free(s);
    array[i] = '\0';
    return array;
}

char *input_string() {
    string *s = get_string();
    int ch;
    while ((ch = getchar()) != EOF && ch != '\n')
        append_character(s, ch);
    char *result = generate_character_array(s);
    return result;
}

void append_string(string_list *list, string *s) {
    string_node *n = get_string_node(s);
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
}

void append_string_node(string_list *list, string_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
}

void prepend_string(string_list *list, string *s) {
    string_node *n = get_string_node(s);
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head = n;
    }
    list -> length++;
}

void prepend_string_node(string_list *list, string_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head = n;
    }
    list -> length++;
}

string_node *pull_node_from_front(string_list *list) {
    string_node *first_node = list -> head;
    list -> head = first_node -> next;
    list -> length--;
    return first_node;

}

string *pull_string_from_front(string_list *list) {
    string_node *first_node = pull_node_from_front(list);
    string *s = first_node -> data;
    free(first_node);
    return s;

}

void pop_front(string_list *list) {
    string_node *first_node = pull_node_from_front(list);
    free_string_node(first_node);
}

string_list *split(string *s, char c) {
    string_list *result = get_string_list();
    append_string(result, get_string());
    for (character_node *chr = s -> head; chr != NULL; chr = chr -> next) {
        if (chr -> data == c)
            append_string(result, get_string());
        else
            append_character(result -> tail -> data, chr -> data);
    }
    return result;
}

int_node *get_int_node(int a) {
    int_node *n = (int_node *) malloc(sizeof(int_node));
    n -> data = a;
    n -> next = NULL;
    return n;
}

int_list *get_int_list() {
    int_list *list = (int_list *) malloc(sizeof(int_list));
    list -> length = 0;
    list -> head = NULL;
    list -> tail = NULL;
    return list;
}

void free_int_list(int_list *list) {
    if (list -> length == 0) {
        free(list);
        return;
    }
    int_node *next = list -> head -> next;
    for (int_node *ptr = list -> head; ptr != NULL; ptr = next) {
        next = ptr -> next;
        free(ptr);
    }
    free(list);
}

void append_int(int_list *list, int a) {
    int_node *n = get_int_node(a);
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
}

void append_int_node(int_list *list, int_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
}

void prepend_int(int_list *list, int a) {
    int_node *n = get_int_node(a);
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head = n;
    }
    list -> length++;
}

void prepend_int_node(int_list *list, int_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head = n;
    }
    list -> length++;
}

int_node *pull_int_node_from_front(int_list *list) {
    int_node *first = list -> head;
    list -> head = first -> next;
    list -> length--;
    return first;
}

int pull_int_from_front(int_list *list) {
    int_node *n = pull_int_node_from_front(list);
    int data = n -> data;
    free(n);
    return data;
}

table get_table(int table_rows, int table_columns, int row_height, int column_width, FILE *fp) {
    table t = {table_rows, table_columns, row_height, column_width, 0, 0, fp};
    return t;
}

void print_table(table *t, char c) {
    int table_rows = t -> table_rows;
    int table_columns = t -> table_columns;
    int row_height = t -> row_height;
    int column_width = t -> column_width;
    FILE *f = t -> fp;

    int real_row_height = row_height + 1;
    int real_column_width = column_width + 3;
    int printing_rows = table_rows * (real_row_height) + 1;
    int printing_columns = table_columns * (real_column_width) + 1;
    while (t -> row_index < printing_rows) {
        while (t -> column_index < printing_columns) {
            if (t -> row_index % real_row_height == 0) {
                if (t -> column_index % real_column_width == 0)
                    fprintf(f, "+");
                else
                    fprintf(f, "-");
            }
            else {
                if (t -> column_index % real_column_width == 0)
                    fprintf(f, "|");
                else
                    if (t -> column_index % real_column_width == (real_column_width / 2) && t -> row_index % real_row_height == (real_row_height / 2)) {
                        fprintf(f, "%c", c);
                        t -> column_index++;
                        return;
                    }
                    else
                        fprintf(f, " ");
            }
            t -> column_index++;
        }
        fprintf(f, "\n");
        t -> column_index = 0;
        t -> row_index++;
    }
    return;
}
