require "expr.rb"

class FuncCallExpr < Expr
  attr_accessor :function  # function name
  attr_accessor :arguments # array of expressions
end

class MemberCallExpr < FuncCallExpr
  attr_accessor :expression # expression the method is being pulled from
end
