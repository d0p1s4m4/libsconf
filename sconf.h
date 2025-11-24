/*
 * Copyright (C) 2025 d0p1.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#ifndef SCONF_H
# define SCONF_H 1

# include <stdlib.h>
# include <stdarg.h>
# include <stdio.h>
# include <stdint.h>

# ifdef __cplusplus
extern "C"
{
# endif /* __cplusplus */

# define SCONF_END ((struct sconf *)0)

enum sconf_error {
	SCONF_OK = 0,
	SCONF_ERR_MALLOC,
	SCONF_ERR_OUTOFBOUND,
	SCONF_ERR_NOTALIST,
};

enum sconf_bool {
	SCONF_FALSE = 0,
	SCONF_TRUE  = 1,
};

enum sconf_type {
	SCONF_T_NIL,
	SCONF_T_LIST,
	SCONF_T_STRING,
	SCONF_T_CHAR,
	SCONF_T_INT,
	SCONF_T_DOUBLE,
	SCONF_T_SYMBOL,
	SCONF_T_BOOL
};

struct sconf {
	enum sconf_type type;

	struct sconf *next;
	struct sconf *prev;

	union {
		char *as_string;
		int as_int;
		double as_double;
		struct sconf *as_child;
	} value;
};

const char *sconf_version(void);
struct sconf *sconf_load(FILE *fp);
struct sconf *sconf_parse(const char *str);
struct sconf *sconf_parse_with_len(const char *str, size_t len);
void sconf_dump(FILE *fp, struct sconf *sexp);
struct sconf *sconf_new_list(void);
struct sconf *sconf_new_symbol(const char *sym);
struct sconf *sconf_new_string(const char *str);
struct sconf *sconf_new_char(char c);
struct sconf *sconf_new_int(int i);
struct sconf *sconf_new_double(double d);
struct sconf *sconf_new_bool(enum sconf_bool b);
struct sconf *sconf_new_true(void);
struct sconf *sconf_new_false(void);
struct sconf *sconf_new_nil(void);
int sconf_list_append(struct sconf *lst, struct sconf *itm);
int sconf_list_appends(struct sconf *lst, ...);
int sconf_list_remove(struct sconf *lst, struct sconf *itm);
int sconf_list_size(struct sconf *lst);
struct sconf *sconf_list_at(struct sconf *lst, int idx);
struct sconf *sconf_list_first(struct sconf *lst);
void sconf_destroy(struct sconf *sexp);
enum sconf_error sconf_get_last_error(void);
const char *sconf_error_str(enum sconf_error err);

static inline int
sconf_is_bool(struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_BOOL);
}

static inline int
sconf_is_true(struct sconf *sexp)
{
	return (sconf_is_bool(sexp) && sexp->value.as_int == SCONF_TRUE);
}

static inline int
sconf_is_false(struct sconf *sexp)
{
	return (!sconf_is_bool(sexp) /* if not bool default as false */
			|| sexp->value.as_int == SCONF_FALSE);
}

static inline int
sconf_is_int(struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_INT);
}

static inline int
sconf_is_double(struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_DOUBLE);
}

static inline int
sconf_is_string(struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_STRING);
}

static inline int
sconf_is_symbol(struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_SYMBOL);
}

static inline int
sconf_is_list(struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_LIST);
}

# ifdef __cplusplus
}
# endif /* __cplusplus */

#endif /* !SCONF_H */
