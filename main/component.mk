CPPFLAGS += -D_GLIBCXX_USE_C99

### Select a single test application to build
#CPPFLAGS += -DTEST_QUEUE
#CPPFLAGS += -DTEST_TIMER
#CPPFLAGS += -DTEST_NETWORK
CPPFLAGS += -DTEST_RGB_LED

COMPONENT_ADD_INCLUDEDIRS := .

COMPONENT_SRCDIRS := \
    . \
	./test