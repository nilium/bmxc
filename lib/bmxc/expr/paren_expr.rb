class ParenExpr < Expr
  def initialize token
    super token
  end

  def to_s
    "(#{self.inner.to_s})"
  end
end
