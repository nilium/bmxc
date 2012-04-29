/*
	Copyright (c) 2010 Noel R. Cower

	This software is provided 'as-is', without any express or implied
	warranty. In no event will the authors be held liable for any damages
	arising from the use of this software.

	Permission is granted to anyone to use this software for any purpose,
	including commercial applications, and to alter it and redistribute it
	freely, subject to the following restrictions:

	1. The origin of this software must not be misrepresented; you must not
	claim that you wrote the original software. If you use this software
	in a product, an acknowledgment in the product documentation would be
	appreciated but is not required.

	2. Altered source versions must be plainly marked as such, and must not be
	misrepresented as being the original software.

	3. This notice may not be removed or altered from any source
	distribution.
*/

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>
#include <stdarg.h>

#include "lexer.h"

const size_t LEXER_INITIAL_CAPACITY = 500;

typedef struct s_token_mark {
	const char *place;
	size_t line, column;
	size_t token;
} token_mark_t;

struct s_lexer {
	size_t capacity;
	token_t *tokens;

	const char *source_begin, *source_end;
	token_mark_t current;

	char *error;
};

static int lexer_asprintf(char **ret, const char *format, ...);
static void lexer_tokens_fit(lexer_t *lexer, size_t n);
static token_t *lexer_new_token(lexer_t *lexer);
static token_t *lexer_merge_tokens(lexer_t *lexer, size_t from, size_t to, token_kind_t newKind);
static token_mark_t lexer_mark(lexer_t *lexer);
static void lexer_reset(lexer_t *lexer, token_mark_t mark);
static char lexer_current(lexer_t *lexer);
static bool lexer_has_next(lexer_t *lexer);
static char lexer_next(lexer_t *lexer);
static char lexer_peek(lexer_t *lexer);
static void lexer_skip_whitespace(lexer_t *lexer);
static token_t lexer_read_base_number(lexer_t *lexer);
static token_kind_t token_kind_for_single(const char *single, size_t len);
static token_t lexer_read_number(lexer_t *lexer);
token_t lexer_read_word(lexer_t *lexer);
static token_t lexer_read_string(lexer_t *lexer);
static token_t lexer_read_line_comment(lexer_t *lexer);

const char *g_token_names[] = {
	"TOK_INVALID",

	"TOK_ID",

	"TOK_END_KW",

	"TOK_FUNCTION_KW",
	"TOK_ENDFUNCTION_KW",

	"TOK_METHOD_KW",
	"TOK_ENDMETHOD_KW",

	"TOK_TYPE_KW",
	"TOK_EXTENDS_KW",
	"TOK_ABSTRACT_KW",
	"TOK_FINAL_KW",
	"TOK_NODEBUG_KW",
	"TOK_ENDTYPE_KW",

	"TOK_EXTERN_KW",
	"TOK_ENDEXTERN_KW",

	"TOK_REM_KW",
	"TOK_ENDREM_KW",

	"TOK_FLOAT_KW",
	"TOK_DOUBLE_KW",
	"TOK_BYTE_KW",
	"TOK_SHORT_KW",
	"TOK_INT_KW",
	"TOK_LONG_KW",
	"TOK_STRING_KW",
	"TOK_OBJECT_KW",

	"TOK_LOCAL_KW",
	"TOK_GLOBAL_KW",
	"TOK_CONST_KW",

	"TOK_VARPTR_KW",
	"TOK_PTR_KW",
	"TOK_VAR_KW",

	"TOK_NULL_KW",

	"TOK_STRICT_KW",
	"TOK_SUPERSTRICT_KW",

	"TOK_FRAMEWORK_KW",

	"TOK_MODULE_KW",
	"TOK_MODULEINFO_KW",

	"TOK_IMPORT_KW",
	"TOK_INCLUDE_KW",

	"TOK_PRIVATE_KW",
	"TOK_PUBLIC_KW",

	"TOK_OR_KW",
	"TOK_AND_KW",
	"TOK_SHR_KW",
	"TOK_SHL_KW",
	"TOK_SAR_KW",
	"TOK_MOD_KW",
	"TOK_NOT_KW",

	"TOK_WHILE_KW",
	"TOK_WEND_KW",
	"TOK_ENDWHILE_KW",

	"TOK_FOR_KW",
	"TOK_NEXT_KW",
	"TOK_UNTIL_KW",
	"TOK_TO_KW",
	"TOK_EACHIN_KW",

	"TOK_REPEAT_KW",
	"TOK_FOREVER_KW",

	"TOK_IF_KW",
	"TOK_ENDIF_KW",
	"TOK_ELSE_KW",
	"TOK_ELSEIF_KW",
	"TOK_THEN_KW",

	"TOK_SELECT_KW",
	"TOK_CASE_KW",
	"TOK_DEFAULT_KW",
	"TOK_ENDSELECT_KW",

	"TOK_SELF_KW",
	"TOK_SUPER_KW",
	"TOK_PI_KW",
	"TOK_NEW_KW",
	"TOK_DELETE_KW",

#if USE_BMX_EXTENSIONS
	// "extensions"
	"TOK_PROTOCOL_KW",
	"TOK_ENDPROTOCOL_KW",
	"TOK_AUTO_KW",
	"TOK_IMPLEMENTS_KW",
#endif

	"TOK_COLON",
	"TOK_QUESTION",
	"TOK_BANG",
	"TOK_HASH",
	"TOK_DOT",
	"TOK_DOUBLEDOT",
	"TOK_TRIPLEDOT",
	"TOK_AT",
	"TOK_DOUBLEAT",
	"TOK_DOLLAR",
	"TOK_PERCENT",
	"TOK_SINGLEQUOTE",
	"TOK_OPENPAREN",
	"TOK_CLOSEPAREN",
	"TOK_OPENBRACKET",
	"TOK_CLOSEBRACKET",
	"TOK_OPENCURL",
	"TOK_CLOSECURL",
	"TOK_GREATERTHAN",
	"TOK_LESSTHAN",
	"TOK_EQUALS",
	"TOK_MINUS",
	"TOK_PLUS",
	"TOK_ASTERISK",
	"TOK_CARET",
	"TOK_TILDE",
	"TOK_GRAVE",
	"TOK_BACKSLASH",
	"TOK_SLASH",
	"TOK_COMMA",
	"TOK_SEMICOLON",
	"TOK_PIPE",
	"TOK_AMPERSAND",
	"TOK_NEWLINE",

	"TOK_ASSIGN_ADD",
	"TOK_ASSIGN_SUBTRACT",
	"TOK_ASSIGN_DIVIDE",
	"TOK_ASSIGN_MULTIPLY",
	"TOK_ASSIGN_POWER",

	"TOK_ASSIGN_SHL",
	"TOK_ASSIGN_SHR",
	"TOK_ASSIGN_SAR",
	"TOK_ASSIGN_MOD",

	"TOK_ASSIGN_XOR",
	"TOK_ASSIGN_AND",
	"TOK_ASSIGN_OR",

	"TOK_ASSIGN_AUTO",
	"TOK_DOUBLEMINUS",
	"TOK_DOUBLEPLUS",

	"TOK_NUMBER_LIT",
	"TOK_HEX_LIT",
	"TOK_BIN_LIT",
	"TOK_STRING_LIT",

	"TOK_LINE_COMMENT",
	"TOK_BLOCK_COMMENT",

	"TOK_EOF",

	NULL
};

