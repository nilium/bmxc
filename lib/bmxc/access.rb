require "expr.rb"

class AccessExpr < Expr
  attr_accessor :left
end

class ArrayAccessExpr < AccessExpr
  attr_accessor :subscript
end

class DotAccessExpr < AccessExpr
  attr_accessor :member
end
