require 'bmxc/token_reader'
require 'bmxc/decl/class_decl'
require 'bmxc/scope'
require 'bmxc/decl/func_decl'
require 'bmxc/decl/storage_decl'
require 'bmxc/expr/literal/array'
require 'bmxc/expr/literal/string'
require 'bmxc/expr/literal/number'
require 'bmxc/expr/literal/hex'
require 'bmxc/expr/literal/bin'

class Parser
  def initialize tokens
    @tr = TokenReader.new tokens
    @scope = Scope.new
  end

  def go
    tr = @tr

    begin
      tok = tr.read

      if tok.kind == :TOK_REM_KW
        skip_block_comment
        next
      elsif tok.kind == :TOK_LINE_COMMENT
        next
      end

      if tok.kind == :TOK_TYPE_KW
        @scope.children.push read_class_decl
      end

    end until tr.eof?

    @scope.children.each {
      |stmt|
      puts stmt
      if stmt.respond_to? :children
        stmt.children.each {
          |c|
          puts c.inspect
        }
      end
    }

  end

  def skip_block_comment
    tr = @tr
    until tr.eof?
      tok = tr.read
      break if tok.kind == :TOK_ENDREM_KW
    end
  end


  def read_class_decl
    tr = @tr

    typetok = tr.current
    name = tr.read { |t| t.kind == :TOK_ID }

    decl = ClassDecl.new typetok, @scope, name.content

    if tr.peek.kind == :TOK_EXTENDS_KW
      tok = tr.skip.read
      decl.superclass = Type.new tok, decl, tok.content
    end

    until tr.eof?

      tok = tr.read

      case tok.kind
      when :TOK_FIELD_KW
        decl.children.push *read_variable_list(decl, FieldDecl)
      when :TOK_GLOBAL_KW
        decl.children.push *read_variable_list(decl, GlobalDecl)
      when :TOK_CONST_KW
        decl.children.push *read_variable_list(decl, ConstantDecl)
      when :TOK_FUNCTION_KW, :TOK_METHOD_KW
        decl.children.push read_func_decl decl
      else
      end

      break if tok.kind == :TOK_ENDTYPE_KW

    end

    return decl
  end

  def read_func_decl owner
    tr = @tr
    mark = tr.mark
    start = tr.current
    decl = nil
    tok = tr.read { |t| [:TOK_ID, :TOK_DELETE_KW, :TOK_NEW_KW].include? t.kind }
    if start.kind == :TOK_FUNCTION_KW
      decl = FuncDecl.new start, owner, tok.content
      end_token_kind = :TOK_ENDFUNCTION_KW
    elsif start.kind == :TOK_METHOD_KW
      decl = MethodDecl.new start, owner, tok.content
      end_token_kind = :TOK_ENDMETHOD_KW
    else
      raise "Invalid token for function start"
    end

    until tr.eof?
      break if tr.read.kind == end_token_kind
    end

    decl
  end

  def read_variable_list owner, declType
    tr = @tr

    vars = []

    until tr.eof?
      name = tr.read { |t| t.kind == :TOK_ID }
      decl = declType.new name, owner, name.content
      vars.push decl

      cont = tr.peek.kind
      if [:TOK_COLON, :TOK_PERCENT, :TOK_AT, :TOK_DOUBLEAT, :TOK_BANG, :TOK_HASH, :TOK_DOLLAR].include? cont
        tr.skip if cont == :TOK_COLON
        decl.type = read_type decl
        cont = tr.peek.kind
      end

      if cont == :TOK_EQUALS
        decl.value = read_expr
        cont = tr.peek.kind
      end

      break unless cont == :TOK_COMMA
    end

    return vars
  end

  def read_expr
    tr = @tr

    tok = tr.read

    case tok.kind
    when :TOK_OPENPAREN
      expr = ParenExpr.new tok
      expr.inner = read_expr
      tr.read { |t| t.kind == :TOK_CLOSEPAREN }

    when :TOK_OPENBRACKET
      expr = ArrayLiteral.new tok

    when :TOK_NUMBER_LIT
      expr = NumberLiteral.new tok
    when :TOK_HEX_LIT
    when :TOK_BIN_LIT
    when :TOK_STRING_LIT

    when :TOK_ID
    end

    return expr
  end

  def read_array_literal
    tr = @tr
    lit = ArrayLiteral.new tr.current

    until tr.eof?
      tok = tr.read

      case tok.kind
      when :TOK_CLOSEBRACKET

      end

    end
  end

  def read_type owner
    tr = @tr

    tok = tr.read
    typ = Type.new tok, owner, tok.content

    if tr.peek.kind == :TOK_PTR_KW
      typ = PointerType.new tr.read, typ
      while tr.peek { |t| if t.kind == :TOK_PTR_KW; tr.skip; typ.level += 1; end }; end
    end

    while tr.peek.kind == :TOK_OPENBRACKET
      typ = ArrayType.new tr.read, typ

      if tr.peek.kind != :TOK_CLOSEBRACKET
        typ.length = read_expr
      end

      tr.read { |t| t.kind == :TOK_CLOSEBRACKET }
    end

    if tr.peek.kind == :TOK_VAR_KW
      typ = RefType.new tr.read, typ
    end

    return typ
  end

end