static const char *token_strings[] = {
	"INVALID",

	"Identifier",

	"End",

	"Function",
	"End Function",

	"Method",
	"End Method",

	"Type",
	"Extends",
	"Abstract",
	"Final",
	"No Debug",
	"End Type",

	"Extern",
	"End Extern",

	"Rem",
	"End Rem",

	"Float",
	"Double",
	"Byte",
	"Short",
	"Int",
	"Long",
	"String",
	"Object",

	"Local",
	"Global",
	"Const",

	"VarPtr",
	"Ptr",
	"Var",

	"Null",

	"Strict",
	"SuperStrict",

	"Framework",

	"Module",
	"ModuleInfo",

	"Import",
	"Include",

	"Private",
	"Public",

	"Or",
	"And",

	"Shr",
	"Shl",
	"Sar",
	"Mod",
	"Not",

	"While",
	"Wend",
	"End While",

	"For",
	"Next",
	"Until",
	"To",
	"EachIn",

	"Repeat",
	"Forever",

	"If",
	"End If",
	"Else",
	"Else If",
	"Then",

	"Select",
	"Case",
	"Default",
	"End Select",

	"Self",
	"Super",
	"Pi",
	"New",
	"Delete",

#if USE_BMX_EXTENSIONS
	"Protocol",
	"End Protocol",
	"Auto",
	"Implements",
#endif

	":",
	"?",
	"!",
	"#",
	".",
	"..",
	"...",
	"@",
	"@@",
	"$",
	"%",
	"'",
	"(",
	")",
	"[",
	"]",
	"{",
	"}",
	">",
	"<",
	"=",
	"-",
	"+",
	"*",
	"^",
	"~",
	"`",
	"\\",
	"/",
	",",
	";",
	"|",
	"&",
	"\\n",

	":+",
	":-",
	":/",
	":*",
	":^",

	":Shl",
	":Shr",
	":Sar",
	":Mod",

	":~",
	":&",
	":|",

#if USE_BMX_EXTENSIONS
	":=",
	"--",
	"++",
#endif

	"Number Literal",
	"Hex Literal",
	"Bin Literal",
	"String Literal",

	"Line Comment",
	"Block Comment",

	"<EOF>",

	NULL
};


typedef struct s_token_single {
	token_kind_t kind;
	const char *matches;
	bool case_sensitive;
} token_single_t;

