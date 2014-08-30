#include "Application.hpp"

int main(int argc, char **argv)
{
	Application::getSingleton()->run(argc, argv);
	return 0;
}
