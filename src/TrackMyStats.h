#pragma once

#include "bakkesmod/plugin/bakkesmodplugin.h"
#include "bakkesmod/wrappers/UniqueIDWrapper.h"
#include "Stats.h"

using namespace std;

class TrackMyStats : public BakkesMod::Plugin::BakkesModPlugin {
private:
	UniqueIDWrapper uniqueID;
	std::unique_ptr<MMRNotifierToken> mmrNotifier;
	filesystem::path dataDir;
	map<string, string> rating;
	Stats stats;

public:
	virtual void onLoad();
	virtual void onUnload();

	void Bootstrap();
	void HandleStatEvent(ServerWrapper caller, void* args);
	void HandleStatTickerEvent(ServerWrapper caller, void* args);
	void HandleGameStart();
	void HandleGameEnd();
	map<string, string> GetMMR(UniqueIDWrapper id);

	GameSettingPlaylistWrapper GetCurrentPlaylist();
	PriWrapper GetPrimaryPlayer();
	bool IsPrimaryPlayer(PriWrapper target);
	bool InValidGame();

	void Stringify();
	void HandleDemolition(PriWrapper actor, PriWrapper victim);
	void ResetRating();
};