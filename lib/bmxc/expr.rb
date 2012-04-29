class Expr < Statement
  def initialize token
    super token
    @inner = self
  end

  attr_accessor :inner
end
