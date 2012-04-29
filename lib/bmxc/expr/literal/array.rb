require 'bmxc/expr/literal'

class ArrayLiteral < Literal
  attr_accessor :type # type is based on the first value
  attr_accessor :values

  def initialize token
    super token
  end
end
