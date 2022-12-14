

struct param_list
{
    char *name;
    struct type *type;
    struct symbol *symbol;
    struct param_list *next;
};

struct param_list *param_list_create(char *name, struct type *type, struct symbol *symbol, struct param_list *next);
void param_list_print(struct param_list *p, int tab);
struct param_list *param_list_copy(struct param_list *param);
int param_list_compare(struct param_list *a, struct param_list *b);
void param_list_resolve(struct param_list *p);
void param_list_delete(struct param_list *p);
