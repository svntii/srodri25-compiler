
typedef enum
{
	TYPE_BOOLEAN,
	TYPE_CHAR,
	TYPE_INTEGER,
	TYPE_STRING,
	TYPE_ARRAY,
	TYPE_FUNCTION,
	TYPE_AUTO,
	TYPE_VOID
} type_t;

struct type
{
	type_t kind;
	struct type *subtype;
	struct param_list *params;
};

struct type *type_create(type_t kind, struct type *subtype, struct param_list *params);
void type_print(struct type *t);