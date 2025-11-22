#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "sconf.h"
#ifdef HAVE_CONFIG_H
# include "config.h"
#endif /* HAVE_CONFIG_H */

#ifndef PACKAGE_VERSION
# define PACKAGE_VERSION "?.?.?"
#endif

static enum sconf_error sconf_last_error = SCONF_OK;

const char *
sconf_version(void)
{
	return (PACKAGE_VERSION);
}

enum sconf_error
sconf_get_last_error(void)
{
	return (sconf_last_error);
}

const char *
sconf_error_str(enum sconf_error err)
{
	switch (err)
	{
	case SCONF_OK:
		return ("ok");
	case SCONF_ERR_MALLOC:
		return ("failed to allocate memory");
	case SCONF_ERR_OUTOFBOUND:
		return ("index out of bound");
	case SCONF_ERR_NOTALIST:
		return ("is not a list");
	default:
		return ("???");
	}
}

static inline struct sconf *
sconf_new(void)
{
	struct sconf *sexp;

	sexp = (struct sconf *)malloc(sizeof(struct sconf));
	if (sexp == NULL)
	{
		sconf_last_error = SCONF_ERR_MALLOC;
	}

	sexp->next = NULL;
	sexp->prev = NULL;

	return (sexp);
}

struct sconf *
sconf_new_list(void)
{
	struct sconf *sexp;

	sexp = sconf_new();
	if (sexp == NULL) return (NULL);
	sexp->type = SCONF_T_LIST;
	sexp->value.as_child = NULL;

	return (sexp);
}

struct sconf *
sconf_new_int(int i)
{
	struct sconf *sexp;

	sexp = sconf_new();
	if (sexp == NULL) return (NULL);
	sexp->type = SCONF_T_INT;
	sexp->value.as_int = i;

	return (sexp);
}

struct sconf *
sconf_new_double(double d)
{
	struct sconf *sexp;

	sexp = sconf_new();
	if (sexp == NULL) return (NULL);
	sexp->type = SCONF_T_DOUBLE;
	sexp->value.as_double = d;

	return (sexp);
}

struct sconf *
sconf_new_char(char c)
{
	struct sconf *sexp;

	sexp = sconf_new();
	if (sexp == NULL) return (NULL);
	sexp->type = SCONF_T_CHAR;
	sexp->value.as_int = c;

	return (sexp);
}

struct sconf *
sconf_new_bool(enum sconf_bool b)
{
	struct sconf *sexp;

	sexp = sconf_new();
	if (sexp == NULL) return (NULL);
	sexp->type = SCONF_T_BOOL;
	sexp->value.as_int = b;

	return (sexp);
}

struct sconf *
sconf_new_true(void)
{
	return (sconf_new_bool(SCONF_TRUE));
}

struct sconf *
sconf_new_false(void)
{
	return (sconf_new_bool(SCONF_FALSE));
}

struct sconf *
sconf_new_symbol(const char *sym)
{
	struct sconf *sexp;
	char *ptr;

	sexp = sconf_new();
	if (sexp == NULL) return (NULL);
	ptr = strdup(sym);
	if (ptr == NULL)
	{
		free(sexp);
		sconf_last_error = SCONF_ERR_MALLOC;
		return (NULL);
	}
	sexp->type = SCONF_T_SYMBOL;
	sexp->value.as_string = ptr;

	return (sexp);
}

struct sconf *
sconf_new_string(const char *str)
{
	struct sconf *sexp;

	sexp = sconf_new_symbol(str);
	if (sexp == NULL) return (NULL);

	sexp->type = SCONF_T_STRING;

	return (sexp);
}

struct sconf *
sconf_new_nil(void)
{
	struct sconf *sexp;

	sexp = sconf_new();
	if (sexp == NULL) return (NULL);

	sexp->type = SCONF_T_NIL;
	return (sexp);
}

int
sconf_list_append(struct sconf *lst, struct sconf *itm)
{
	struct sconf *child;

	if (lst == NULL || itm == NULL)
	{
		return (SCONF_FALSE);
	}

	child = lst->value.as_child;

	if (child == NULL)
	{
		lst->value.as_child = itm;
		itm->prev = itm;
	}
	else
	{
		child->prev->next = itm;
		itm->prev = child->prev;
		child->prev = itm;
	}

	return (SCONF_TRUE);
}

