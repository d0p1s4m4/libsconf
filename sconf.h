/*
 * Copyright (C) 2025 d0p1.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

/**
 * \file sconf.h
 * \brief Lightweith S-expression parsing library.
 *
 * libsconf provides a simple interface to parse, inspect, create,
 * manipulate S-expressions.
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

/**
 * \brief Marks termination in variadic list append calls.
 */
# define SCONF_END ((struct sconf *)0)

/**
 * \enum sconf_error
 * \brief Error codes returned by libsconf.
 */
enum sconf_error {
	SCONF_OK = 0,          /**< No error */
	SCONF_ERR_MALLOC,      /**< Memory allocation failed */
	SCONF_ERR_OUTOFBOUND,  /**< Index out of range */
	SCONF_ERR_NOTALIST,    /**< Value is not a list */
	SCONF_ERR_EOF,         /**< Unexpected EOF during parsing */
};

/**
 * \enum sconf_bool
 * \brief Boolean values.
 */
enum sconf_bool {
	SCONF_FALSE = 0,
	SCONF_TRUE  = 1,
};

/**
 * \enum sconf_type
 * \brief Type of an S-expression object.
 */
enum sconf_type {
	SCONF_T_NIL,    /**< Empty value */
	SCONF_T_LIST,   /**< List of S-expression */
	SCONF_T_STRING, /**< Null-terminated string */
	SCONF_T_CHAR,   /**< Single character */
	SCONF_T_INT,    /**< Integer number */
	SCONF_T_DOUBLE, /**< Floating-point number */
	SCONF_T_SYMBOL, /**< Symbol identifier */
	SCONF_T_BOOL    /**< Boolean */
};

/**
 * \struct sconf
 * \brief Represents a single S-expression object.
 */
struct sconf {
	enum sconf_type type; /**< object type */

	struct sconf *next;   /**< next element in list */
	struct sconf *prev;   /**< previous element in list */

	union {
		char *as_string;        /**< For strings and symbols */
		int as_int;             /**< For integers and booleans */
		double as_double;       /**< For floating-point numbers */
		struct sconf *as_child; /**< For list first elements */
	} value;
};

/**
 * \brief Get library version as a constant string (eg: 1.0.0).
 * \return Null-terminated version string
 */
const char *sconf_version(void);

/**
 * \brief Parse S-expression from an open FILE stream.
 * \param fp input file pointer
 * \return Parsed object or NULL on error.
 */
struct sconf *sconf_load(FILE *fp);

/**
 * \brief Parse S-expression from a null-terminated string.
 * \param str input null-terminated string
 * \return Parsed object or NULL on error.
 */
struct sconf *sconf_parse(const char *str);

/**
 * \brief Parse S-expression from buffer with length.
 * \param str input buffer
 * \param len buffer length
 * \return Parsed object or NULL on error.
 */
struct sconf *sconf_parse_with_len(const char *str, size_t len);

/**
 * \brief Pretty-print an S-expression to a stream.
 * \param fp output stream
 * \param sexp S-expression.
 */
void sconf_dump(FILE *fp, const struct sconf *sexp);

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

/**
 * \brief Append an object to a list.
 * \param lst list root
 * \param itm element to add
 * \return 0 on success.
 */
int sconf_list_append(struct sconf *lst, struct sconf *itm);

/**
 * \brief Append multiple objects to a list.
 * \param lst list root
 * \param ... elements to add, ending with SCONF_END
 * \return 0 on success.
 */
int sconf_list_appends(struct sconf *lst, ...);

/**
 * \brief Remove an element from a list (not destroyed).
 * \param lst list root
 * \param itm element to remove
 * \return 0 on success.
 */
int sconf_list_remove(struct sconf *lst, struct sconf *itm);

/**
 * \brief Count list elements.
 * \param lst list root
 * \return number of elements.
 */
int sconf_list_size(const struct sconf *lst);

/**
 * \brief Retrive element at index.
 * \param lst list root
 * \param idx index
 * \return Object or NULL if out of range.
 */
struct sconf *sconf_list_at(const struct sconf *lst, int idx);

/**
 * \brief Return the first element of a list.
 * \param lst list root
 * \return Object or NULL on empty list.
 */
struct sconf *sconf_list_first(const struct sconf *lst);

/**
 * \brief Return the last element of a list.
 * \param lst list root
 * \return Object or NULL on empty list.
 */
struct sconf *sconf_list_last(const struct sconf *lst);

/**
 * \brief Check if list is empty.
 * \param lst list root
 * \return SCONF_TRUE if empty, SCONF_FALSE otherwhise.
 */
int sconf_list_empty(const struct sconf *lst);

/**
 * \brief Free an S-expression object.
 */
void sconf_destroy(struct sconf *sexp);

/**
 * \brief Get the last error code.
 */
enum sconf_error sconf_get_last_error(void);

/**
 * \brief Convert an error code to a readable string.
 */
const char *sconf_error_str(enum sconf_error err);

static inline int
sconf_is_bool(const struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_BOOL);
}

static inline int
sconf_is_true(const struct sconf *sexp)
{
	return (sconf_is_bool(sexp) && sexp->value.as_int == SCONF_TRUE);
}

static inline int
sconf_is_false(const struct sconf *sexp)
{
	return (!sconf_is_bool(sexp) /* if not bool default as false */
			|| sexp->value.as_int == SCONF_FALSE);
}

static inline int
sconf_is_int(const struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_INT);
}

static inline int
sconf_is_double(const struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_DOUBLE);
}

static inline int
sconf_is_string(const struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_STRING);
}

static inline int
sconf_is_symbol(const struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_SYMBOL);
}

static inline int
sconf_is_list(const struct sconf *sexp)
{
	return (sexp != NULL && sexp->type == SCONF_T_LIST);
}

static inline char *
sconf_get_string_value(const struct sconf *sexp)
{
	if (!sconf_is_string(sexp)) return (NULL);

	return (sexp->value.as_string);
}


static inline char *
sconf_get_symbol_value(const struct sconf *sexp)
{
	if (!sconf_is_symbol(sexp)) return (NULL);

	return (sexp->value.as_string);
}

# ifdef __cplusplus
}
# endif /* __cplusplus */

#endif /* !SCONF_H */
