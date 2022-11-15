#include "decl.h"
#include "stmt.h"
#include "symbol.h"
#include "scope.h"
#include "type.h"
#include "expr.h"
#include "param_list.h"
#include "special.h"
extern int MAIN_RESOLVEERROR_COUNT;
struct decl *decl_create(char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next)
{
    struct decl *d = special_xmalloc(sizeof(*d));
    d->name = name;
    d->type = type;
    d->value = value;
    d->code = code;
    d->next = next;
    return d;
}

void decl_print(struct decl *d, int tab)
{
    if (d)
    {
        printf("%s", d->name);
        printf(":");
        type_print(d->type, tab);
        if (d->value)
        {
            printf(" = ");
        }
        expr_print(d->value, tab);
        if (d->type->kind == TYPE_FUNCTION && d->code)
        {
            printf(" = ");
        }
        stmt_print(d->code, tab);
        if (d->value || !d->code)
        {
            printf(";");
        }
        printf("\n");
        special_tabprinter(tab);
        decl_print(d->next, tab);
    }
}

void decl_resolve(struct decl *d)
{
    if (!d)
        return;
    // created a symbol for d
    d->symbol = symbol_create(scope_level() > 1 ? SYMBOL_LOCAL : SYMBOL_GLOBAL, d->type, d->name, 0, 0);

    if (d->type->kind == TYPE_FUNCTION && !d->code)
    {
        d->symbol->prototype = 1;
    }

    // x:integer; or y:integer = 10;
    struct symbol *s = scope_lookup_current(d->name);

    if (s && type_compare(s->type, d->type) && s->prototype)
    {
        // check if it is a prototype and the types match
        MAIN_RESOLVEERROR_COUNT++;
        printf("resolve error: %s is already defined", s->name);
    }

    expr_resolve(d->value);
    scope_bind(d->name, d->symbol);

    // main: function void = { ... }

    if (d->code)
    {
        scope_enter();
        param_list_resolve(d->type->params);
        stmt_resolve(d->code);
        scope_exit();
    }
    // GOTO next decl
    decl_resolve(d->next);
    return;
}

void decl_typecheck(struct decl *d)
{
    if (!d)
        return;

    if (d->value)
    {
        struct type *t = expr_typecheck(d->value);
        if (d->symbol->type->kind == TYPE_AUTO)
        {
            d->symbol->type = type_copy(t);
        }
        if (!type_compare(t, d->symbol->type))
        {
            special_decl_error_handler(d, t);
        }
    }
    if (d->code)
    {
        struct type *r = NULL;
        stmt_typecheck(d->code, r);
        if (r && !type_compare(r, d->type))
        {
            special_decl_error_handler(d, r);
        }
    }
    decl_typecheck(d->next);
}
