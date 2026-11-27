#include "definitions.h"
#include <stdlib.h>
#include <stdio.h>

character_node *get_character_node(char c, pointer_list *memory) {
    character_node *n = (character_node *)my_malloc(sizeof(character_node), memory);
    if (n == NULL)
        return NULL;
    n -> data = c;
    n -> next = NULL;
    return n;
}

string *get_string(pointer_list *memory) {
    string *s = (string *) my_malloc(sizeof(string), memory);
    if (s == NULL)
        return NULL;
    s -> length = 0;
    s -> head = NULL;
    s -> tail = NULL;
    return s;
}

string *create_string(char *s, pointer_list *memory) {
    string *result = get_string(memory);
    if (result == NULL)
        return NULL;
    for (char *c = s; *c != '\0'; c++) {
        int err = append_character(result, *c, memory);
        if (err)
            return NULL;
    }
    return result;
}

void free_string(string *s, pointer_list *memory) {
    character_node *next = NULL;
    for (character_node *ptr = s -> head; ptr != NULL; ptr = next) {
        next = ptr -> next;
        my_free(ptr, memory);
    }
    my_free(s, memory);
}

string_node *get_string_node(string *s, pointer_list *memory) {
    string_node *n = (string_node *) my_malloc(sizeof(string_node), memory);
    if (n == NULL)
        return NULL;
    n -> data = s;
    n -> next = NULL;
    return n;
}

void free_string_node(string_node *n, pointer_list *memory) {
    free_string(n -> data, memory);
    my_free(n, memory);
}

string_list *get_string_list(pointer_list *memory) {
    string_list *list = (string_list *)my_malloc(sizeof(string_list), memory);
    if (list == NULL)
        return NULL;
    list -> length = 0;
    list -> head = NULL;
    list -> tail = NULL;
    return list;
}

void free_string_list(string_list *list, pointer_list *memory) {
    string_node *next = NULL;
    for (string_node *ptr = list -> head; ptr != NULL; ptr = next) {
        next = ptr -> next;
        free_string_node(ptr, memory);
    }
    my_free(list, memory);
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

int append_character(string *s, char c, pointer_list *memory) {
    character_node *n = get_character_node(c, memory);
    if (n == NULL)
        return 1;
    if (s -> tail == NULL) {
        s -> tail = n;
        s -> head = n;
    }
    else {
        s -> tail -> next = n;
        s -> tail = n;
    }
    s -> length++;
    return 0;
}

int prepend_character(string *s, char c, pointer_list *memory) {
    character_node *n = get_character_node(c, memory);
    if (n == NULL)
        return 1;
    if (s -> head == NULL) {
        s -> head = n;
        s -> tail = n;
    }
    else {
        n -> next = s -> head;
        s -> head = n;
    }
    s -> length++;
    return 0;
}

int insert_character_sorted(string *s, char c, int allow_duplicates, pointer_list *memory) {
    if (s -> head == NULL) {
        character_node *n = get_character_node(c, memory);
        if (n == NULL)
            return 1;
        s -> head = n;
        s -> tail = n;
        s -> length++;
    }
    else if (c < s -> head -> data) {
        int err = prepend_character(s, c, memory);
        if (err)
            return 1;
    }
    else if (s -> tail -> data < c) {
        int err = append_character(s, c, memory);
        if (err)
            return 1;
    }
    else
        for (character_node *ptr = s -> head; ptr != NULL; ptr = ptr -> next) {
            if (ptr -> data == c && !allow_duplicates)
                break;
            if (ptr -> data <= c && c < ptr -> next -> data) {
                character_node *n = get_character_node(c, memory);
                if (n == NULL)
                    return 1;
                n -> next = ptr -> next;
                ptr -> next = n;
                s -> length++;
                break;
            }
        }
    return 0;
}

void remove_character(string *s, char c, pointer_list *memory) {
    character_node *prev = NULL;
    for (character_node *ptr = s -> head; ptr != NULL; ) {
        character_node *next = ptr -> next;
        if (ptr -> data == c) {
            if (prev == NULL)
                s -> head = next;
            else
                prev -> next = next;
            if (next == NULL)
                s -> tail = prev;
            my_free(ptr, memory);
            s -> length--;
        }
        else {
            prev = ptr;
        }
        ptr = next;
    }
}

char *generate_character_array(string *s, pointer_list *memory) {
    char *array = (char *) my_malloc((s -> length + 1) * sizeof(char), memory);
    if (array == NULL)
        return NULL;
    int i;
    for (i = 0; i < s -> length; i++) {
        character_node *first = s -> head;
        array[i] = first -> data;
        s -> head = first -> next;
        my_free(first, memory);
    }
    my_free(s, memory);
    array[i] = '\0';
    return array;
}

char *input_string(pointer_list *memory) {
    string *s = get_string(memory);
    if (s == NULL)
        return NULL;
    int ch;
    while ((ch = getchar()) != EOF && ch != '\n') {
        int err = append_character(s, ch, memory);
        if (err)
            return NULL;
    }
    char *result = generate_character_array(s, memory);
    return result;
}

int append_string(string_list *list, string *s, pointer_list *memory) {
    string_node *n = get_string_node(s, memory);
    if (n == NULL)
        return 1;
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
    return 0;
}

int append_string_node(string_list *list, string_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
    return 0;
}

int prepend_string(string_list *list, string *s, pointer_list *memory) {
    string_node *n = get_string_node(s, memory);
    if (n == NULL)
        return 1;
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head = n;
    }
    list -> length++;
    return 0;
}

int prepend_string_node(string_list *list, string_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head = n;
    }
    list -> length++;
    return 0;
}

