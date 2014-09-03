#include <iostream>
#include "Application.hpp"

using namespace std;

shared_ptr<Application> Application::m_app = nullptr;

shared_ptr<Application> Application::getSingleton()
{
	if (m_app == nullptr)
		m_app = shared_ptr<Application>(new Application());
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
	m_gui = make_shared<GuiGtk>(argc, argv);// Who thought it was a good idea to run a GUI from an uninitialized object ?
	m_gui->run();
	m_core->shutdown();
	return 0;
}
