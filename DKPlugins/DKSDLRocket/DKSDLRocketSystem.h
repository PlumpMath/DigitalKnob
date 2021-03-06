#ifndef SYSTEMINTEFACESDL2_H
#define SYSTEMINTEFACESDL2_H
#include "DK.h"
#include <Rocket/Core/SystemInterface.h>
#include <Rocket/Core/Input.h>
#include <SDL.h>

//////////////////////////////////////////////////////////////////////
class RocketSDL2SystemInterface : public Rocket::Core::SystemInterface
{
public:
    Rocket::Core::Input::KeyIdentifier TranslateKey(SDL_Keycode sdlkey);
    int TranslateMouseButton(Uint8 button);
	int GetKeyModifiers();
	float GetElapsedTime();
    bool LogMessage(Rocket::Core::Log::Type type, const Rocket::Core::String& message);
};
#endif