static token_single_t const token_singles[] = {
	{ .kind = TOK_END_KW, .case_sensitive = false, .matches = "end" },

	{ .kind = TOK_FUNCTION_KW, .case_sensitive = false, .matches = "function" },
	{ .kind = TOK_ENDFUNCTION_KW, .case_sensitive = false, .matches = "endfunction" },

	{ .kind = TOK_METHOD_KW, .case_sensitive = false, .matches = "method" },
	{ .kind = TOK_ENDMETHOD_KW, .case_sensitive = false, .matches = "endmethod" },

	{ .kind = TOK_TYPE_KW, .case_sensitive = false, .matches = "type" },
	{ .kind = TOK_EXTENDS_KW, .case_sensitive = false, .matches = "extends" },
	{ .kind = TOK_ABSTRACT_KW, .case_sensitive = false, .matches = "abstract" },
	{ .kind = TOK_FINAL_KW, .case_sensitive = false, .matches = "final" },
	{ .kind = TOK_NODEBUG_KW, .case_sensitive = false, .matches = "nodebug" },
	{ .kind = TOK_ENDTYPE_KW, .case_sensitive = false, .matches = "endtype" },

	{ .kind = TOK_EXTERN_KW, .case_sensitive = false, .matches = "extern" },
	{ .kind = TOK_ENDEXTERN_KW, .case_sensitive = false, .matches = "endextern" },

	{ .kind = TOK_REM_KW, .case_sensitive = false, .matches = "rem" },
	{ .kind = TOK_ENDREM_KW, .case_sensitive = false, .matches = "endrem" },

	{ .kind = TOK_FLOAT_KW, .case_sensitive = false, .matches = "float" },
	{ .kind = TOK_DOUBLE_KW, .case_sensitive = false, .matches = "double" },
	{ .kind = TOK_BYTE_KW, .case_sensitive = false, .matches = "byte" },
	{ .kind = TOK_SHORT_KW, .case_sensitive = false, .matches = "short" },
	{ .kind = TOK_INT_KW, .case_sensitive = false, .matches = "int" },
	{ .kind = TOK_LONG_KW, .case_sensitive = false, .matches = "long" },
	{ .kind = TOK_STRING_KW, .case_sensitive = false, .matches = "string" },
	{ .kind = TOK_OBJECT_KW, .case_sensitive = false, .matches = "object" },

	{ .kind = TOK_LOCAL_KW, .case_sensitive = false, .matches = "local" },
	{ .kind = TOK_GLOBAL_KW, .case_sensitive = false, .matches = "global" },
	{ .kind = TOK_CONST_KW, .case_sensitive = false, .matches = "const" },

	{ .kind = TOK_VARPTR_KW, .case_sensitive = false, .matches = "varptr" },
	{ .kind = TOK_PTR_KW, .case_sensitive = false, .matches = "ptr" },
	{ .kind = TOK_VAR_KW, .case_sensitive = false, .matches = "var" },

	{ .kind = TOK_NULL_KW, .case_sensitive = false, .matches = "null" },

	{ .kind = TOK_STRICT_KW, .case_sensitive = false, .matches = "strict" },
	{ .kind = TOK_SUPERSTRICT_KW, .case_sensitive = false, .matches = "superstrict" },

	{ .kind = TOK_FRAMEWORK_KW, .case_sensitive = false, .matches = "framework" },

	{ .kind = TOK_MODULE_KW, .case_sensitive = false, .matches = "module" },
	{ .kind = TOK_MODULEINFO_KW, .case_sensitive = false, .matches = "moduleinfo" },

	{ .kind = TOK_IMPORT_KW, .case_sensitive = false, .matches = "import" },
	{ .kind = TOK_INCLUDE_KW, .case_sensitive = false, .matches = "include" },

	{ .kind = TOK_PRIVATE_KW, .case_sensitive = false, .matches = "private" },
	{ .kind = TOK_PUBLIC_KW, .case_sensitive = false, .matches = "public" },

	{ .kind = TOK_OR_KW, .case_sensitive = false, .matches = "or" },
	{ .kind = TOK_AND_KW, .case_sensitive = false, .matches = "and" },
	{ .kind = TOK_SHR_KW, .case_sensitive = false, .matches = "shr" },
	{ .kind = TOK_SHL_KW, .case_sensitive = false, .matches = "shl" },
	{ .kind = TOK_SAR_KW, .case_sensitive = false, .matches = "sar" },
	{ .kind = TOK_MOD_KW, .case_sensitive = false, .matches = "mod" },
	{ .kind = TOK_NOT_KW, .case_sensitive = false, .matches = "not" },

	{ .kind = TOK_WHILE_KW, .case_sensitive = false, .matches = "while" },
	{ .kind = TOK_WEND_KW, .case_sensitive = false, .matches = "wend" },
	{ .kind = TOK_ENDWHILE_KW, .case_sensitive = false, .matches = "endwhile" },

	{ .kind = TOK_FOR_KW, .case_sensitive = false, .matches = "for" },
	{ .kind = TOK_NEXT_KW, .case_sensitive = false, .matches = "next" },
	{ .kind = TOK_UNTIL_KW, .case_sensitive = false, .matches = "until" },
	{ .kind = TOK_TO_KW, .case_sensitive = false, .matches = "to" },
	{ .kind = TOK_EACHIN_KW, .case_sensitive = false, .matches = "eachin" },

	{ .kind = TOK_REPEAT_KW, .case_sensitive = false, .matches = "repeat" },
	{ .kind = TOK_FOREVER_KW, .case_sensitive = false, .matches = "forever" },

	{ .kind = TOK_IF_KW, .case_sensitive = false, .matches = "if" },
	{ .kind = TOK_ENDIF_KW, .case_sensitive = false, .matches = "endif" },
	{ .kind = TOK_ELSE_KW, .case_sensitive = false, .matches = "else" },
	{ .kind = TOK_ELSEIF_KW, .case_sensitive = false, .matches = "elseif" },
	{ .kind = TOK_THEN_KW, .case_sensitive = false, .matches = "then" },

	{ .kind = TOK_SELECT_KW, .case_sensitive = false, .matches = "select" },
	{ .kind = TOK_CASE_KW, .case_sensitive = false, .matches = "case" },
	{ .kind = TOK_DEFAULT_KW, .case_sensitive = false, .matches = "default" },
	{ .kind = TOK_ENDSELECT_KW, .case_sensitive = false, .matches = "endselect" },

	{ .kind = TOK_SELF_KW, .case_sensitive = false, .matches = "self" },
	{ .kind = TOK_SUPER_KW, .case_sensitive = false, .matches = "super" },
	// Should this really be a keyword?
	{ .kind = TOK_PI_KW, .case_sensitive = false, .matches = "pi" },
	{ .kind = TOK_NEW_KW, .case_sensitive = false, .matches = "new" },
	{ .kind = TOK_DELETE_KW, .case_sensitive = false, .matches = "delete" },

#if USE_BMX_EXTENSIONS

	{ .kind = TOK_PROTOCOL_KW, .case_sensitive = false, .matches = "protocol" },
	{ .kind = TOK_ENDPROTOCOL_KW, .case_sensitive = false, .matches = "endprotocol" },
	{ .kind = TOK_AUTO_KW, .case_sensitive = false, .matches = "auto" },
	{ .kind = TOK_IMPLEMENTS_KW, .case_sensitive = false, .matches = "implements" },

#endif

	{ .kind = TOK_COLON, .case_sensitive = false, .matches = ":"  },
	{ .kind = TOK_QUESTION, .case_sensitive = false, .matches = "?"	 },
	{ .kind = TOK_BANG, .case_sensitive = false, .matches = "!"	 },
	{ .kind = TOK_HASH, .case_sensitive = false, .matches = "#"	 },
	{ .kind = TOK_DOLLAR, .case_sensitive = false, .matches = "$"  },
	{ .kind = TOK_PERCENT, .case_sensitive = false, .matches = "%"	},
	{ .kind = TOK_OPENPAREN, .case_sensitive = false, .matches = "("  },
	{ .kind = TOK_CLOSEPAREN, .case_sensitive = false, .matches = ")"  },
	{ .kind = TOK_OPENBRACKET, .case_sensitive = false, .matches = "["	},
	{ .kind = TOK_CLOSEBRACKET, .case_sensitive = false, .matches = "]"	 },
	{ .kind = TOK_OPENCURL, .case_sensitive = false, .matches = "{"	 },
	{ .kind = TOK_CLOSECURL, .case_sensitive = false, .matches = "}"  },
	{ .kind = TOK_GREATERTHAN, .case_sensitive = false, .matches = ">"	},
	{ .kind = TOK_LESSTHAN, .case_sensitive = false, .matches = "<"	 },
	{ .kind = TOK_EQUALS, .case_sensitive = false, .matches = "="  },
	{ .kind = TOK_MINUS, .case_sensitive = false, .matches = "-"  },
	{ .kind = TOK_PLUS, .case_sensitive = false, .matches = "+"	 },
	{ .kind = TOK_ASTERISK, .case_sensitive = false, .matches = "*"	 },
	{ .kind = TOK_CARET, .case_sensitive = false, .matches = "^"  },
	{ .kind = TOK_TILDE, .case_sensitive = false, .matches = "~"  },
	{ .kind = TOK_GRAVE, .case_sensitive = false, .matches = "`"  },
	{ .kind = TOK_BACKSLASH, .case_sensitive = false, .matches = "\\"  },
	{ .kind = TOK_SLASH, .case_sensitive = false, .matches = "/"  },
	{ .kind = TOK_COMMA, .case_sensitive = false, .matches = ","  },
	{ .kind = TOK_SEMICOLON, .case_sensitive = false, .matches = ";"  },
	{ .kind = TOK_PIPE, .case_sensitive = false, .matches = "|"	 },
	{ .kind = TOK_AMPERSAND, .case_sensitive = false, .matches = "&"  },
	{ .kind = TOK_NEWLINE, .case_sensitive = false, .matches = "\n"	 },

	{ .kind = TOK_INVALID, .case_sensitive = false, .matches = NULL },
};


