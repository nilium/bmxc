Token = Struct.new :file, :line, :column, :kind, :content

class TokenReader
  TOKEN_REGEX=/(?x:^
    "(?<file> [^"]+)"
    \[ (?<line>  \d+) : (?<column>  \d+) \]
    :
    \[ (?<code>  \d+) : (?<kind>  TOK_[A-Z_]+) \]
    : (?<content> .*)
  $)/

  def self.read_tokens file
    toks = []
    out = `bin/bmxlex "#{file}"`

    out.lines.each {
      |line|

      match = TOKEN_REGEX.match(line)
      file = match[:file]
      line, column = match[:line].to_i, match[:column].to_i
      kind = match[:kind].to_sym
      content = match[:content].gsub(/(?<!\\)\\n/, "\n")
      token = Token.new(file, line, column, kind, content)
      toks << token
    }

    return toks
  end

  def initialize tokens
    raise "No tokens provided" if tokens.nil? || tokens.empty?

    @tokens = Marshal.load(Marshal.dump(tokens)).freeze
    @current = -1
    @mark = -1
  end

  # Sets the rewind marker and returns it
  def mark
    @mark = @current
  end

  def tokens
    @tokens
  end

  def count
    @tokens.length
  end

  def current
    raise "No current token" if @current == -1 || self.eof?
    @tokens[@current]
  end

  # peek { |token| ... }  -> result of block
  # peek                  -> next token or nil
  # Returns nil if attempting to read past EOF
  def peek
    return nil if self.eof? || @current == (@tokens.length - 1)
    peek_index = @current + 1
    if block_given?
      yield @tokens[peek_index]
    else
      @tokens[peek_index]
    end
  end

  def eof?
    @current == @tokens.length - 1
  end

  # read { |token| ... }  -> token
  # If the block's result is false, raises an exception
  # 
  # read                  -> next token
  # Raises an exception if attempting to read past EOF
  def read
    raise "Cannot read after EOF" if self.eof?
    tok = @tokens[@current += 1]
    if block_given?
      raise "Condition not met for token #{tok}" unless yield tok
    end
    tok
  end

  # skips a number of tokens
  # returns self
  def skip count = 1
    raise "Cannot skip past EOF" if @current + count >= @tokens.length - 1 || self.eof?
    @current += count
    self
  end

  # Rewinds to the last marker or to_mark
  def rewind to_mark=nil
    raise "Cannot rewind at the start of the token list" if @current == -1
    to_mark ||= mark
    @current = to_mark
    self
  end
end
