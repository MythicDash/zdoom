#ifdef CLOVER_CONTROLLER
#include <SDL.h>
#include <list>

#include "m_joy.h"

extern bool GUICapture;

class SDLGameControllerManager
{
public:
	SDLGameControllerManager()
	{
		for (int i = 0; i < SDL_NumJoysticks(); ++i)
		{
			if (SDL_IsGameController(i))
			{
				controllers.push_back(SDL_GameControllerOpen(i));
			}
		}
	}
	
	~SDLGameControllerManager()
	{
		for(auto controller : controllers)
		{
			SDL_GameControllerClose(controller);
		}
	}
	
	void ProcessEvent(const SDL_Event &sev)
	{
		SDL_Keycode keycode;
		SDL_Scancode scancode;
		
		switch(sev.cbutton.button)
		{
		case SDL_CONTROLLER_BUTTON_DPAD_UP:
			keycode = SDLK_UP;
			scancode = SDL_SCANCODE_UP;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
			keycode = SDLK_DOWN;
			scancode = SDL_SCANCODE_DOWN;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
			keycode = SDLK_LEFT;
			scancode = SDL_SCANCODE_LEFT;
			break;
		case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
			keycode = SDLK_RIGHT;
			scancode = SDL_SCANCODE_RIGHT;
			break;
		case SDL_CONTROLLER_BUTTON_LEFTSHOULDER:
			keycode = SDLK_l;
			scancode = SDL_SCANCODE_L;
			break;
		case SDL_CONTROLLER_BUTTON_RIGHTSHOULDER:
			keycode = SDLK_r;
			scancode = SDL_SCANCODE_R;
			break;
		case SDL_CONTROLLER_BUTTON_START:
			keycode = SDLK_ESCAPE;
			scancode = SDL_SCANCODE_ESCAPE;
			break;
		case SDL_CONTROLLER_BUTTON_BACK:
			keycode = SDLK_BACKSPACE;
			scancode = SDL_SCANCODE_BACKSPACE;
			break;
		case SDL_CONTROLLER_BUTTON_A:
			keycode = (GUICapture) ? SDLK_RETURN : SDLK_a;
			scancode = (GUICapture) ? SDL_SCANCODE_RETURN : SDL_SCANCODE_A;
			break;
		case SDL_CONTROLLER_BUTTON_B:
			keycode = SDLK_b;
			scancode = SDL_SCANCODE_B;
			break;
		case SDL_CONTROLLER_BUTTON_X:
			keycode = SDLK_x;
			scancode = SDL_SCANCODE_X;
			break;
		case SDL_CONTROLLER_BUTTON_Y:
			keycode = SDLK_y;
			scancode = SDL_SCANCODE_Y;
			break;
		default:
			return;
		}
		
		SDL_Event keyPressEvent;
		keyPressEvent.type = (sev.type == SDL_CONTROLLERBUTTONDOWN) ? SDL_KEYDOWN : SDL_KEYUP;
		keyPressEvent.key.keysym.sym = keycode;
		keyPressEvent.key.keysym.scancode = scancode;
		keyPressEvent.key.keysym.mod = 0;
		SDL_PushEvent(&keyPressEvent);
	}
private:
	std::list<SDL_GameController*> controllers;
} static *GameControllerManager;


void I_StartupJoysticks()
{
	SDL_SetHint(SDL_HINT_JOYSTICK_ALLOW_BACKGROUND_EVENTS,"1");
	if(SDL_InitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER) >= 0)
	{
		SDL_GameControllerAddMappingsFromFile("/etc/sdl2/gamecontrollerdb.txt");
		GameControllerManager = new SDLGameControllerManager();
	}
}
void I_ShutdownJoysticks()
{
	if(GameControllerManager)
	{
		delete GameControllerManager;
		SDL_QuitSubSystem(SDL_INIT_JOYSTICK | SDL_INIT_GAMECONTROLLER);
	}
}

void I_GetJoysticks(TArray<IJoystickConfig *> &sticks)
{
	sticks.Clear();
}

void I_GetAxes(float axes[NUM_JOYAXIS])
{
	memset(axes, 0, sizeof(float)*NUM_JOYAXIS);
}

void I_ProcessGameControllerEvent(const SDL_Event &sev)
{
	if(GameControllerManager)
		GameControllerManager->ProcessEvent(sev);
}

void I_ProcessJoysticks() {}

IJoystickConfig *I_UpdateDeviceList()
{
	return NULL;
}
#endif
