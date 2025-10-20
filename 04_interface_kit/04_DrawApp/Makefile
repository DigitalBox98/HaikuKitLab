## Haiku Generic Makefile

# Build both
all: DrawApp PatternApp

# Build DrawApp
DrawApp: 
	@$(MAKE) NAME=DrawApp \
		TYPE=APP \
		SRCS="DrawApp.cpp" \
		LIBS=be \
		APP_MIME_SIG="application/x-vnd.example-draw" \
		-f $(BUILDHOME)/etc/makefile-engine

# Build PatternApp
PatternApp:
	@$(MAKE) NAME=PatternApp \
		TYPE=APP \
		SRCS="PatternApp.cpp" \
		LIBS=be \
		APP_MIME_SIG="application/x-vnd.example-pattern" \
		-f $(BUILDHOME)/etc/makefile-engine

# Clean both
clean:
	@$(MAKE) -f $(BUILDHOME)/etc/makefile-engine clean NAME=DrawApp
	@$(MAKE) -f $(BUILDHOME)/etc/makefile-engine clean NAME=PatternApp

