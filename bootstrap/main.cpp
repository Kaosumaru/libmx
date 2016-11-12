#include <stdio.h>
#include <iostream>
#include "application/application.h"

class Bootstrap : public MX::App
{
public:
	void OnPrepare() override
	{
		OpenWindow(1280, 800, false);
	}
};

int main(int argc, char* argv[])
{
	Bootstrap app;
	app.Run();
	return 0;
}
