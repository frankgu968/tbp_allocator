TESTS = test_pool_alloc
check_PROGRAMS = test_pool_alloc
test_pool_alloc_SOURCES = test_pool_alloc.c
test_pool_alloc_CFLAGS = -I$(top_srcdir) @CHECK_CFLAGS@ -DDEBUG
test_pool_alloc_LDADD = $(top_builddir)/src/libtmalloc.a @CHECK_LIBS@
