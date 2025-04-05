#pragma once
#include "GameObject.h"
#include "Info.h"

class Projectile : public GameObject
{
public:
	Projectile();

public:
	SkillData* Data;
};