typedef struct s_token_pair {
	token_kind_t left, right;
	token_kind_t kind;
	size_t range;
} token_pair_t;

static token_pair_t const token_pairs[] = {
	{ .left = TOK_END_KW, .right = TOK_REM_KW, .kind = TOK_ENDREM_KW, .range = 1 },

	{ .left = TOK_END_KW, .right = TOK_METHOD_KW, .kind = TOK_ENDMETHOD_KW, .range = 1 },
	{ .left = TOK_END_KW, .right = TOK_FUNCTION_KW, .kind = TOK_ENDFUNCTION_KW, .range = 1 },
	{ .left = TOK_END_KW, .right = TOK_TYPE_KW, .kind = TOK_ENDTYPE_KW, .range = 1 },

	{ .left = TOK_END_KW, .right = TOK_EXTERN_KW, .kind = TOK_ENDEXTERN_KW, .range = 1 },

	{ .left = TOK_END_KW, .right = TOK_IF_KW, .kind = TOK_ENDIF_KW, .range = 1 },
	{ .left = TOK_END_KW, .right = TOK_SELECT_KW, .kind = TOK_ENDSELECT_KW, .range = 1 },
	{ .left = TOK_END_KW, .right = TOK_WHILE_KW, .kind = TOK_ENDWHILE_KW, .range = 1 },

#if USE_BMX_EXTENSIONS

	{ .left = TOK_END_KW, .right = TOK_PROTOCOL_KW, .kind = TOK_ENDPROTOCOL_KW, .range = 1 },

#endif

	{ .left = TOK_COLON, .right = TOK_PLUS, .kind = TOK_ASSIGN_ADD, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_MINUS, .kind = TOK_ASSIGN_SUBTRACT, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_SLASH, .kind = TOK_ASSIGN_DIVIDE, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_ASTERISK, .kind = TOK_ASSIGN_MULTIPLY, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_CARET, .kind = TOK_ASSIGN_POWER, .range = 0 },

	{ .left = TOK_COLON, .right = TOK_SHL_KW, .kind = TOK_ASSIGN_SHL, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_SHR_KW, .kind = TOK_ASSIGN_SHR, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_SAR_KW, .kind = TOK_ASSIGN_SAR, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_MOD_KW, .kind = TOK_ASSIGN_MOD, .range = 0 },

	{ .left = TOK_COLON, .right = TOK_TILDE, .kind = TOK_ASSIGN_XOR, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_AMPERSAND, .kind = TOK_ASSIGN_AND, .range = 0 },
	{ .left = TOK_COLON, .right = TOK_PIPE, .kind = TOK_ASSIGN_OR, .range = 0 },

#if USE_BMX_EXTENSIONS

	{ .left = TOK_COLON, .right = TOK_EQUALS, .kind = TOK_ASSIGN_AUTO, .range = 0 },
	{ .left = TOK_MINUS, .right = TOK_MINUS, .kind = TOK_DOUBLEMINUS, .range = 0 },
	{ .left = TOK_PLUS, .right = TOK_PLUS, .kind = TOK_DOUBLEPLUS, .range = 0 },

#endif

	//	{ .left = TOK_MINUS, .right = TOK_NUMBER_LIT, .kind = TOK_NUMBER_LIT, .range = 0 },
	{ .left = TOK_INVALID, .right = TOK_INVALID, .kind = TOK_INVALID, .range = -1 },
};


