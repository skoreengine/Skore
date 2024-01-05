// Copyright 2023 Paulo Marangoni.
// Use of this source code is governed by the license that can be found in the LICENSE file at the root of this distribution.

#include "Skore/App.hpp"

using namespace Skore;

int main()
{
	App::Init();

	App::CreateContext(AppContextCreation{
		.Title = "Skore",
		.Maximized = true
	});

	return App::Run();
}