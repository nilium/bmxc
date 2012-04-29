
module Operators
  Operator = Struct.new :kind, :priority

  PREFIX_INC          = Operator.new '++'.to_sym,  10
  PREFIX_DEC          = Operator.new '--'.to_sym,  10

  POSTFIX_INC         = Operator.new '++'.to_sym,  20
  POSTFIX_DEC         = Operator.new '--'.to_sym,  20

  NEGATION            = Operator.new :-,           20
  LOGICAL_NOT         = Operator.new '!'.to_sym,   20
  LOGICAL_BNOT        = Operator.new :~,           20
  ADDRESS_OF          = Operator.new :&,           20
  NEW                 = Operator.new :New,         20
  DELETE              = Operator.new :Delete,      20

  MULTIPLY            = Operator.new :*,           30
  DIVIDE              = Operator.new :/,           30
  MODULO              = Operator.new :%,           30

  ADD                 = Operator.new :+,           40
  SUBTRACT            = Operator.new :-,           40

  LEFT_SHIFT          = Operator.new :<<,          50
  RIGHT_SHIFT         = Operator.new :>>,          50

  LESS_THAN           = Operator.new :<,           60
  LESS_EQUAL          = Operator.new :<=,          60
  GREATER_THAN        = Operator.new :>,           60
  GREATER_EQUAL       = Operator.new :>=,          60

  EQUALITY            = Operator.new :==,          70
  INEQUALITY          = Operator.new '!='.to_sym,  70

  BITWISE_AND         = Operator.new :&,           80
  BITWISE_OR          = Operator.new :|,           90
  BITWISE_XOR         = Operator.new '^'.to_sym,   100

  LOGICAL_AND         = Operator.new '&&'.to_sym,  110
  LOGICAL_OR          = Operator.new '||'.to_sym,  120

  ASSIGN              = Operator.new '='.to_sym,   130
  ASSIGN_MULTIPLY     = Operator.new '*='.to_sym,  130
  ASSIGN_DIVIDE       = Operator.new '/='.to_sym,  130
  ASSIGN_MODULO       = Operator.new '%='.to_sym,  130
  ASSIGN_ADD          = Operator.new '+='.to_sym,  130
  ASSIGN_SUBTRACT     = Operator.new '-='.to_sym,  130
  ASSIGN_LEFT_SHIFT   = Operator.new '<<='.to_sym, 130
  ASSIGN_RIGHT_SHIFT  = Operator.new '>>='.to_sym, 130
  ASSIGN_BITWISE_AND  = Operator.new '&='.to_sym,  130
  ASSIGN_BITWISE_OR   = Operator.new '|='.to_sym,  130
  ASSIGN_BITWISE_XOR  = Operator.new '^='.to_sym,  130
end