char *token_to_string(const token_t *tok) {
	const char *orig;
	char *buf = NULL;
	size_t len;
	if (tok == NULL) {
		orig = token_strings[TOK_INVALID];
		len = strlen(orig);
	} else if (tok->from != NULL && tok->to != NULL && tok->kind != TOK_EOF && tok->kind != TOK_INVALID && tok->kind != TOK_NEWLINE) {
		orig = tok->from;
		len = (size_t)(tok->to-tok->from);
	} else {
		orig = token_strings[tok->kind];
		if (orig != NULL) {
			len = strlen(orig);
		}
	}
	if (orig != NULL) {
		buf = (char*)calloc(len+1, sizeof(char));
		strncpy(buf, orig, len);
	}
	return buf;
}


lexer_t *lexer_new(const char *source_begin, const char *source_end) {
	if (source_begin == NULL || source_end == NULL || source_begin > source_end) {
		return NULL;
	}

	lexer_t *lexer = (lexer_t *)malloc(sizeof(lexer_t));

	lexer->capacity = 0;
	lexer->tokens = NULL;
	lexer->source_begin = source_begin;
	lexer->source_end = source_end;
	lexer->current.place = source_begin;
	lexer->current.line = 1;
	lexer->current.column = 1;
	lexer->current.token = 0;
	lexer->error = NULL;
	lexer_tokens_fit(lexer, LEXER_INITIAL_CAPACITY);

	return lexer;
}


