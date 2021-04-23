#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/GameObject/Stats/StatEventWrapper.h"
#include "bakkesmod/wrappers/UniqueIDWrapper.h"

using namespace std;

typedef map<string, int> StatMap;

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

class TrackMyStats : public BakkesMod::Plugin::BakkesModPlugin {
private:
	UniqueIDWrapper uniqueID;
	float currentMMR;
	std::unique_ptr<MMRNotifierToken> mmrNotifier;
	filesystem::path dataDir;
	StatMap stats;

public:
	virtual void onLoad();
	virtual void onUnload();

	void Bootstrap();
	void HandleStatEvent(ServerWrapper caller, void* args);
	void HandleStatTickerEvent(ServerWrapper caller, void* args);
	void HandleGameStart();
	void HandleGameEnd();
	void GetMMR(UniqueIDWrapper id);

	GameSettingPlaylistWrapper GetCurrentPlaylist();
	PriWrapper GetPrimaryPlayer();
	bool IsPrimaryPlayer(PriWrapper target);
	bool InValidGame();

	void Stringify();
	void IncrementStat(string label);
	void HandleDemolition(PriWrapper actor, PriWrapper victim);
};