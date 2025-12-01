## Haiku Generic Makefile

# Build both
all: MarkdownTranslator UseTranslator

# Build MarkdownTranslator
MarkdownTranslator: 
	@$(MAKE) NAME=MarkdownTranslator \
		TYPE=APP \
		SRCS="MarkdownTranslator.cpp" \
		LIBS="be translation stdc++" \
		APP_MIME_SIG="application/x-vnd.md-translator" \
		-f $(BUILDHOME)/etc/makefile-engine

# Build UseTranslator
UseTranslator:
	@$(MAKE) NAME=UseTranslator \
		SRCS="UseTranslator.cpp" \
		LIBS="be translation stdc++" \
		-f $(BUILDHOME)/etc/makefile-engine

# Clean both
clean:
	@$(MAKE) -f $(BUILDHOME)/etc/makefile-engine clean NAME=MarkdownTranslator
	@$(MAKE) -f $(BUILDHOME)/etc/makefile-engine clean NAME=UseTranslator