void lexer_destroy(lexer_t *lexer) {
	if (lexer == NULL) {
		return;
	}

	if (lexer->tokens != NULL) {
		free(lexer->tokens);
		lexer->tokens = NULL;
	}
	if (lexer->error != NULL) {
		free(lexer->error);
		lexer->error = NULL;
	}
	lexer->source_begin = NULL;
	lexer->source_end = NULL;
	lexer->current.place = NULL;

	free(lexer);
}


static void lexer_tokens_fit(lexer_t *lexer, size_t n) {
	if (n < lexer->capacity) {
		return;
	}

	size_t sz = lexer->capacity*2;
	if (sz < n) {
		sz = n;
	}
	lexer->tokens = realloc(lexer->tokens, sz*sizeof(token_t));
	lexer->capacity = sz;
}


static token_t *lexer_new_token(lexer_t *lexer) {
	size_t index = lexer->current.token + 1;
	lexer_tokens_fit(lexer, index+1);
	token_t *token = lexer->tokens+lexer->current.token;
	lexer->current.token = index;
	token->kind = TOK_INVALID;
	token->from = token->to = NULL;
	token->line = 0;
	token->column = 0;
	return token;
}


static token_t *lexer_merge_tokens(lexer_t *lexer, size_t from, size_t to, token_kind_t newKind) {
	lexer->tokens[from].to = lexer->tokens[to].to;
	lexer->tokens[from].kind = newKind;
	int offset = from - to;
	size_t idx = to+1;
	for (; idx < lexer->current.token; ++idx)
		lexer->tokens[idx+offset] = lexer->tokens[idx];
	lexer->current.token += offset;

	return NULL;
}


static token_mark_t lexer_mark(lexer_t *lexer) {
	return lexer->current;
}


static void lexer_reset(lexer_t *lexer, token_mark_t mark) {
	lexer->current = mark;
}


static char lexer_current(lexer_t *lexer) {
	if (lexer->source_end < lexer->current.place)
		return 0;
	return *(lexer->current.place);
}


static bool lexer_has_next(lexer_t *lexer) {
	return (bool)((lexer->current.place) < lexer->source_end);
}

/*
note: when parsing, if you have a loop that calls lexer_next, and you want that
loop to consume the last character parsed, call lexer_next once again after the
loop completes - if the final character shouldn't be consumed, of course, don't
*/
static char lexer_next(lexer_t *lexer) {
	if (lexer_current(lexer) == '\n') {
		lexer->current.line += 1;
		lexer->current.column = 1;
	} else {
		++lexer->current.column;
	}
	return lexer_has_next(lexer) ? *(++lexer->current.place) : 0;
}


static char lexer_peek(lexer_t *lexer) {
	return lexer_has_next(lexer) ? *(lexer->current.place+1) : 0;
}


static void lexer_skip_whitespace(lexer_t *lexer) {
	char cur;
	while ((cur = lexer_current(lexer)) != 0 &&
		   (cur == ' ' || cur == '\t' || cur == '\r')) {
		lexer_next(lexer);
	}
}


static token_t lexer_read_base_number(lexer_t *lexer) {
	char cur = lexer_current(lexer);
	token_mark_t mark = lexer_mark(lexer);
	token_t token = {
		.kind = TOK_NUMBER_LIT,
		.line = mark.line,
		.column = mark.column,
		.from = mark.place,
		.to = NULL,
	};

	if (cur == '%') {	// bin
		while (lexer_has_next(lexer) && ((cur = lexer_next(lexer)) == '0' || cur == '1'));
	} else if (cur == '$') {	// hex
		while (lexer_has_next(lexer) && isxdigit(lexer_next(lexer)));
	} else {
		lexer_asprintf(&lexer->error, "[%d:%d] Malformed number literal encountered, not a number\n",
				lexer->current.line, lexer->current.column);
		token.kind = TOK_INVALID;
		return token;
	}

	lexer_next(lexer);
	token.to = lexer->current.place;

	return token;
}


static token_kind_t token_kind_for_single(const char *single, size_t len) {
	const token_single_t* iter = token_singles;
	while (iter->kind != TOK_INVALID) {
		if (strlen(iter->matches) == len && ((iter->case_sensitive
			 ? strncmp(iter->matches, single, len)
			 : strncasecmp(iter->matches, single, len)) == 0)) {
				break;
			}
		++iter;
	}
	return iter->kind;
}


