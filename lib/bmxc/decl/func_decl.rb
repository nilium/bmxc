require 'bmxc/decl'

class FuncDecl < Decl
  attr_reader :owner
  attr_reader :name
  attr_accessor :params

  def initialize token, owner, name
    super token, owner

    @name = name
  end

  def to_s
    "Function #{name}"
  end
end

class MethodDecl < FuncDecl

  def to_s
    "Method #{name}"
  end
end
