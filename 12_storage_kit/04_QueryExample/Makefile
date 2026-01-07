## Haiku Generic Makefile

# Build both
all: QueryMimeType QueryAttribute

# Build QueryMimeType
QueryMimeType: 
	@$(MAKE) NAME=QueryMimeType \
		TYPE=APP \
		SRCS="QueryMimeType.cpp" \
		LIBS=be \
		APP_MIME_SIG="application/x-vnd.kitlab-listtextfiles" \
		-f $(BUILDHOME)/etc/makefile-engine

# Build QueryAttribute
QueryAttribute:
	@$(MAKE) NAME=QueryAttribute \
		TYPE=APP \
		SRCS="QueryAttribute.cpp" \
		LIBS=be \
		APP_MIME_SIG="application/x-vnd.kitlab-listcommentfiles" \
		-f $(BUILDHOME)/etc/makefile-engine

# Clean both
clean:
	@$(MAKE) -f $(BUILDHOME)/etc/makefile-engine clean NAME=QueryMimeType
	@$(MAKE) -f $(BUILDHOME)/etc/makefile-engine clean NAME=QueryAttribute

