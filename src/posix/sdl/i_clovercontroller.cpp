#ifdef CLOVER_CONTROLLER
#include <SDL.h>
#include <list>

#include "doomdef.h"
#include "d_event.h"
#include "m_joy.h"

const short buttonValues[18] = { KEY_PAD_A, KEY_PAD_B, KEY_PAD_X, KEY_PAD_Y, KEY_PAD_BACK, 0, KEY_PAD_START, KEY_PAD_LTHUMB, KEY_PAD_RTHUMB, KEY_PAD_LSHOULDER, KEY_PAD_RSHOULDER, KEY_PAD_DPAD_UP, KEY_PAD_DPAD_DOWN, KEY_PAD_DPAD_LEFT, KEY_PAD_DPAD_RIGHT, 0, KEY_PAD_LTRIGGER, KEY_PAD_RTRIGGER };

struct SDLGameControllerJoyStick : public IJoystickConfig
{
	SDLGameControllerJoyStick(SDL_GameController* controller, int index) : controller(controller), index(index), scale(1.0f)
	{
		SetDefaultConfig();
	}
	
	~SDLGameControllerJoyStick()
	{
		M_SaveJoystickConfig(this);
	}
	
	FString GetName()
	{
		return SDL_GameControllerName(controller);
	}
	
	float GetSensitivity()
	{
		return scale;
	}
	
	void SetSensitivity(float scale)
	{
		this->scale = scale;
	}

	int GetNumAxes()
	{
		return 4;
	}
	
	float GetAxisDeadZone(int axis)
	{
		return axes[axis].deadZone;
	}
	
	EJoyAxis GetAxisMap(int axis)
	{
		return axes[axis].gameAxis;
	}
	
	const char *GetAxisName(int axis)
	{
		return axes[axis].name.GetChars();
	}
	
	float GetAxisScale(int axis)
	{
		return axes[axis].scale;
	}

	void SetAxisDeadZone(int axis, float zone)
	{
		axes[axis].deadZone = zone;
	}
	
	void SetAxisMap(int axis, EJoyAxis gameaxis)
	{
		axes[axis].gameAxis = gameaxis;
	}
	
	void SetAxisScale(int axis, float scale)
	{
		axes[axis].scale = scale;
	}

	bool IsSensitivityDefault()
	{
		return scale == 1.0f;
	}
	
	bool IsAxisDeadZoneDefault(int axis)
	{
		return axes[axis].deadZone == defaultAxes[axis].deadZone;
	}
	
	bool IsAxisMapDefault(int axis)
	{
		return axes[axis].gameAxis == defaultAxes[axis].gameAxis;
	}
	
	bool IsAxisScaleDefault(int axis)
	{
		return axes[axis].scale == 1.0f;
	}

	void SetDefaultConfig()
	{
		for(int i = 0; i < 4; ++i)
			axes[i] = defaultAxes[i];
	}
	
	FString GetIdentifier()
	{
		char id[16];
		mysnprintf(id, countof(id), "GC:%d", index);
		return id;
	}
	
	void ProcessInput()
	{
		for(int i = 0; i < 4; ++i)
			axes[i].value = Joy_RemoveDeadZone(SDL_GameControllerGetAxis(controller,(SDL_GameControllerAxis)i)/32767.0f, axes[i].deadZone, NULL);
	}
	
	void AddAxes(float g_axes[NUM_JOYAXIS])
	{
		for (int i = 0; i < 4; ++i)
		{
			if(axes[i].gameAxis != JOYAXIS_None)
				g_axes[axes[i].gameAxis] -= float(axes[i].value * scale * axes[i].scale);
		}
	}
	
	struct Axis
	{
		FString name;
		EJoyAxis gameAxis;
		float deadZone;
		float scale;
		float value;
	};
private:
	Axis axes[4];
	static const Axis defaultAxes[4];
	SDL_GameController* controller;
	int index;
	float scale;
};
const SDLGameControllerJoyStick::Axis SDLGameControllerJoyStick::defaultAxes[4] = {{"Left Stick X", JOYAXIS_Side, 0.000001f, 1.0f},
						{"Left Stick Y", JOYAXIS_Forward, 0.000001f, 1.0f},
						{"Right Stick X", JOYAXIS_Yaw, 0.000001f, 1.0f},
						{"Right Stick Y", JOYAXIS_Pitch, 0.000001f, 1.0f}};

class SDLGameController
{
public:
	SDLGameController(int id)
	{
		controller = SDL_GameControllerOpen(id);
		axisCount = SDL_JoystickNumAxes(SDL_GameControllerGetJoystick(controller));
		if(axisCount >= 4)
			joystick = new SDLGameControllerJoyStick(controller, id);
		else
			joystick = nullptr;
	}
	
	~SDLGameController()
	{
		if(joystick)
			delete joystick;
		SDL_GameControllerClose(controller);
	}
	
	void ProcessInput()
	{
		for(int i = 0; i < 16; ++i)
			processButtonState(SDL_GameControllerGetButton(controller, (SDL_GameControllerButton)i), buttonState[i], i);
		
		if(joystick)
		{
			if(axisCount >= 6)
			{
				processButtonState(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERLEFT), buttonState[16], 16);
				processButtonState(SDL_GameControllerGetAxis(controller, SDL_CONTROLLER_AXIS_TRIGGERRIGHT), buttonState[17], 17);
			}
			joystick->ProcessInput();
		}
	}
	
	void AddAxes(float axes[NUM_JOYAXIS])
	{
		if(joystick)
			joystick->AddAxes(axes);
	}
	
	SDLGameControllerJoyStick* GetJoystick()
	{
		return joystick;
	}
private:
	SDL_GameController* controller;
	SDLGameControllerJoyStick* joystick;
	int axisCount;
	bool buttonState[18] = { 0 };
	
	void processButtonState(bool newState, bool & oldState, int buttonId)
	{
		if(newState != oldState)
		{
			event_t event = { newState ? EV_KeyDown : EV_KeyUp, 0, buttonValues[buttonId] };
			if(event.data1 != 0)
				D_PostEvent(&event);
			
			oldState = newState;
		}
	}
};

class SDLGameControllerManager
{
public:
	SDLGameControllerManager()
	{
		for (int i = 0; i < SDL_NumJoysticks(); ++i)
		{
			if (SDL_IsGameController(i))
			{
				controllers.push_back(new SDLGameController(i));
			}
		}
	}
	
	~SDLGameControllerManager()
	{
		for(auto & controller : controllers)
			delete controller;
	}
	
	void ProcessInput()
	{
		for(auto & controller : controllers)
			controller->ProcessInput();
	}
	
	void GetJoysticks(TArray<IJoystickConfig *> &sticks)
	{
		for(auto & controller : controllers)
		{
			auto joystick = controller->GetJoystick();
			if(joystick)
			{
				M_LoadJoystickConfig(joystick);
				sticks.Push(joystick);
			}
		}
	}
	
	void AddAxes(float axes[NUM_JOYAXIS])
	{
		for(auto & controller : controllers)
			controller->AddAxes(axes);
	}
private:
	std::list<SDLGameController*> controllers;
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
	GameControllerManager->GetJoysticks(sticks);
}

void I_GetAxes(float axes[NUM_JOYAXIS])
{
	memset(axes, 0, sizeof(float)*NUM_JOYAXIS);
	if (use_joystick)
		GameControllerManager->AddAxes(axes);
}

void I_ProcessJoysticks()
{
	if(GameControllerManager)
		GameControllerManager->ProcessInput();
}

IJoystickConfig *I_UpdateDeviceList()
{
	return NULL;
}
#endif
