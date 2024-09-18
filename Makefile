.PHONY: format

format:
	find include src -iname "*.cpp" -o -iname "*.hpp" | xargs clang-format -i
