module Tokens
	TOK_INVALID=:TOK_INVALID

	TOK_ID=:TOK_ID

	TOK_END_KW=:TOK_END_KW

	TOK_FUNCTION_KW=:TOK_FUNCTION_KW
	TOK_ENDFUNCTION_KW=:TOK_ENDFUNCTION_KW

	TOK_METHOD_KW=:TOK_METHOD_KW
	TOK_ENDMETHOD_KW=:TOK_ENDMETHOD_KW

	TOK_TYPE_KW=:TOK_TYPE_KW
	TOK_EXTENDS_KW=:TOK_EXTENDS_KW
	TOK_ABSTRACT_KW=:TOK_ABSTRACT_KW
	TOK_FINAL_KW=:TOK_FINAL_KW
	TOK_NODEBUG_KW=:TOK_NODEBUG_KW
	TOK_ENDTYPE_KW=:TOK_ENDTYPE_KW

	TOK_EXTERN_KW=:TOK_EXTERN_KW
	TOK_ENDEXTERN_KW=:TOK_ENDEXTERN_KW

	TOK_REM_KW=:TOK_REM_KW
	TOK_ENDREM_KW=:TOK_ENDREM_KW

	TOK_FLOAT_KW=:TOK_FLOAT_KW
	TOK_DOUBLE_KW=:TOK_DOUBLE_KW
	TOK_BYTE_KW=:TOK_BYTE_KW
	TOK_SHORT_KW=:TOK_SHORT_KW
	TOK_INT_KW=:TOK_INT_KW
	TOK_LONG_KW=:TOK_LONG_KW
	TOK_STRING_KW=:TOK_STRING_KW
	TOK_OBJECT_KW=:TOK_OBJECT_KW

	TOK_LOCAL_KW=:TOK_LOCAL_KW
	TOK_GLOBAL_KW=:TOK_GLOBAL_KW
	TOK_CONST_KW=:TOK_CONST_KW

	TOK_VARPTR_KW=:TOK_VARPTR_KW
	TOK_PTR_KW=:TOK_PTR_KW
	TOK_VAR_KW=:TOK_VAR_KW

	TOK_NULL_KW=:TOK_NULL_KW

	TOK_STRICT_KW=:TOK_STRICT_KW
	TOK_SUPERSTRICT_KW=:TOK_SUPERSTRICT_KW

	TOK_FRAMEWORK_KW=:TOK_FRAMEWORK_KW

	TOK_MODULE_KW=:TOK_MODULE_KW
	TOK_MODULEINFO_KW=:TOK_MODULEINFO_KW

	TOK_IMPORT_KW=:TOK_IMPORT_KW
	TOK_INCLUDE_KW=:TOK_INCLUDE_KW

	TOK_PRIVATE_KW=:TOK_PRIVATE_KW
	TOK_PUBLIC_KW=:TOK_PUBLIC_KW

	TOK_OR_KW=:TOK_OR_KW
	TOK_AND_KW=:TOK_AND_KW
	TOK_SHR_KW=:TOK_SHR_KW
	TOK_SHL_KW=:TOK_SHL_KW
	TOK_SAR_KW=:TOK_SAR_KW
	TOK_MOD_KW=:TOK_MOD_KW
	TOK_NOT_KW=:TOK_NOT_KW

	TOK_WHILE_KW=:TOK_WHILE_KW
	TOK_WEND_KW=:TOK_WEND_KW
	TOK_ENDWHILE_KW=:TOK_ENDWHILE_KW

	TOK_FOR_KW=:TOK_FOR_KW
	TOK_NEXT_KW=:TOK_NEXT_KW
	TOK_UNTIL_KW=:TOK_UNTIL_KW
	TOK_TO_KW=:TOK_TO_KW
	TOK_EACHIN_KW=:TOK_EACHIN_KW

	TOK_REPEAT_KW=:TOK_REPEAT_KW
	TOK_FOREVER_KW=:TOK_FOREVER_KW

	TOK_IF_KW=:TOK_IF_KW
	TOK_ENDIF_KW=:TOK_ENDIF_KW
	TOK_ELSE_KW=:TOK_ELSE_KW
	TOK_ELSEIF_KW=:TOK_ELSEIF_KW
	TOK_THEN_KW=:TOK_THEN_KW

	TOK_SELECT_KW=:TOK_SELECT_KW
	TOK_CASE_KW=:TOK_CASE_KW
	TOK_DEFAULT_KW=:TOK_DEFAULT_KW
	TOK_ENDSELECT_KW=:TOK_ENDSELECT_KW

	TOK_SELF_KW=:TOK_SELF_KW
	TOK_SUPER_KW=:TOK_SUPER_KW
	TOK_PI_KW=:TOK_PI_KW
	TOK_NEW_KW=:TOK_NEW_KW
	TOK_DELETE_KW=:TOK_DELETE_KW

	TOK_COLON=:TOK_COLON
	TOK_QUESTION=:TOK_QUESTION
	TOK_BANG=:TOK_BANG
	TOK_HASH=:TOK_HASH
	TOK_DOT=:TOK_DOT
	TOK_DOUBLEDOT=:TOK_DOUBLEDOT
	TOK_TRIPLEDOT=:TOK_TRIPLEDOT
	TOK_AT=:TOK_AT
	TOK_DOUBLEAT=:TOK_DOUBLEAT
	TOK_DOLLAR=:TOK_DOLLAR
	TOK_PERCENT=:TOK_PERCENT
	TOK_SINGLEQUOTE=:TOK_SINGLEQUOTE
	TOK_OPENPAREN=:TOK_OPENPAREN
	TOK_CLOSEPAREN=:TOK_CLOSEPAREN
	TOK_OPENBRACKET=:TOK_OPENBRACKET
	TOK_CLOSEBRACKET=:TOK_CLOSEBRACKET
	TOK_OPENCURL=:TOK_OPENCURL
	TOK_CLOSECURL=:TOK_CLOSECURL
	TOK_GREATERTHAN=:TOK_GREATERTHAN
	TOK_LESSTHAN=:TOK_LESSTHAN
	TOK_EQUALS=:TOK_EQUALS
	TOK_MINUS=:TOK_MINUS
	TOK_PLUS=:TOK_PLUS
	TOK_ASTERISK=:TOK_ASTERISK
	TOK_CARET=:TOK_CARET
	TOK_TILDE=:TOK_TILDE
	TOK_GRAVE=:TOK_GRAVE
	TOK_BACKSLASH=:TOK_BACKSLASH
	TOK_SLASH=:TOK_SLASH
	TOK_COMMA=:TOK_COMMA
	TOK_SEMICOLON=:TOK_SEMICOLON
	TOK_PIPE=:TOK_PIPE
	TOK_AMPERSAND=:TOK_AMPERSAND
	TOK_NEWLINE=:TOK_NEWLINE

	TOK_ASSIGN_ADD=:TOK_ASSIGN_ADD
	TOK_ASSIGN_SUBTRACT=:TOK_ASSIGN_SUBTRACT
	TOK_ASSIGN_DIVIDE=:TOK_ASSIGN_DIVIDE
	TOK_ASSIGN_MULTIPLY=:TOK_ASSIGN_MULTIPLY
	TOK_ASSIGN_POWER=:TOK_ASSIGN_POWER

	TOK_ASSIGN_SHL=:TOK_ASSIGN_SHL
	TOK_ASSIGN_SHR=:TOK_ASSIGN_SHR
	TOK_ASSIGN_SAR=:TOK_ASSIGN_SAR
	TOK_ASSIGN_MOD=:TOK_ASSIGN_MOD

	TOK_ASSIGN_XOR=:TOK_ASSIGN_XOR
	TOK_ASSIGN_AND=:TOK_ASSIGN_AND
	TOK_ASSIGN_OR=:TOK_ASSIGN_OR

	TOK_ASSIGN_AUTO=:TOK_ASSIGN_AUTO
	TOK_DOUBLEMINUS=:TOK_DOUBLEMINUS
	TOK_DOUBLEPLUS=:TOK_DOUBLEPLUS

	TOK_NUMBER_LIT=:TOK_NUMBER_LIT
	TOK_HEX_LIT=:TOK_HEX_LIT
	TOK_BIN_LIT=:TOK_BIN_LIT
	TOK_STRING_LIT=:TOK_STRING_LIT

	TOK_LINE_COMMENT=:TOK_LINE_COMMENT
	TOK_BLOCK_COMMENT=:TOK_BLOCK_COMMENT

	TOK_EOF=:TOK_EOF
end