class Type
  attr_accessor :name
  attr_accessor :ref
  attr_accessor :inner
  attr_reader :owner
  attr_reader :owner

  def initialize token, owner, name
    @name = case name.nil? ? nil : name.downcase
      when nil  ; 'int'
      when '%', 'int'    ; 'int'
      when '%%', 'long'  ; 'int64_t'
      when '@', 'byte'   ; 'uint8_t'
      when '@@', 'short' ; 'uint16_t'
      when '!', 'double' ; 'double'
      when '#', 'float'  ; 'float'
      when '$', 'string' ; 'string'
    end
    @token = token
    @owner = owner
  end

  def resolve

  end

  def to_s
    @name
  end
end

class RefType < Type
  def initialize token, inner
    super token, inner, nil
    self.inner = inner
  end

  def to_s
    "#{inner.to_s} &"
  end
end

class PointerType < RefType
  attr_accessor :level

  def initialize token, inner
    super token, inner
    @level = 1
  end

  def to_s
    "#{inner.to_s} #{'*'*@level}"
  end
end

class ArrayType < RefType
  attr_accessor :length

  def to_s
    "#{inner.to_s}[#{length.to_s unless length.nil?}]"
  end
end
