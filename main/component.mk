# Add the include directory to include paths so that files can be included using <smooth/file.h>

### Select a single test application to build
#CPPFLAGS += -DTEST_QUEUE
#CPPFLAGS += -DTEST_TIMER
CPPFLAGS += -DTEST_NETWORK

COMPONENT_ADD_INCLUDEDIRS := .

COMPONENT_SRCDIRS := \
    . \
	./test