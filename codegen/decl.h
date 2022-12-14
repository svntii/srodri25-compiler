struct decl
{
	char *name;
	struct type *type;
	struct expr *value;
	struct stmt *code;
	struct symbol *symbol;
	struct decl *next;
	int local_count;
};

struct decl *decl_create(char *name, struct type *type, struct expr *value, struct stmt *code, struct decl *next);
void decl_print(struct decl *d, int tab);
void decl_typecheck(struct decl *d);
void decl_resolve(struct decl *d);
void decl_typecheck(struct decl *d);
void decl_codegen(struct decl *d);
void decl_arraycodegen(struct expr *e);
void decl_argcodegen(struct expr *e);
