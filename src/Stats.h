#pragma once
#include <string>
#include <map>

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"

using namespace std;

typedef map<string, int> StatMap;
typedef map<string, string> StatEventDictionary;

struct StatHUDEvent {
	uintptr_t Actor;
	uintptr_t Victim;
	uintptr_t StatEvent;
};

struct StatEvent {
	uintptr_t Actor;
	uintptr_t StatEvent;
	uintptr_t Count;
};

class Stats {
private:
	StatMap stats;
	string player;

	bool IsPlayer(PriWrapper target);
	void HandleDemolition(PriWrapper actor, PriWrapper victim);

	StatEventDictionary statLabelDict = {
		{ "Aerial Goal", "aerialGoals" },
		{ "Aerial Hit", "aerialHits" },
		{ "Backwards Goal", "backwardsGoals" },
		{ "Ball Touch", "ballTouches" },
		{ "Bicycle Goal", "bicycleGoals" },
		{ "Bicycle Hit", "bicycleHits" },
		{ "Boost Pickup", "boostPickups" },
		{ "Center Ball", "centers" },
		{ "Clear Ball", "clears" },
		{ "Damage", "damages" },
		{ "Epic Save", "epicSaves" },
		{ "Extermination", "exterms" },
		{ "First Touch", "firstTouches" },
		{ "Hat Trick", "hatTricks" },
		{ "High Five", "highFives" },
		{ "Long Goal", "longGoals" },
		{ "Low Five", "lowFives" },
		{ "Overtime Goal", "overtimeGoals" },
		{ "Playmaker", "playmakers" },
		{ "Pool Shot", "poolShots" },
		{ "Save", "saves" },
		{ "Savior", "saviors" },
		{ "Swish Goal", "swishes" },
		{ "Turtle Goal", "turtleGoals" },
		{ "Ultra Damage", "ultraDamages" },
	};
public:
	Stats();
	void SetPlayer(PriWrapper player);
	string ToJSON();
	void Increment(string statKey);
	void OnStatEvent(void* args);
	void OnStatTickerEvent(void* args);
};