static token_t lexer_read_number(lexer_t *lexer) {
	char cur = lexer_current(lexer);
	token_mark_t mark = lexer_mark(lexer);
	bool isDec = (cur == '.');
	bool isExp = false;
	token_t token = {
		.kind = TOK_NUMBER_LIT,
		.line = mark.line,
		.column = mark.column,
		.from = mark.place,
		.to = NULL,
	};

	while (lexer_has_next(lexer) && (cur = lexer_next(lexer)) != 0) {		   
		if (cur == '.') {
			if (isDec) {
				break;
			}
			isDec = true;
			continue;
		}

		if (isdigit(cur)) {
			continue;
		}

		if (tolower(cur) == 'e') {
			if (isExp) {
				lexer_asprintf(&lexer->error, "[%d:%d] Malformed number literal encountered, exponent already provided\n",
						lexer->current.line, lexer->current.column);
				token.kind = TOK_INVALID;
				return token;
			}

			isExp = true;
			cur = lexer_peek(lexer);
			if (cur == '-' || cur == '+') {
				lexer_next(lexer);
				cur = lexer_peek(lexer);
			}
			if (!isdigit(cur)) {
				lexer_asprintf(&lexer->error, "[%d:%d] Malformed number literal encountered, exponent expected but not found (%c:%d)\n",
						lexer->current.line, lexer->current.column, cur, cur);
				token.kind = TOK_INVALID;
				return token;
			}
			continue;
		}

		break;
	}

	token.to = lexer->current.place;

	return token;
}


token_t lexer_read_word(lexer_t *lexer) {
	token_mark_t mark = lexer_mark(lexer);
	token_t token = {
		.kind = TOK_ID,
		.line = mark.line,
		.column = mark.column,
		.from = mark.place,
		.to = NULL,
	};

	while (lexer_has_next(lexer)) {
		char cur = lexer_peek(lexer);
		if (cur != '_' && !isalnum(cur)) {
			break;
		}
		lexer_next(lexer);
	}

	lexer_next(lexer);
	token.to = lexer->current.place;

	token_kind_t alter = token_kind_for_single(token.from, (size_t)(token.to-token.from));
	if (alter != TOK_INVALID) {
		token.kind = alter;
	}

	return token;
}


static token_t lexer_read_string(lexer_t *lexer) {
	char cur = lexer_current(lexer);
	token_mark_t mark = lexer_mark(lexer);
	token_t token = {
		.kind = TOK_STRING_LIT,
		.line = mark.line,
		.column = mark.column,
		.from = mark.place,
		.to = NULL,
	};

	while (lexer_has_next(lexer) && (cur = lexer_next(lexer)) != '"') {
		if (cur == '\n') {
			lexer_asprintf(&lexer->error, "[%d:%d] String literal does not terminate before newline or EOF\n",
					lexer->current.line, lexer->current.column);
			token.kind = TOK_INVALID;
			return token;
		}
	}
	lexer_next(lexer);
	token.to = lexer->current.place;

	return token;
}


static token_t lexer_read_line_comment(lexer_t *lexer) {
	char cur = lexer_current(lexer);
	token_mark_t mark = lexer_mark(lexer);
	token_t token = {
		.kind = TOK_LINE_COMMENT,
		.line = mark.line,
		.column = mark.column,
		.from = mark.place,
		.to = NULL,
	};

	do {
		cur = lexer_next(lexer);
	} while(cur != 0 && cur != '\n');

	token.to = lexer->current.place;

	return token;
}


