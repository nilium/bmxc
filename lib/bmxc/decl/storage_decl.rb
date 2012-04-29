require 'bmxc/type'

class StorageDecl < Decl
  attr_accessor :name
  attr_reader :token
  attr_accessor :type
  attr_accessor :value

  def initialize token, owner, name
    super token, owner
    @type = Type.new token, self, 'int'
    @name = name
  end
end

class GlobalDecl < StorageDecl
  def inspect
    "Global #{@name}#{":#{@type.to_s}" unless @type.nil?}#{" = #{@value.to_s}" unless @value.nil?}"
  end
end

class ConstantDecl < StorageDecl
  def inspect
    "Const #{@name}#{":#{@type.to_s}" unless @type.nil?}#{" = #{@value.to_s}" unless @value.nil?}"
  end
end

class FieldDecl < StorageDecl
  def inspect
    "Field #{@name}#{":#{@type.to_s}" unless @type.nil?}#{" = #{@value.to_s}" unless @value.nil?}"
  end
end
