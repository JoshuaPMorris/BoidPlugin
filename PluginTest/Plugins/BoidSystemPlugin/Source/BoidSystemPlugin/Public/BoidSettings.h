#pragma once


class ABoid;

class BOIDSYSTEMPLUGIN_API BoidSettings
{
public:
	BoidSettings();
	~BoidSettings();
	
	TArray<ABoid*> ListOfBoids;
	bool DebugOn = true;
};