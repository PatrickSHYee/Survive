// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

//#include "Survive.generated.h"
#pragma once

#include "CoreMinimal.h"

// in-game items
struct FItem {

	FString Name;

	int weight;

	bool consumable;

	float ammo_type;
};

struct FBleed {
	float amt;

	bool CheckForBleeding() {
		if (amt > 0.1) {
			amt -= 0.08;
			return true;
		}
		else {
			return false;
		}
	}
};

struct FBooster {

	float amt;

	bool CheckForBoost() {
		if (amt > 0) {
			return true;
		}
		return false;
	}
};