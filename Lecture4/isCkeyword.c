
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <glib.h>

enum C_keyword_id
{
        C_KEYWORD_FIRST = 0,

        C89_KEYWORD_FIRST = C_KEYWORD_FIRST,
        /* C89 keyword */
        C_KEYWORD_AUTO = C89_KEYWORD_FIRST,
        C_KEYWORD_BREAK,
        C_KEYWORD_CASE,
        C_KEYWORD_CHAR,
        C_KEYWORD_CONST,
        C_KEYWORD_CONTINUE,
        C_KEYWORD_DEFAULT,
        C_KEYWORD_DO,
        C_KEYWORD_DOUBLE,
        C_KEYWORD_ELSE,
        C_KEYWORD_ENUM,
        C_KEYWORD_EXTERN,
        C_KEYWORD_FLOAT,
        C_KEYWORD_FOR,
        C_KEYWORD_GOTO,
        C_KEYWORD_IF,
        C_KEYWORD_INT,
        C_KEYWORD_LONG,
        C_KEYWORD_REGISTER,
        C_KEYWORD_RETRUN,
        C_KEYWORD_SHORT,
        C_KEYWORD_SIGNED,
        C_KEYWORD_SIZEOF,
        C_KEYWORD_STATIC,
        C_KEYWORD_STRUCT,
        C_KEYWORD_SWITCH,
        C_KEYWORD_TYPEDEF,
        C_KEYWORD_UNION,
        C_KEYWORD_UNSIGNED,
        C_KEYWORD_VOID,
        C_KEYWORD_VOLATILE,
        C_KEYWORD_WHILE,

        C89_KEYWORD_LAST = C_KEYWORD_WHILE,


        C99_KEYWORD_FIRST = C89_KEYWORD_LAST + 1,
        /*C99 keyword*/
        C_KEYWORD_INLINE = C99_KEYWORD_FIRST,
        C_KEYWORD_RESTRICT,
        C_KEYWORD__BOOL,
        C_KEYWORD__COMPLEX,
        C_KEYWORD__IMAGINARY,

        C99_KEYWORD_LAST = C_KEYWORD__IMAGINARY,

        C11_KEYWORD_FIRST = C99_KEYWORD_LAST + 1,
        /*C11 keyword*/
        C_KEYWORD__ALIGNAS = C11_KEYWORD_FIRST,
        C_KEYWORD__ALIGNOF,
        C_KEYWORD__ATOMIC,
        C_KEYWORD__GENERIC,
        C_KEYWORD__NORETURN,
        C_KEYWORD__STATIC_ASSERT,
        C_KEYWORD__THREAD_LOCAL,

        C11_KEYWORD_LAST = C_KEYWORD__THREAD_LOCAL,

        C_KEYWORD_LAST = C11_KEYWORD_LAST,
};

#define C_KEYWORD_MIN C_KEYWORD_FIRST
#define C_KEYWORD_MAX C_KEYWORD_LAST
#define C_KEYWORD_NR (C_KEYWORD_MAX - C_KEYWORD_MIN + 1)

enum c_standard_id
{
        C_STANDARD_FIRST = 0,

        C_STANDARD_C89,
        C_STANDARD_C99,
        C_STANDARD_C11,

        C_STANDARD_LAST = C_STANDARD_C11,
};

#define C_STANDARD_MIN C_STANDARD_FIRST
#define C_STANDARD_MAX C_STANDARD_LAST
#define C_STANDARD_NR (C_STANDARD_MAX - C_STANDARD_MIN + 1)

struct c_standard_tag_info
{
        GQuark          atom;
        const char*     standard_string;
};

struct c_keyword_tag_info
{
        GQuark          keyword_atom;
        const char*     keyword_string;
        GQuark          standard_atom;
        // struct c_standard_tag_info keyword_standard;
};

#define KEYWORD_BUFFER_SIZE 500

int main(int argc, char* argv[])
{
        /* initialize */

        struct c_standard_tag_info c_standard_info[] = {
                {0, "C89"},
                {0, "C99"},
                {0, "C11"},
        };

        assert (sizeof(c_standard_info) >= C_STANDARD_NR);

        for (int i = 0; i < C_STANDARD_NR; i++)
        {
                c_standard_info[i].atom =
                        g_quark_from_string(c_standard_info[i].standard_string);
        }

        struct c_keyword_tag_info c_keyword_info[] = {
                {0, "auto", 0},
                {0, "break", 0},
                {0, "case", 0},
                {0, "char", 0},
                {0, "const", 0},
                {0, "continue", 0},
                {0, "default", 0},
                {0, "do", 0},
                {0, "double", 0},
                {0, "else", 0},
                {0, "enum", 0},
                {0, "extern", 0},
                {0, "float", 0},
                {0, "for", 0},
                {0, "goto", 0},
                {0, "if", 0},
                {0, "int", 0},
                {0, "long", 0},
                {0, "register", 0},
                {0, "return", 0},
                {0, "short", 0},
                {0, "signed", 0},
                {0, "sizeof", 0},
                {0, "static", 0},
                {0, "struct", 0},
                {0, "switch", 0},
                {0, "typedef", 0},
                {0, "union", 0},
                {0, "unsigned", 0},
                {0, "void", 0},
                {0, "volatile", 0},
                {0, "while", 0},

                {0, "inline", 0},
                {0, "restrict", 0},
                {0, "_Bool", 0},
                {0, "_Complex", 0},
                {0, "_Imaginary", 0},

                {0, "_Alignas", 0},
                {0, "_Alignof", 0},
                {0, "_Atomic", 0},
                {0, "_Generic", 0},
                {0, "_Noreturn", 0},
                {0, "_Static_assert", 0},
                {0, "_Thread_local", 0},
        };

        assert (sizeof(c_keyword_info) >= C_KEYWORD_NR);


        for (int i = C89_KEYWORD_FIRST; i < C89_KEYWORD_LAST; i++)
        {
                c_keyword_info[i].keyword_atom =
                        g_quark_from_string(c_keyword_info[i].keyword_string);
                c_keyword_info[i].standard_atom = c_standard_info[0].atom;
        }

        for (int i = C99_KEYWORD_FIRST; i < C99_KEYWORD_LAST; i++)
        {
                c_keyword_info[i].keyword_atom =
                        g_quark_from_string(c_keyword_info[i].keyword_string);
                c_keyword_info[i].standard_atom = c_standard_info[1].atom;
        }

        for (int i = C11_KEYWORD_FIRST; i < C11_KEYWORD_LAST; i++)
        {
                c_keyword_info[i].keyword_atom =
                        g_quark_from_string(c_keyword_info[i].keyword_string);
                c_keyword_info[i].standard_atom = c_standard_info[2].atom;
        }

        char keyword_string[KEYWORD_BUFFER_SIZE] = {0};

        printf("Please Type a word to find if it is a keyword in C\n");

        while (scanf("%s", keyword_string))
        {
                GQuark keyword_atom =
                        g_quark_from_string(keyword_string);

                int i = 0;

                for (i = C_KEYWORD_FIRST;
                        i < C_KEYWORD_LAST; i++)
                {
                        if (c_keyword_info[i].keyword_atom
                                == keyword_atom)
                        {
                                break;
                        }
                }

                if (i < C_KEYWORD_LAST)
                {
                        printf("%s is %s standard keyword\n",
                                keyword_string,
                                g_quark_to_string(c_keyword_info[i].standard_atom));
                }
                else
                {
                        printf("%s is not a C keyword\n",
                        keyword_string);
                }

                printf("Type word to continue, Ctrl + C to end\n");
        }

        return 0;

}