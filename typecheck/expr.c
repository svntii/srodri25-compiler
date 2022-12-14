#include "expr.h"
#include "type.h"
#include "symbol.h"
#include "param_list.h"
#include "scope.h"
#include "stmt.h"
#include "decl.h"
#include "special.h"
extern int MAIN_RESOLVEERROR_COUNT;
struct expr *expr_create(expr_t kind, struct expr *left, struct expr *right)
{
    struct expr *e = special_xmalloc(sizeof(struct expr)); // we have declared a pointer called e and allocated appropriate memory to that pointer
    e->kind = kind;
    e->left = left;
    e->right = right;
    return e;
}

struct expr *expr_create_name(const char *name)
{
    struct expr *e = expr_create(EXPR_NAME, 0, 0);
    e->name = name;
    return e;
}

struct expr *expr_create_integer_literal(int i)
{
    struct expr *e = expr_create(EXPR_INTEGER_LITERAL, 0, 0);
    e->literal_value = i;
    return e;
}

struct expr *expr_create_boolean_literal(int b)
{
    struct expr *e = expr_create(EXPR_BOOLEAN_LITERAL, 0, 0);
    e->literal_value = b;
    return e;
}

struct expr *expr_create_char_literal(char c)
{
    struct expr *e = expr_create(EXPR_CHAR_LITERAL, 0, 0);
    e->literal_value = c; // look again
    return e;
}

struct expr *expr_create_string_literal(const char *str)
{
    struct expr *e = expr_create(EXPR_STRING_LITERAL, 0, 0);
    e->string_literal = str;
    return e;
}

void expr_print(struct expr *e, int tab)
{
    if (e)
    {
        switch (e->kind)
        {
        case EXPR_ADD:
            expr_print(e->left, tab);
            printf(" + ");
            expr_print(e->right, tab);
            break;
        case EXPR_SUB:
            expr_print(e->left, tab);
            printf(" - ");
            expr_print(e->right, tab);
            break;
        case EXPR_MULT:
            expr_print(e->left, tab);
            printf(" * ");
            expr_print(e->right, tab);
            break;
        case EXPR_DIV:
            expr_print(e->left, tab);
            printf(" / ");
            expr_print(e->right, tab);
            break;
        case EXPR_OR:
            expr_print(e->left, tab);
            printf(" || ");
            expr_print(e->right, tab);
            break;
        case EXPR_AND:
            expr_print(e->left, tab);
            printf(" && ");
            expr_print(e->right, tab);
            break;
        case EXPR_LT:
            expr_print(e->left, tab);
            printf(" < ");
            expr_print(e->right, tab);
            break;
        case EXPR_LTE:
            expr_print(e->left, tab);
            printf(" <= ");
            expr_print(e->right, tab);
            break;
        case EXPR_GT:
            expr_print(e->left, tab);
            printf(" > ");
            expr_print(e->right, tab);
            break;
        case EXPR_GTE:
            expr_print(e->left, tab);
            printf(" >= ");
            expr_print(e->right, tab);
            break;
        case EXPR_EQ:
            expr_print(e->left, tab);
            printf(" == ");
            expr_print(e->right, tab);
            break;
        case EXPR_NEQ:
            expr_print(e->left, tab);
            printf(" != ");
            expr_print(e->right, tab);
            break;
        case EXPR_MOD:
            expr_print(e->left, tab);
            printf(" %% ");
            expr_print(e->right, tab);
            break;
        case EXPR_EXP:
            expr_print(e->left, tab);
            printf("^");
            expr_print(e->right, tab);
            break;
        case EXPR_NEGATE:
            printf("-");
            expr_print(e->right, tab);
            break;
        case EXPR_NOT:
            printf("!");
            expr_print(e->right, tab);
            break;
        case EXPR_POSIN:
            expr_print(e->right, tab);
            printf("++");
            break;
        case EXPR_POSDEC:
            expr_print(e->right, tab);
            printf("--");
            break;
        case EXPR_GROUP:
            printf("( ");
            expr_print(e->left, tab);
            expr_print(e->right, tab);
            printf(" )");
            break;
        case EXPR_SUBSCRIPT:
            expr_print(e->left, tab);  // prints the name
            expr_print(e->right, tab); // prints the subscipts
            break;
        case EXPR_SUBSCRIPT_SUB:
            printf("[");
            expr_print(e->left, tab);
            printf("]");
            expr_print(e->right, tab);
            break;
        case EXPR_FUNCTION_CALL:
            expr_print(e->left, tab);
            printf("(");
            expr_print(e->right, tab);
            printf(")");
            break;
        case EXPR_ARG:
            expr_print(e->left, tab);
            if (e->right)
                printf(", ");
            expr_print(e->right, tab);
            break;
        case EXPR_PRINT_BODY:
            expr_print(e->left, tab);
            if (e->right)
                printf(", ");
            expr_print(e->right, tab);
            break;
        case EXPR_ASSIGNMENT:
            expr_print(e->left, tab);
            printf(" = ");
            expr_print(e->right, tab);
            break;
        case EXPR_TERN:
            expr_print(e->left, tab);
            printf(" ? ");
            expr_print(e->right, tab);
            break;
        case EXPR_TERN_BODY:
            expr_print(e->left, tab);
            printf(" : ");
            expr_print(e->right, tab);
            break;
        case EXPR_INTEGER_LITERAL:
            printf("%d", e->literal_value);
            break;
        case EXPR_BOOLEAN_LITERAL:
            if (e->literal_value)
            {
                printf("true");
            }
            else
            {
                printf("false");
            }
            break;
        case EXPR_CHAR_LITERAL:
            printf("'");
            printf("%c", e->literal_value);
            printf("'");
            break;
        case EXPR_STRING_LITERAL:

            printf("%s", e->string_literal);
            break;

        case EXPR_ARRAY_LITERAL:
            printf("{ ");
            // expr_print_array_literal(e, tab);
            expr_print(e->left, tab);
            expr_print(e->right, tab);
            printf(" }");
            break;
        case EXPR_NAME:

            printf("%s", e->name);
            break;
        default:
            break;
        }
    }
}

