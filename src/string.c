#include "../include/string.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "SDL3/SDL_stdinc.h"

#define safe_free(ptr) if(ptr != nullptr) {SDL_free(ptr);ptr=nullptr;}
#define INITIAL_CAPACITY 8

void string_ensure_cap(String *s, size_t cap);
void string_append_cstr_wlen(String *s, const char *cstr, size_t cstr_len);

String *String_Create() {
    String *s = SDL_malloc(sizeof(String));
    if (s == nullptr) {
        fprintf(stderr, "malloc for s failed in string_create_empty");
        exit(EXIT_FAILURE);
    }
    s->len = 0;
    s->cap = INITIAL_CAPACITY;
    s->str = SDL_malloc(sizeof(char) * INITIAL_CAPACITY);
    if (s->str == nullptr) {
        fprintf(stderr, "malloc for s->str failed in string_create_empty");
        exit(EXIT_FAILURE);
    }
    return s;
}

void string_ensure_cap(String *s, const size_t cap) {
    if (cap > s->cap) {
        const size_t new_cap = cap; // exponential growth ensures fewer reallocs
        char *new_str = SDL_realloc(s->str, sizeof(char) * new_cap);
        if (new_str == nullptr) {
            fprintf(stderr, "realloc failed in string_ensure_cap\n");
            exit(EXIT_FAILURE);
        }
        s->str = new_str;
        s->cap = new_cap;
    }
}

String *String_FromCstr(const char *cstr) {
    const size_t len = strlen(cstr);
    String *s = String_Create();
    string_ensure_cap(s, len + 1);
    strncpy(s->str, cstr, len);
    s->str[len] = 0;
    s->len = len;
    return s;
}

void string_append_cstr_wlen(String *s, const char *cstr, const size_t cstr_len) {
    const size_t new_len = s->len + cstr_len;
    string_ensure_cap(s, new_len + 1);
    for (size_t i = s->len; i < new_len; i++) {
        s->str[i] = cstr[i - s->len];
    }
    s->str[new_len] = 0;
    s->len = new_len;
}

void String_Append(String *s, const String *t) {
    return string_append_cstr_wlen(s, t->str, t->len);
}

void String_AppendCstr(String *s, const char *cstr) {
    return string_append_cstr_wlen(s, cstr, strlen(cstr));
}

void String_AppendChar(String *s, const char c) {
    return string_append_cstr_wlen(s, &c, 1);
}

String *String_Clone(const String *s) {
    String *str = String_Create();
    String_Append(str, s);
    return str;
}

String *String_ConcatCstr(const String *s, const char *cstr) {
    String *str = String_Clone(s);
    String_AppendCstr(str, cstr);
    return str;
}

void String_Clear(String *s) {
    string_ensure_cap(s, 1);
    s->str[0] = 0;
    s->len = 0;
}

void String_Destroy(String *s) {
    safe_free(s->str);
    safe_free(s);
}