int
sconf_list_appends(struct sconf *lst, ...)
{
	va_list ap;
	struct sconf *itm;

	if (lst == NULL)
	{
		return (SCONF_FALSE);
	}

	va_start(ap, lst);
	for (itm = va_arg(ap, struct sconf *);
		 itm != NULL;
		 itm = va_arg(ap, struct sconf *))
	{
		sconf_list_append(lst, itm);
	}
	va_end(ap);

	return (SCONF_TRUE);
}

int
sconf_list_remove(struct sconf *lst, struct sconf *itm)
{
	struct sconf *child;
	struct sconf *tmp;

	if (lst == NULL || itm == NULL)
	{
		return (SCONF_FALSE);
	}

	child = lst->value.as_child;
	if (child == NULL)
	{
		return (SCONF_FALSE);
	}

	if (itm != child)
	{
		itm->prev->next = itm->next;
	}
	if (itm->next != NULL)
	{
		itm->next->prev = itm->prev;
	}

	if (itm == child)
	{
		lst->value.as_child = itm->next;
	}
	else if (itm->next == NULL)
	{
		child->prev = itm->prev;
	}

	itm->next = NULL;
	itm->prev = NULL;

	return (SCONF_TRUE);
}

int
sconf_list_size(struct sconf *lst)
{
	int sz;
	struct sconf *tmp;

	if (lst == NULL) return (-1);

	sz = 0;
	for (tmp = lst->value.as_child; tmp != NULL; tmp = tmp->next)
	{
		sz++;
	}

	return (sz);
}

struct sconf *
sconf_list_at(struct sconf *lst, int idx)
{
	int curr_idx;
	struct sconf *tmp;

	if (lst == NULL) return (NULL);
	if (idx < 0) return (NULL);

	curr_idx = 0;
	for (tmp = lst->value.as_child; tmp != NULL; tmp = tmp->next)
	{
		if (curr_idx == idx) return (tmp);
		curr_idx++;
	}
	return (NULL);
}

struct sconf *
sconf_list_first(struct sconf *lst)
{
	if (lst == NULL) return (NULL);
	if (lst->type != SCONF_T_LIST) return (NULL);

	return (lst->value.as_child);
}

void
sconf_destroy(struct sconf *sexp)
{
	struct sconf *cur;
	struct sconf *next;

	if (sexp == NULL) return;

	if (sexp->type == SCONF_T_SYMBOL
		|| sexp->type == SCONF_T_STRING)
	{
		free(sexp->value.as_string);
	}
	else if (sexp->type == SCONF_T_LIST)
	{
		cur = sexp->value.as_child;
		while (cur != NULL)
		{
			next = cur->next;
			sconf_destroy(cur);
			cur = next;
		}
	}

	free(sexp);
}

static void
dump_level(FILE *fp, struct sconf *sexp, int level)
{
	struct sconf *child;

	if (fp == NULL || sexp == NULL) return;

	switch (sexp->type)
	{
	case SCONF_T_SYMBOL:
		fprintf(fp, "%s", sexp->value.as_string);
		break;
	case SCONF_T_STRING:
		fprintf(fp, "\"%s\"", sexp->value.as_string);
		break;
	case SCONF_T_INT:
		fprintf(fp, "%d", sexp->value.as_int);
		break;
	case SCONF_T_DOUBLE:
		fprintf(fp, "%f", sexp->value.as_double);
		break;
	case SCONF_T_NIL:
		fprintf(fp, "nil");
		break;
	case SCONF_T_BOOL:
		if (sexp->value.as_int == SCONF_TRUE)
		{
			fprintf(fp, "true");
		}
		else
		{
			fprintf(fp, "false");
		}
		break;
	case SCONF_T_CHAR:
		fprintf(fp, "%c", (char)sexp->value.as_int);
		break;
	case SCONF_T_LIST: /* XXX: rework indentation */
		if (level > 1)
		{
			fprintf(fp, "\n");
		}
		fprintf(fp, "%*s", level, "(");
		for (child = sexp->value.as_child;
			 child != NULL;
			 child = child->next)
		{
			dump_level(fp, child, level+1);
			if (child->next != NULL)
			{
				fprintf(fp, " ");
			}
		}
		fprintf(fp, ")");
		break;
	}
}