void expr_resolve(struct expr *e)
{
    if (!e)
        return;
    // post-order resolve. Must look at children;
    expr_resolve(e->left);
    expr_resolve(e->right);

    if (e->kind == EXPR_NAME)
    {
        struct symbol *global = scope_lookup(e->name);
        // global = scope_lookup(e->name); // # TODO confirm

        if (!global)
        {
            MAIN_RESOLVEERROR_COUNT++;
            printf("resolve error: %s is not defined\n", e->name);
        }
        else
        {
            e->symbol = global;
            printf("%s is bound to ", e->name);
            symbol_print(e->symbol);
        }
    }
}

struct type *expr_typecheck(struct expr *e)
{
    // recursive, post-order traversal of the expression tree
    if (!e)
        return NULL;

    struct type *lt = expr_typecheck(e->left);
    struct type *rt = expr_typecheck(e->right);

    struct type *result;

    switch (e->kind)
    {
    case EXPR_INTEGER_LITERAL:
        result = type_create(TYPE_INTEGER, 0, 0, 0, 0);
        break;
    case EXPR_STRING_LITERAL:
        result = type_create(TYPE_STRING, 0, 0, 0, 0);
        break;
    case EXPR_BOOLEAN_LITERAL:
        result = type_create(TYPE_BOOLEAN, 0, 0, 0, 0);
        break;
    case EXPR_CHAR_LITERAL:
        result = type_create(TYPE_CHAR, 0, 0, 0, 0);
        break;
    case EXPR_ARRAY_LITERAL:
        // { 1 , 2, 3 }
        result = type_copy(lt);
        break;
    case EXPR_ARG:
        if (type_compare(lt, rt)) // lookback
        {
            result = type_copy(lt);
        }
        else if (lt == NULL)
        {
            result = type_copy(rt);
        }
        else if (rt == NULL)
        {
            result = type_copy(lt);
        }
        else
        {
            // error message
            special_expr_error_handler(e, lt, rt);
            result = type_create(TYPE_INTEGER, 0, 0, 0, 0);
        }

        break;
    case EXPR_PRINT_BODY:
        result = type_copy(lt);
        break;
    case EXPR_NAME:
        result = type_copy(e->symbol->type);
        break;
    case EXPR_EXP:  // 2 ^ 4
    case EXPR_MOD:  // 2 % 2
    case EXPR_ADD:  // 1 + 1
    case EXPR_SUB:  // 3 - 2
    case EXPR_MULT: // 3 * 2
    case EXPR_DIV:  // 1 / 2

        if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER)
        {
            // error message
            special_expr_error_handler(e, lt, rt);
        }
        result = type_create(TYPE_INTEGER, 0, 0, 0, 0);
        break;