string_node *pull_node_from_front(string_list *list) {
    string_node *first_node = list -> head;
    list -> head = first_node -> next;
    list -> length--;
    return first_node;
}

string *pull_string_from_front(string_list *list, pointer_list *memory) {
    string_node *first_node = pull_node_from_front(list);
    string *s = first_node -> data;
    my_free(first_node, memory);
    return s;

}

void pop_front(string_list *list, pointer_list *memory) {
    string_node *first_node = pull_node_from_front(list);
    free_string_node(first_node, memory);
}

string_list *split(string *s, char c, pointer_list *memory) {
    string_list *result = get_string_list(memory);
    if (result == NULL)
        return NULL;
    string *str = get_string(memory);
    if (str == NULL)
        return NULL;
    int err = append_string(result, str, memory);
    if (err)
        return NULL;
    for (character_node *chr = s -> head; chr != NULL; chr = chr -> next) {
        if (chr -> data == c) {
            str = get_string(memory);
            if (s == NULL)
                return NULL;
            int err = append_string(result, str, memory);
            if (err)
                return NULL;
        }
        else {
            int err = append_character(result -> tail -> data, chr -> data, memory);
            if (err)
                return NULL;
        }
    }
    return result;
}

int_node *get_int_node(int a, pointer_list *memory) {
    int_node *n = (int_node *) my_malloc(sizeof(int_node), memory);
    if (n == NULL)
        return NULL;
    n -> data = a;
    n -> next = NULL;
    return n;
}

int_list *get_int_list(pointer_list *memory) {
    int_list *list = (int_list *) my_malloc(sizeof(int_list), memory);
    if (list == NULL)
        return NULL;
    list -> length = 0;
    list -> head = NULL;
    list -> tail = NULL;
    return list;
}

void free_int_list(int_list *list, pointer_list *memory) {
    if (list -> length == 0) {
        my_free(list, memory);
        return;
    }
    int_node *next = list -> head -> next;
    for (int_node *ptr = list -> head; ptr != NULL; ptr = next) {
        next = ptr -> next;
        my_free(ptr, memory);
    }
    my_free(list, memory);
}

int append_int(int_list *list, int a, pointer_list *memory) {
    int_node *n = get_int_node(a, memory);
    if (n == NULL)
        return 1;
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
    return 0;
}

int append_int_node(int_list *list, int_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
    return 0;
}

int prepend_int(int_list *list, int a, pointer_list *memory) {
    int_node *n = get_int_node(a, memory);
    if (n == NULL)
        return 1;
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head = n;
    }
    list -> length++;
    return 0;
}

int prepend_int_node(int_list *list, int_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head = n;
    }
    list -> length++;
    return 0;
}

int_node *pull_int_node_from_front(int_list *list) {
    int_node *first = list -> head;
    list -> head = first -> next;
    list -> length--;
    return first;
}

int pull_int_from_front(int_list *list, pointer_list *memory) {
    int_node *n = pull_int_node_from_front(list);
    int data = n -> data;
    my_free(n, memory);
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

pointer_node *get_pointer_node(void *ptr) {
    pointer_node *n = malloc(sizeof(pointer_node));
    if (n == NULL)
        return NULL;
    n -> data = ptr;
    n -> next = NULL;
    n -> prev = NULL;
    return n;
}

pointer_list *get_pointer_list() {
    pointer_list *list = malloc(sizeof(pointer_list));
    if (list == NULL)
        return NULL;
    list -> length = 0;
    list -> head = NULL;
    list -> tail = NULL;
    return list;
}

int append_pointer_node(pointer_list *list, pointer_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> prev = list -> tail;
        list -> tail -> next = n;
        list -> tail = n;
    }
    list -> length++;
    return 0;
}

int prepend_pointer_node(pointer_list *list, pointer_node *n) {
    if (list -> head == NULL) {
        list -> head = n;
        list -> tail = n;
    }
    else {
        n -> next = list -> head;
        list -> head -> prev = n;
        list -> head = n;
    }
    list -> length++;
    return 0;
}

int append_pointer(pointer_list *list, void *ptr) {
    pointer_node *n = get_pointer_node(ptr);
    if (n == NULL)
        return 1;
    append_pointer_node(list, n);
    return 0;
}

int prepend_pointer(pointer_list *list, void *ptr) {
    pointer_node *n = get_pointer_node(ptr);
    if (n == NULL)
        return 1;
    prepend_pointer_node(list, n);
    return 0;
}

void my_free(void *ptr, pointer_list *list) {
    pointer_node *n;
    for (n = list -> head; n -> data != ptr; n = n -> next);
    pointer_node *prev = n -> prev;
    pointer_node *next = n -> next;
    if (prev == NULL && next == NULL) {
        list -> head = NULL;
        list -> tail = NULL;
    }
    else if (prev == NULL) {
        list -> head = next;
        next -> prev = NULL;
    }
    else if (next == NULL) {
        list -> tail = prev;
        prev -> next = NULL;
    }
    else {
        prev -> next = next;
        next -> prev = prev;
    }
    free(ptr);
    free(n);
    list -> length--;
}

void free_pointer_list(pointer_list *list) {
    pointer_node *next = NULL;
    for (pointer_node *ptr = list -> head; ptr != NULL; ptr = next) {
        next = ptr -> next;
        free(ptr -> data);
        free(ptr);
    }
    free(list);
}

void *my_malloc(int size, pointer_list *list) {
    void *ptr = malloc(size);
    if (ptr == NULL) {
        printf("Elfogyott a szabad memóriaterület.\nKilépés.\n");
        return NULL;
    }
    int err = append_pointer(list, ptr);
    if (err) {
        free(ptr);
        printf("Elfogyott a szabad memóriaterület.\nKilépés.\n");
        return NULL;
    }
    return ptr;
}

