// Copyright (C) 2023 owoDra

#pragma once

#include "Modules/ModuleManager.h"

/**
 *  Modules for the blueprint node features of the Game Locomotion: Human Addon plugin
 */
class FGLHAddonNodeModule : public IModuleInterface
{
public:
	virtual void StartupModule() override;
	virtual void ShutdownModule() override;

};