    case EXPR_OR:
    case EXPR_AND:
        if (lt->kind != TYPE_BOOLEAN || rt->kind != TYPE_BOOLEAN)
        {
            special_expr_error_handler(e, lt, rt);
        }
        result = type_create(TYPE_BOOLEAN, 0, 0, 0, 0);
        break;

    case EXPR_LT:
    case EXPR_LTE:
    case EXPR_GT:
    case EXPR_GTE:
        if (lt->kind != TYPE_INTEGER || rt->kind != TYPE_INTEGER)
        {
            special_expr_error_handler(e, lt, rt);
        }
        result = type_create(TYPE_BOOLEAN, 0, 0, 0, 0);
        break;

    case EXPR_EQ:
    case EXPR_NEQ:

        if (!type_compare(lt, rt))
        {
            special_expr_error_handler(e, lt, rt);
        }
        if (lt->kind == TYPE_VOID ||
            lt->kind == TYPE_ARRAY ||
            lt->kind == TYPE_FUNCTION)
        {
            /* display an error */
            special_expr_error_handler(e, lt, rt);
        }
        result = type_create(TYPE_BOOLEAN, 0, 0, 0, 0);
        break;
    case EXPR_POSIN:
    case EXPR_POSDEC:
        if (!rt || rt->kind != TYPE_INTEGER)
        {
            // print error
            special_expr_error_handler(e, lt, rt);
        }
        result = type_create(TYPE_INTEGER, 0, 0, 0, 0);
        break;
    case EXPR_NEGATE:
        if (rt->kind == TYPE_INTEGER)
        {
            result = type_create(TYPE_INTEGER, 0, 0, 0, 0);
        }
        else
        {
            // print error
            special_expr_error_handler(e, lt, rt);
        }
        break;
    case EXPR_NOT:
        if (rt->kind == TYPE_BOOLEAN)
        {
            result = type_create(TYPE_BOOLEAN, 0, 0, 0, 0);
        }
        else
        {
            // print error
            special_expr_error_handler(e, lt, rt);
        }

    case EXPR_GROUP:
        // think about
        result = type_copy(rt);
        break;
    case EXPR_SUBSCRIPT:
        if (lt->kind == TYPE_ARRAY)
        {

            result = type_copy(lt->subtype);
        }
        else
        {
            /* error: not an array */
            /* but we need to return a valid type */
            special_expr_error_handler(e, lt, rt);
            result = type_copy(lt);
        }
        break;
    case EXPR_SUBSCRIPT_SUB:
        if (lt->kind == TYPE_INTEGER)
        {
            result = type_copy(lt->subtype);
        }
        else
        {
            /* error: not an array */
            /* but we need to return a valid type */
            special_expr_error_handler(e, lt, rt);
            result = type_create(TYPE_INTEGER, 0, 0, 0, 0);
        }
        break;

    case EXPR_FUNCTION_CALL:

        if (lt->kind == TYPE_FUNCTION)
        {
            result = type_copy(lt->subtype);
        }
        else
        {
            // error message
            special_expr_error_handler(e, lt, rt);
            result = type_create(TYPE_VOID, 0, 0, 0, 0);
        }

        break;
    case EXPR_ASSIGNMENT:

        if (type_compare(lt, rt))
        {
            result = type_copy(lt);
        }
        else
        {
            // print error
            special_expr_error_handler(e, lt, rt);
            result = type_create(TYPE_INTEGER, 0, 0, 0, 0);
        }

        break;
    case EXPR_TERN:
        if (lt->kind == TYPE_BOOLEAN)
        {
            result = type_copy(rt);
        }
        else
        {
            // error message
            special_expr_error_handler(e, lt, rt);
        }
        break;
    case EXPR_TERN_BODY:
        // work on this
        if (!type_compare(lt, rt))
        {
            special_expr_error_handler(e, lt, rt);
        }

        result = type_copy(lt);
        break;

    default:
        break;
    }

    type_delete(lt);
    type_delete(rt);

    return result;
}
