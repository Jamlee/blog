LIBS=foo bar
$(LIBS): %:
	@echo get name: $*

default: $(LIBS)

# ➜  01 git:(2020) ✗ make foo
# get name: foo
# ➜  01 git:(2020) ✗ make default
# get name: foo
# get name: bar
# ➜  01 git:(2020) ✗ make bar
# get name: bar