void
sconf_dump(FILE *fp, struct sconf *sexp)
{
	dump_level(fp, sexp, 1);
}

/*
 * ---------------------------------------------------------------------------
 * parser
 * ---------------------------------------------------------------------------
 */

struct cstr {
	size_t cap;
	size_t cnt;
	char *s;
};

struct parser {
	const char *data;
	size_t len;
	size_t off;
	struct cstr buff;
};

#define CSTR_BASE_CAP 8

static inline void
cstr_init(struct cstr *cs)
{
	cs->cap = 0;
	cs->cnt = 0;
	cs->s = NULL;
}

static inline void
cstr_destroy(struct cstr *cs)
{
	cs->cap = 0;
	cs->cnt = 0;
	free(cs->s);
	cs->s = NULL;
}

static inline void
cstr_reset(struct cstr *cs)
{
	cs->cnt = 0;
}

static inline void
cstr_grow(struct cstr *cs)
{
	if (cs->cap < (cs->cnt + 1))
	{
		cs->cap = cs->cap >= CSTR_BASE_CAP ? cs->cap * 2 : CSTR_BASE_CAP;
		cs->s = realloc(cs->s, cs->cap * sizeof(char));
	}
}

static void
cstr_append(struct cstr *cs, char c)
{
	cstr_grow(cs);
	cs->s[cs->cnt++] = c;
}

static int parse_value(struct sconf *itm, struct parser *p);

static inline int
parse_get(struct parser *p)
{
	int c;

	if (p->off >= p->len) return (EOF);

	c = *(p->data + p->off);
	if (c == '\0') return (EOF);

	return (c);
}

static inline int
parse_next(struct parser *p)
{
	int c;

	c = parse_get(p);
	if (c == EOF) return (EOF);

	p->off++;

	return (c);
}

static void
parse_comment(struct parser *p)
{
	int c;

	c = parse_next(p);
	while (c != EOF)
	{
		c = parse_next(p);
		if (c == '\n') return;
	}
}

static void
parse_skip(struct parser *p)
{
redo:
	while (isspace(parse_get(p)))
	{
		p->off++;
	}

	if (parse_get(p) == ';')
	{
		parse_comment(p);
		goto redo;
	}
}

static int
parse_list(struct sconf *itm, struct parser *p)
{
	int c;
	struct sconf *tmp;

	itm->type = SCONF_T_LIST;
	itm->value.as_child = NULL;

	do
	{
		parse_skip(p);
		c = parse_get(p);
		if (c == ')')
		{
			return (SCONF_TRUE);
		}
		else if (c == EOF)
		{
			return (SCONF_FALSE);
		}

		tmp = sconf_new();
		if (parse_value(tmp, p) != SCONF_TRUE)
		{
			sconf_destroy(tmp);
			return (SCONF_FALSE);
		}

		sconf_list_append(itm, tmp);
	}
	while (parse_get(p) != EOF);

	return (SCONF_FALSE);
}

static int
parse_number(struct sconf *itm, struct parser *p)
{
	int floating;
	int i;
	int c;
	double val;

	cstr_reset(&p->buff);
	floating = 0;
	i = 0;
	do
	{
		c = parse_next(p);
		cstr_append(&p->buff, c);
		if (c == '.')
		{
			floating = 1;
		}
		c = parse_get(p);
	}
	while (i < 127 && (isalnum(c) || c == '-' || c == '.'));
	cstr_append(&p->buff, '\0');

	val = strtod(p->buff.s, NULL);
	if (floating)
	{
		itm->type = SCONF_T_DOUBLE;
		itm->value.as_double = val;
	}
	else
	{
		itm->type = SCONF_T_INT;
		itm->value.as_int = (int)val;
	}

	return (SCONF_TRUE);
}

