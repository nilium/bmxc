require 'bmxc/type'
require 'bmxc/decl'

class ClassDecl < Decl
  attr_accessor :name
  attr_accessor :superclass

  # children = functions, variables (fields, globals, consts), subtypes
  attr_accessor :children

  def initialize token, owner, name=""
    super token, owner
    @name = name
    @children = []
    @superclass = nil
    @scope = nil
  end

  def to_s
    "#{@name}#{" : public #{superclass.name}" if @superclass}"
  end
end
