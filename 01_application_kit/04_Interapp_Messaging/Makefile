## Haiku Generic Makefile

# Build both
all: Receiver Sender

# Build Receiver
Receiver: 
	@$(MAKE) NAME=Receiver \
		TYPE=APP \
		SRCS="Receiver.cpp" \
		LIBS=be \
		APP_MIME_SIG="application/x-vnd.example-Receiver" \
		-f $(BUILDHOME)/etc/makefile-engine

# Build Sender
Sender:
	@$(MAKE) NAME=Sender \
		TYPE=APP \
		SRCS="Sender.cpp" \
		LIBS=be \
		APP_MIME_SIG="application/x-vnd.example-Sender" \
		-f $(BUILDHOME)/etc/makefile-engine

# Clean both
clean:
	@$(MAKE) -f $(BUILDHOME)/etc/makefile-engine clean NAME=Receiver
	@$(MAKE) -f $(BUILDHOME)/etc/makefile-engine clean NAME=Sender