static int
parse_symbol(struct sconf *itm, struct parser *p)
{
	int c;
	int i;

	cstr_reset(&p->buff);

	i = 0;
	do
	{
		c = parse_next(p);
		cstr_append(&p->buff, c);
		c = parse_get(p);
	}
	while (!isspace(c) && c != EOF && c != ')' && c != '(' && i < 127);

	cstr_append(&p->buff, '\0');

	if (strcmp(p->buff.s, "yes") == 0 || strcmp(p->buff.s, "true") == 0)
	{
		itm->type = SCONF_T_BOOL;
		itm->value.as_int = SCONF_TRUE;
	}
	else if (strcmp(p->buff.s, "no") == 0 || strcmp(p->buff.s, "false") == 0)
	{
		itm->type = SCONF_T_BOOL;
		itm->value.as_int = SCONF_FALSE;
	}
	else if (strcmp(p->buff.s, "nil") == 0)
	{
		itm->type = SCONF_T_NIL;
	}
	else
	{
		itm->type = SCONF_T_SYMBOL;
		itm->value.as_string = strdup(p->buff.s);
	}

	return (SCONF_TRUE);
}

static int
parse_string(struct sconf *itm, struct parser *p)
{
	int c;

	cstr_reset(&p->buff);
	do
	{
		c = parse_next(p);
		if (c == '"')
		{
			cstr_append(&p->buff, '\0');

			itm->type = SCONF_T_STRING;
			itm->value.as_string = strdup(p->buff.s);
			return (SCONF_TRUE);
		}
		else if (c == '\\')
		{
			c = parse_next(p);
			switch (c)
			{
			case 'n':
				cstr_append(&p->buff, '\n');
				break;
			case 'r':
				cstr_append(&p->buff, '\r');
				break;
			case '"':
				cstr_append(&p->buff, '"');
				break;
			default:
				cstr_append(&p->buff, '\\');
				cstr_append(&p->buff, c);
				break;
			}
		}
		else
		{
			cstr_append(&p->buff, c);
		}

		c = parse_get(p);
	}
	while (c != EOF);

	/* unexpected eof */
	return (SCONF_FALSE);
}

static int
parse_value(struct sconf *itm, struct parser *p)
{
	int c;

	parse_skip(p);

	c = parse_get(p);
	switch (c)
	{
	case EOF:
		return (SCONF_FALSE);
		break;
	case '(':
		p->off++;
		return (parse_list(itm, p));
		break;
	case '\\':
		break; /* char */
	case '"':
		p->off++;
		return (parse_string(itm, p));
		break; /* string */
	default:
		if (isdigit(c) || c == '-')
		{
			return (parse_number(itm, p));
		}
		else
		{
			return (parse_symbol(itm, p));
		}
		break;
	}

	return (SCONF_FALSE);
}

struct sconf *
sconf_parse_with_len(const char *str, size_t len)
{
	struct parser p;
	struct sconf *sexp;

	if (str == NULL || len == 0)
	{
		return (NULL);
	}

	p.data = str;
	p.len = len;
	p.off = 0;
	cstr_init(&p.buff);

	sexp = sconf_new();
	if (sexp == NULL) return (NULL);

	if (parse_value(sexp, &p) == SCONF_FALSE)
	{
		cstr_destroy(&p.buff);
		sconf_destroy(sexp);
		return (NULL);
	}

	cstr_destroy(&p.buff);
	return (sexp);
}

struct sconf *
sconf_parse(const char *str)
{
	if (str == NULL || *str == '\0')
	{
		return (NULL);
	}

	return (sconf_parse_with_len(str, strlen(str)));
}

struct sconf *
sconf_load(FILE *fp)
{
	long fsz;
	char *content;

	if (fseek(fp, 0, SEEK_END) != 0)
	{
		return (NULL);
	}

	fsz = ftell(fp);
	if (fsz <= 0)
	{
		return (NULL);
	}

	rewind(fp);

	content = (char *)malloc(fsz + sizeof(char));
	if (content == NULL)
	{
		return (NULL);
	}

	if (fread(content, fsz, 1, fp) != 1)
	{
		return (NULL);
	}

	content[fsz] = '\0';

	return (sconf_parse_with_len(content, fsz + 1));
}
