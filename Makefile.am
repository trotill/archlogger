SUBDIRS = src

if GCC
# These flags are gcc specific
export GCC_AM_CFLAGS = -O2 -g -ggdb -Wall -Wextra -Wstrict-prototypes -Wmissing-prototypes -Wwrite-strings -Wno-variadic-macros -pedantic -Wimport -Wfatal-errors -Wformat=2 -Winit-self -Wmissing-include-dirs -Wswitch-default -Wunknown-pragmas -Wmissing-noreturn
endif

if GXX
# These flags are g++ specific
export GXX_AM_CXXFLAGS = -O2 -g -ggdb -Wall -Wextra -Wwrite-strings -Wno-variadic-macros -pedantic -Wimport -Wfatal-errors -Wformat=2 -Winit-self -Wmissing-include-dirs -Wswitch-default -Wunknown-pragmas -Wmissing-noreturn
endif
