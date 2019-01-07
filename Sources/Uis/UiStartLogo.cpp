﻿#include "UiStartLogo.hpp"

#include "Events/EventTime.hpp"
#include "Events/Events.hpp"
#include "Maths/Visual/DriverConstant.hpp"
#include "Maths/Visual/DriverSlide.hpp"

namespace acid
{
#if defined(ACID_VERBOSE)
	const Time START_DELAY = Time::Seconds(1.5f);
#else
	const Time START_DELAY = Time::Seconds(3.0f);
#endif

	UiStartLogo::UiStartLogo(UiObject *parent) :
		UiObject(parent, UiBound(Vector2(0.5f, 0.5f), UiBound::CENTRE, true, false, Vector2(1.0f, 1.0f))),
		m_guiBackground(std::make_unique<Gui>(this, UiBound(Vector2(0.5f, 0.5f), UiBound::CENTRE, true, false, Vector2(1.0f, 1.0f)), Texture::Create("Guis/Black.png"))),
		m_guiLogoAcid(std::make_unique<Gui>(this, UiBound(Vector2(0.5f, 0.5f), UiBound::CENTRE, true, true, Vector2(0.4f, 0.4f)), Texture::Create("Logos/Flask-02.png"))),
		m_textCopyright(std::make_unique<Text>(this, UiBound(Vector2(0.5f, 0.2f), UiBound::CENTRE, true), 1.8f, "Copyright (C) 2018, Equilibrium Games - All Rights Reserved.",
			FontType::Create("Fonts/ProximaNova", "Regular"), TEXT_JUSTIFY_CENTRE, 0.8f, Colour::WHITE, 0.0012f, 0.024f)),
		m_starting(true)
	{
		Events::Get()->AddEvent<EventTime>([&]()
		{
			SetAlphaDriver<DriverSlide>(1.0f, 0.0f, Time::Seconds(1.4f));
		}, START_DELAY);
	}

	void UiStartLogo::UpdateObject()
	{
	}
}
