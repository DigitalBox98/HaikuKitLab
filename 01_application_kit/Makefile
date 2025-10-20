SUBDIRS := $(wildcard [0-9][0-9]_*/)

all:
	@for dir in $(SUBDIRS); do \
		echo "Building $$dir..."; \
		$(MAKE) -C $$dir; \
	done

clean:
	@for dir in $(SUBDIRS); do \
		echo "Cleaning $$dir..."; \
		$(MAKE) -C $$dir clean; \
	done
