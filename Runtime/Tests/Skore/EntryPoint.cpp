// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"


int main(int argc, char** argv)
{
	doctest::Context context;
	context.applyCommandLine(argc, argv);
	context.setOption("no-breaks", true);
	return context.run();
}