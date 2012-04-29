require "expr.rb"

class UnaryOpExpr < Expr
  attr_accessor :operator

  def inner
    @inner
  end

  def inner= expr
    raise "Inner expressions must be a kind of Expr" unless inner.kind_of? Expr
    @inner = expr
  end
end
