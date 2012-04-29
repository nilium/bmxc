#!/usr/bin/env ruby -w

SOURCE_FILE = /(?i:\.(c(c|pp|xx|\+\+)?|m{1,2}))$/
INCLUDE_FILE = /^\s*#\s*include\s+(["<])([^">]+)[">]/

# filepath = path to the source file (relative or absolute)
# search_path = paths to search for system includes (relative or absolute)
def pull_dependencies(filepath, search_paths=[], depth=1, checked=[])
  if depth > 32
    raise "We've gone too deep, Captain!"
  end

  abs_path = File.absolute_path(filepath)
  return [] if checked.include?(abs_path) && depth != 0
  checked << abs_path

  dir = File.dirname(filepath)
  deps = []
  
  raise "File does not exist" unless File.exists?(filepath) && ! File.directory?(filepath)

  File.open(filepath, "r") {
    |ios|
    ios.each_line {
      |line|
      if (line =~ INCLUDE_FILE)
        scope = $1
        incl_path = $2
        if scope == '<' # search
          found = false

          search_paths.each {
            |search_path|
            check_path = "#{search_path}/#{incl_path}"
            # puts "Checking for <#{check_path}>"
            if File.exists?(check_path) && ! File.directory?(check_path)
              deps << check_path
              deps |= pull_dependencies(check_path, search_paths, depth + 1, checked)
              found = true
              break
            end
          }

          if ! found
            # puts "Unable to locate <#{incl_path}>"
          end

        elsif scope === '"' # relative
          check_path = "#{dir}/#{incl_path}"
          # puts "Checking for \"#{check_path}\""
          if File.exists?(check_path) && ! File.directory?(check_path)
            deps << check_path
            deps |= pull_dependencies(check_path, search_paths, depth + 1, checked)
          else
            # puts "Unable to locate \"#{incl_path}\""
          end
        end
      end

    }
  }

  return deps
end

def get_sources(search_in=".")
  search_in = search_in.chomp '/'
  Dir.glob("#{search_in}/**/*").select {
    # ignore non-C/C++ source, files starting/in a dir starting with a ., and files in dirs marked .exclude
    |e| File.extname(e) =~ SOURCE_FILE && ! (e =~ /\/\.[^\.]/) && ! (e.include? '.exclude/')
  }
end

def dump_make(ios, sources)
  objects = sources.map {
    |e|
    n = "#{e.chomp(File.extname(e))}.o"
    raise "Failed to convert source file to object file" if e == n
    n
  }

  ios.puts "SOURCES=\\"
  ios.puts "  " + sources.join(" \\\n  ")
  ios.write $/

  ios.puts "OBJECTS=\\"
  ios.puts objects.join(" \\\n  ")
  ios.write $/

  sources.each {
    |filepath|

    filedeps = pull_dependencies(filepath, ["src"])

    ios.puts "#{filepath.sub(SOURCE_FILE, '.o')}: #{filepath} #{filedeps.join ' '}"
    ios.puts "\t$(CC) $(CFLAGS) $(CXXFLAGS) -c $< -o $@\n\n"
  }
end

sources = get_sources('src')
dump_make $stdout, sources

# sources = Dir.glob("src/**/*")
# sources.delete_if {
  # |path|
  # puts path
  # path.include? ".exclude/" || ! path.match(/\/\.[^\.]/).nil?
# }




# File.open("Makefile.sources", "w") {
  # |ios|
# }
