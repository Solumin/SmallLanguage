#!/usr/bin/ruby

# Test files are in the 'examples/' directory
tests = Dir['examples/*']
# The executable:
parser = "./small_parser.exe"
# Failure states
failure = { 2 => "Parsing failed" }

tests.sort.each do |test|
    output = `#{parser} #{test}`
    if failure[$?.exitstatus]
        puts "FAIL #{test}"
        puts "\t#{output.lines[0]}"
    else
        puts "PASS #{test}"
    end
end
