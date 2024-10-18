.PHONY: format docs check

format:
	find include src tests benchmarks fuzz -iname "*.cpp" -o -iname "*.hpp" | xargs clang-format -i

docs:
	doxygen doxygen.conf

check:
	cpregezheck --enable=all include/regez/*.hpp --suppress=unusedFunction -I include  --suppress=missingIncludeSystem --quiet --error-exitcode=1
