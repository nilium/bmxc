require 'bmxc/statement'

class Decl < Statement
  attr_reader :owner

  def initialize token, owner
    super token
    @owner = owner
  end
end
