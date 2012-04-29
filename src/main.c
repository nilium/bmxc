#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "lexer.h"

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
	"TOK_FIELD_KW",

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

#ifdef BMAX_USE_ADDITIONS
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

#ifdef BMAX_USE_ADDITIONS
	"TOK_ASSIGN_LSHIFT",
	"TOK_ASSIGN_RSHIFT",

	"TOK_LSHIFT",
	"TOK_RSHIFT",

	"TOK_ASSIGN_AUTO",
	"TOK_DOUBLEMINUS",
	"TOK_DOUBLEPLUS",
#endif

	"TOK_NUMBER_LIT",
	"TOK_HEX_LIT",
	"TOK_BIN_LIT",
	"TOK_STRING_LIT",

	"TOK_LINE_COMMENT",
	"TOK_BLOCK_COMMENT",

	"TOK_EOF",

	NULL
};

int main(int argc, char const *argv[])
{
	for (--argc, ++argv; 0 < argc; --argc, ++argv) {
		lexer_t *lexer = NULL;
		char *buffer = NULL;
		FILE *fp = NULL;
		long length = -1;
		long readlen = 0;

		if (!(*argv)) {
			fprintf(stderr, "NULL argument to bmxlex\n");
			return 1;
		}

		fp = fopen(*argv, "rb");
		if (fp == NULL) {
			fprintf(stderr, "Failed to open file %s\n", *argv);
			return 1;
		}

		if (fseek(fp, 0, SEEK_END)) {
			fprintf(stderr, "Failed to seek to the end of %s\n", *argv);
			goto ERROR_CLOSE_FILE;
		}

		length = ftell(fp);
		if (length == -1) {
			fprintf(stderr, "Failed to get the length of %s\n", *argv);
			goto ERROR_CLOSE_FILE;
		}

		if (fseek(fp, 0, SEEK_SET)) {
			fprintf(stderr, "Failed to seek to the start of %s\n", *argv);
			goto ERROR_CLOSE_FILE;
		}

		buffer = (char *)malloc(length);
		if (buffer == NULL) {
			fprintf(stderr, "Failed to allocate memory for source buffer for %s\n", *argv);
			goto ERROR_CLOSE_FILE;
		}

		if ((readlen = fread(buffer, 1, length, fp)) != length) {
			fprintf(stderr, "Failed to read file %s to source buffer (read %ld)\n", *argv, readlen);
			goto ERROR_CLOSE_FREE;
		}

		fclose(fp);

		lexer = lexer_new(buffer, buffer + length);
		int err = lexer_run(lexer);
		if (err) {
			fprintf(stderr, "Failed to lex %s\nError: %s\n", *argv, lexer_get_error(lexer));
			// free(buffer);
			// return 1;
		}

		{
			size_t num_tokens = lexer_get_num_tokens(lexer);
			size_t index = 0;
			for (; index < num_tokens; ++index) {
				token_t token;
				char *str = NULL;
				char *trapped = NULL;
				size_t len = 0;
				size_t trapped_len = 0;
				size_t idx = 0;
				size_t trapped_idx = 0;

				lexer_get_token(lexer, index, &token);

				if (token.kind == TOK_INVALID) {
					free(buffer);
					fprintf(stderr, "Invalid token encountered at %d:%d in %s\n",
						token.line, token.column, *argv);
					return 1;
				}

				str = token_to_string(&token);
				len = strlen(str);
				for (; idx < len; ++idx)
					trapped_len += ((str[idx] == '\n') ? 2 : 1);
				trapped = calloc(trapped_len + 1, 1);
				for (idx = 0; idx < len; ++idx, ++trapped_idx) {
					if (str[idx] == '\n') {
						trapped[trapped_idx++] = '\\';
						trapped[trapped_idx] = 'n';
					} else {
						trapped[trapped_idx] = str[idx];
					}
				}

				fprintf(stdout, "\"%s\"[%d:%d]:[%d:%s]:%s\n", *argv, token.line, token.column, token.kind, g_token_names[token.kind], trapped);

				free(str);
				free(trapped);
			}
		}

		lexer_destroy(lexer);
		free(buffer);

		continue;

		// error gotos
		ERROR_CLOSE_FREE:
		free(buffer);
		ERROR_CLOSE_FILE:
		fclose(fp);
		return 1;
	}

	return 0;
}