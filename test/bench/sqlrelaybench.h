// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#ifndef SQLRELAY_BENCH_H
#define SQLRELAY_BENCH_H

#include "bench.h"

class sqlrelaybenchmarks : public benchmarks {
	public:
		sqlrelaybenchmarks(const char *connectstring,
					const char *db,
					uint64_t queries,
					uint64_t rows,
					uint32_t cols,
					uint32_t colsize,
					uint16_t iterations,
					bool debug);
};

#endif
