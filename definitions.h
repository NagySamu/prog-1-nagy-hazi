#include <stdio.h>
typedef struct character_node {
    char data;
    struct character_node *next;
} character_node;

typedef struct string {
    int length;
    character_node *head;
    character_node *tail;
} string;

typedef struct string_node {
    string *data;
    struct string_node *next;
} string_node;

typedef struct string_list {
    int length;
    string_node *head;
    string_node *tail;
} string_list;

typedef struct int_node {
    int data;
    struct int_node *next;
} int_node;

typedef struct int_list {
    int length;
    int_node *head;
    int_node *tail;
} int_list;

typedef enum {AND, OR, NOT} operation;

typedef struct {
    int table_rows;
    int table_columns;
    int row_height;
    int column_width;
    int row_index;
    int column_index;
    FILE *fp;
} table;

typedef struct pointer_node {
    void *data;
    struct pointer_node *next;
    struct pointer_node *prev;
} pointer_node;

typedef struct pointer_list {
    int length;
    pointer_node *head;
    pointer_node *tail;
} pointer_list;

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

character_node *get_character_node(char c, pointer_list *memory);
string *get_string(pointer_list *memory);
string *create_string(char *s, pointer_list *memory);
void free_string(string *s, pointer_list *memory);
string_node *get_string_node(string *s, pointer_list *memory);
void free_string_node(string_node *n, pointer_list *memory);
string_list *get_string_list(pointer_list *memory);
void free_string_list(string_list *list, pointer_list *memory);
void print_string(string s);
void print_string_list(string_list list);
int strf(char s[], char c);
int append_character(string *s, char c, pointer_list *memory);
int prepend_character(string *s, char c, pointer_list *memory);
int insert_character_sorted(string *s, char c, int allow_duplicates, pointer_list *memory);
void remove_character(string *s, char c, pointer_list *memory);
char *generate_character_array(string *s, pointer_list *memory);
char *input_string(pointer_list *memory);
int append_string(string_list *list, string *s, pointer_list *memory);
int append_string_node(string_list *list, string_node *n);
int prepend_string(string_list *list, string *s, pointer_list *memory);
int prepend_string_node(string_list *list, string_node *n);
string_node *pull_node_from_front(string_list *list);
string *pull_string_from_front(string_list *list, pointer_list *memory);
void pop_front(string_list *list, pointer_list *memory);
string_list *split(string *s, char c, pointer_list *memory);
int_node *get_int_node(int a, pointer_list *memory);
int_list *get_int_list(pointer_list *memory);
void free_int_list(int_list *list, pointer_list *memory);
int append_int(int_list *list, int a, pointer_list *memory);
int append_int_node(int_list *list, int_node *n);
int prepend_int(int_list *list, int a, pointer_list *memory);
int prepend_int_node(int_list *list, int_node *n);
int pull_int_from_front(int_list *list, pointer_list *memory);
int_node *pull_int_node_from_front(int_list *list);
table get_table(int table_rows, int table_columns, int row_height, int column_width, FILE *fp);
void print_table(table *t, char c);
pointer_node *get_pointer_node(void *ptr);
pointer_list *get_pointer_list();
int append_pointer_node(pointer_list *list, pointer_node *n);
int prepend_pointer_node(pointer_list *list, pointer_node *n);
int append_pointer(pointer_list *list, void *ptr);
int prepend_pointer(pointer_list *list, void *ptr);
void my_free(void *ptr, pointer_list *list);
void free_pointer_list(pointer_list *list);
void *my_malloc(int size, pointer_list *list);

#endif
