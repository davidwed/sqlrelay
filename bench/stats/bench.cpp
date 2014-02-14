// Copyright (c) 2014  David Muse
// See the file COPYING for more information
#include "bench.h"

benchconnection::benchconnection() {
}

benchconnection::~benchconnection() {
}

void benchconnection::setColumnCount(uint32_t columncount) {
	this->columncount=columncount;
}

void benchconnection::setRowCount(uint64_t rowcount) {
	this->rowcount=rowcount;
}

benchcursor::benchcursor(benchconnection *conn) {
}

benchcursor::~benchcursor() {
}
