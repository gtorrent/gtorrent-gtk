#include "Base.hpp"
#include "../Application.hpp"

/**
* Sets a base for the rest of the application.
*/
GuiBase::GuiBase() :
	m_core(Application::getSingleton()->getCore())
{
}
