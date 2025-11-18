#ifndef STRING_LIB_LIBRARY_H
#define STRING_LIB_LIBRARY_H
#include <stddef.h>

typedef struct {
    size_t len;
    size_t cap;
    char *str;
} String;

/**
 * @brief Creates an empty string
 * @return Pointer to string, must be freed with string_destroy at some point to avoid leaks
 */
String *String_Create();

/**
 * @brief Creates a string from a c string
 * @param cstr Zero-terminated C string
 * @return Pointer to string with content corresponding to cstr, must be freed with string_destroy at some point
 */
String *String_FromCstr(const char *cstr);

/**
 * @brief Append a string to another string
 * @param s The string to be modified
 * @param t The string to be appended to s
 */
void String_Append(String *s, const String *t);

/**
 * @brief Append a c-string to a string
 * @param s The string to be modified
 * @param cstr The (zero-terminated) C-string to be appended
 */
void String_AppendCstr(String *s, const char *cstr);

void String_AppendChar(String *s, char c);
String *String_Clone(const String *s);
String *String_ConcatCstr(const String *s, const char *cstr);

/**
 * @brief Sets the string to an empty string (of length 0)
 * @param s String to be cleared
 */
void String_Clear(String *s);

/**
 * @brief Frees allocated memory of string
 * @param s Target string pointer
 */
void String_Destroy(String *s);

#endif //STRING_LIB_LIBRARY_H