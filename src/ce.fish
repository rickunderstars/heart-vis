#!/usr/bin/env fish
# script to compile and execute c++ code

function compile_and_execute
	if test (count $argv) -eq 0
		echo "{ missing file }"
		echo "usage: ce.fish <filepath.cpp>"
		return 1
	end
	if not test -f "$argv[1]"
		echo "{ file not found }"
		return 1
	end

	set src "$argv[1]"
	set base (basename "$src")
	set out_name (string replace '.cpp' '.out' "$base")

	echo "Compiling '$src'..."

	g++ "$src" -o "$out_name"

	if test $status -eq 0
		echo ""
		echo (set_color brgreen)"      +--------------------------+"(set_color normal)
		echo (set_color brgreen)"      | compilation successfull! |"(set_color normal)
		echo (set_color brgreen)"      +--------------------------+"(set_color normal)
		echo ""
		echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
		echo ""
		./$out_name
	else
		echo ""
		echo "~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"
		echo ""
		echo (set_color brred)"      x-----------------------x"(set_color normal)
		echo (set_color brred)"      | compilation failed... |"(set_color normal)
		echo (set_color brred)"      x-----------------------x"(set_color normal)
		echo ""
	end
end

compile_and_execute $argv