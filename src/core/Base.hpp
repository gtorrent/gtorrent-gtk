#pragma once

#include <gtorrent/Core.hpp>

class GuiBase
{
protected:
	std::shared_ptr<gt::Core> &m_core;

public:
	GuiBase();
	virtual int run() = 0;
	static void loadDefaultSettings();
};

