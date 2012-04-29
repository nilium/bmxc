require "expr.rb"

class BinaryOpExpr < Expr
  def initialize token
    super token
  end

  def left
    @left
  end

  def left= expr
    raise "Left expressions must be a kind of Expr" unless left.kind_of? Expr
    @left = expr
  end

  def right
    @right
  end

  def right= expr
    raise "Right expressions must be a kind of Expr" unless right.kind_of? Expr
    @right = expr
  end
end

# *
class MulExpr < BinaryOpExpr
end

# +
class AddExpr < BinaryOpExpr
end

# -
class SubExpr < BinaryOpExpr
end

# /
class DivExpr < BinaryOpExpr
end

# Mod
class ModExpr < BinaryOpExpr
end

# ^
class PowExpr < BinaryOpExpr
end

# & | Shr Shl
class BinaryExpr < BinaryOpExpr
end

# && || = (opt: is)
class LogicalExpr < BinaryOpExpr
end
