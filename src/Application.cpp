#include <iostream>
#include "Application.hpp"

using namespace std;

shared_ptr<Application> Application::getSingleton()
{
        static std::shared_ptr<Application> m_app = std::shared_ptr<Application>(new Application());

        return m_app;
}

shared_ptr<gt::Core> &Application::getCore()
{
	return m_core;
}

int Application::run(int argc, char **argv)
{
	gt::Log::Debug("gTorrent v." GTORRENT_VERSION);
	gt::Log::Debug("Starting up core library...");
	m_gui->loadDefaultSettings();
	m_core = make_shared<gt::Core>(argc, argv);

	gt::Log::Debug("Starting up GUI layer...");
	m_gui = make_shared<gt::GuiGtk>(argc, argv);
	m_gui->run();

	m_core->shutdown();
	return 0;
}