int lexer_run(lexer_t *lexer) {
	if (lexer == NULL || lexer->error != NULL) {
		return 1;
	}

	token_mark_t mark;
	token_t comment = {.kind=TOK_INVALID};
	token_t token;
	char cur;

	while(lexer_current(lexer) != 0) {
		token.kind = TOK_INVALID;
		lexer_skip_whitespace(lexer);

		mark = lexer_mark(lexer);
		if ((cur = lexer_current(lexer)) == 0) {
			break;
		}

		if (comment.kind == TOK_INVALID) {

			if (cur == '@') {
				token.kind = TOK_AT;
				if (lexer_next(lexer) == '@') {
					token.kind = TOK_DOUBLEAT;
					lexer_next(lexer);
				}

				token.from = mark.place;
				token.to = lexer->current.place;
				token.line = mark.line;
				token.column = mark.column;
			}

			if (cur == '.') {
				if (isdigit(lexer_peek(lexer))) {
					token = lexer_read_number(lexer);
				} else {
					token.kind = TOK_DOT;

#if USE_BMX_EXTENSIONS
					while(token.kind <= TOK_TRIPLEDOT && lexer_next(lexer) == '.') {
						++token.kind;
					}
#else
					while(token.kind <= TOK_DOUBLEDOT && lexer_next(lexer) == '.') {
						++token.kind;
					}
#endif
					token.from = mark.place;
					token.to = lexer->current.place;
					token.line = mark.line;
					token.column = mark.column;
				}
			}

			if (cur == '\'') {
				token = lexer_read_line_comment(lexer);
			}

			if (cur == '%') {
				char peek = lexer_peek(lexer);
				if (peek == '1' || peek == '0') {
					token = lexer_read_base_number(lexer);
				}
			}

			if (cur == '$' && isxdigit(lexer_peek(lexer))) {
				token = lexer_read_base_number(lexer);
			}

			if (cur == '"') {
				token = lexer_read_string(lexer);
			}

			if (isdigit(cur)) {
				token = lexer_read_number(lexer);
			}

			if (token.kind == TOK_INVALID) {
				token_kind_t alter = token_kind_for_single(&cur, 1);
				if (alter != TOK_INVALID) {
					token.kind = alter;
					token.from = mark.place;
					token.line = mark.line;
					token.column = mark.column;
					lexer_next(lexer);
					token.to = lexer->current.place;
				}
			}
		}

		if (cur == '_' || isalpha(cur)) {
			token = lexer_read_word(lexer);
		}

		if (comment.kind != TOK_INVALID) {
			if (token.kind == TOK_END_KW) {
				if (lexer_current(lexer) == ' ') {
					lexer_next(lexer);
				}

				if ((cur = lexer_current(lexer)) == '_' || isalpha(cur)) {
					token_mark_t next_mark = lexer_mark(lexer);
					token_t next = lexer_read_word(lexer);
					if (next.kind == TOK_REM_KW) {
						token.kind = TOK_ENDREM_KW;
						token.to = next.to;
					} else {
						lexer_reset(lexer, next_mark);
					}
				}
			}

			if (token.kind == TOK_ENDREM_KW) {
				token_t block = {
					.kind = TOK_BLOCK_COMMENT,
					.line = comment.line,
					.column = comment.column,
					.from = comment.to + 1,
					.to = token.from - 1,
				};
				comment.kind = TOK_INVALID;
				*lexer_new_token(lexer) = block;
			}

			if (token.kind == TOK_INVALID) {
				lexer_next(lexer);
				lexer_skip_whitespace(lexer);
			}
		}

		if (token.kind != TOK_INVALID && comment.kind == TOK_INVALID) {
			*lexer_new_token(lexer) = token;
		}

		if (comment.kind == TOK_INVALID && token.kind == TOK_REM_KW) {
			comment = token;
		}

		if (comment.kind == TOK_INVALID && token.kind == TOK_INVALID && lexer->error == NULL) {
			lexer_asprintf(&lexer->error, "[%d:%d] Invalid token: %c:%hhu\n",
					lexer->current.line, lexer->current.column, cur, cur);
		}

		if (lexer->error != NULL) {
			return 1;
		}
	}

	lexer_new_token(lexer)->kind = TOK_EOF;

	size_t tok_index = 0;
	while (lexer->tokens[tok_index].kind != TOK_EOF) {
		token_t left, right;
		bool merged = false;
		left = lexer->tokens[tok_index];
		right = lexer->tokens[tok_index+1];

		const token_pair_t *pair_iter = token_pairs;
		while (pair_iter->left != TOK_INVALID && !merged) {
			if (pair_iter->left == left.kind && pair_iter->right == right.kind &&
				right.from <= left.to+pair_iter->range) {
				lexer_merge_tokens(lexer, tok_index, tok_index+1, pair_iter->kind);
				merged = true;
			}
			++pair_iter;
		}

		if (!merged)
			++tok_index;
	}

	return 0;
}

token_t *lexer_copy_tokens(lexer_t *lexer, size_t *num_tokens) {
	if (lexer == NULL || num_tokens == NULL)
		return NULL;

	int num = lexer->current.token;
	token_t *tokens = (token_t*)calloc(lexer->current.token, sizeof(token_t));
	memcpy(tokens, lexer->tokens, sizeof(token_t)*num);
	*num_tokens = num;
	return tokens;
}

int lexer_get_num_tokens(lexer_t *lexer) {
	return lexer->current.token;
}

token_kind_t lexer_get_token(lexer_t *lexer, size_t index, token_t *token) {
	if (lexer == NULL) {
		return TOK_INVALID;
	}

	if (lexer->current.token <= index) {
		if (token != NULL) {
			token->kind = TOK_INVALID;
		}
		return TOK_INVALID;
	}
	if (token != NULL) {
		*token = lexer->tokens[index];
	}
	return token->kind;
}

const char *lexer_get_error(lexer_t *lexer) {
	return (const char*)(lexer != NULL ? lexer->error : NULL);
}

static int lexer_asprintf(char **output, const char *format, ...) {
	va_list args;
	va_start(args, format);
	size_t len = vsnprintf(NULL, 0, format, args)+1;
	va_end(args);
	char *buf = (char*)calloc(len, sizeof(char));
	if (buf==NULL) {
		*output = NULL;
		return -1;
	}
	va_start(args, format);
	vsnprintf(buf, len, format, args);
	va_end(args);
	*output = buf;
	return len-1;
}
