#pragma once
#include "Projectile.h"

class Arrow : public Projectile
{
public:
	void Update() override;
	SharedObject GetOwner() override;

public:
	SharedObject Owner;
};

