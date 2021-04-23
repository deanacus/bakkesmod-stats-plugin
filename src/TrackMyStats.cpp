#include "TrackMyStats.h"
#include "Dicts.h"
#include <map>

using namespace std;

BAKKESMOD_PLUGIN(TrackMyStats, "TrackMyStats", "0.1.0", 0);

// Initialise StatMap
StatMap stats;

// Bakkesmode onLoad callback
void TrackMyStats::onLoad() {
  cvarManager->log("TrackMyStats Loaded");
  currentMMR = 100.00;
  this->Bootstrap();
}

// Bakkesmode onUnload callback
void TrackMyStats::onUnload() {
}

// Handle setting up listeners, and setup other required values
void TrackMyStats::Bootstrap() {
  uniqueID = gameWrapper->GetUniqueID();
  dataDir = gameWrapper->GetDataFolder().append("JSONStatistics");

  mmrNotifier = gameWrapper->GetMMRWrapper().RegisterMMRNotifier(std::bind(&TrackMyStats::GetMMR, this, std::placeholders::_1));
  gameWrapper->HookEventPost("Function GameEvent_Soccar_TA.WaitingForPlayers.BeginState", std::bind(&TrackMyStats::HandleGameStart, this));
  gameWrapper->HookEventPost("Function Engine.PlayerInput.InitInputSystem", std::bind(&TrackMyStats::HandleGameStart, this));
  gameWrapper->HookEventPost("Function TAGame.GameEvent_Soccar_TA.EventMatchEnded", std::bind(&TrackMyStats::HandleGameEnd, this));
  gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatTickerMessage", std::bind(&TrackMyStats::HandleStatTickerEvent, this, std::placeholders::_1, std::placeholders::_2));
  gameWrapper->HookEventWithCallerPost<ServerWrapper>("Function TAGame.GFxHUD_TA.HandleStatEvent", std::bind(&TrackMyStats::HandleStatEvent, this, std::placeholders::_1, std::placeholders::_2));
}

// Called when a stat appears in the top right of the screen
void TrackMyStats::HandleStatTickerEvent(ServerWrapper caller, void *args) {
  if(!this->InValidGame()) {
    return;
  }

  auto EvtArgs = (StatHUDEvent *)args;
  PriWrapper actor = PriWrapper(EvtArgs->Actor);
  PriWrapper victim = PriWrapper(EvtArgs->Victim);
  StatEventWrapper statEvent = StatEventWrapper(EvtArgs->StatEvent);

  string eventString = statEvent.GetLabel().ToString();
  auto statDictItem = statDict.find(eventString);

  if (eventString == "Demolition") {
    this->HandleDemolition(actor, victim);
  }

  if (eventString != "Demolition" && statDictItem != statDict.end() && this->IsPrimaryPlayer(actor)) {
    string statKey = statDictItem->second;
    IncrementStat(statKey);
  }
}

// called on all stats (I think)
void TrackMyStats::HandleStatEvent(ServerWrapper caller, void *args) {
  if(!this->InValidGame()) {
    return;
  }

  auto EvtArgs = (StatEvent *)args;
  PriWrapper actor = PriWrapper(EvtArgs->Actor);
  StatEventWrapper statEvent = StatEventWrapper(EvtArgs->StatEvent);

  string eventString = statEvent.GetLabel().ToString();
  auto statDictItem = statDict.find(eventString);

  if (statDictItem != statDict.end() && this->IsPrimaryPlayer(actor)) {
    string statKey = statDictItem->second;
    IncrementStat(statKey);
  }
}

// Called when a game is started or joined
void TrackMyStats::HandleGameStart() {
  if(!this->InValidGame()) {
    return;
  }
  this->GetMMR(uniqueID);
  stats.clear();
}

// called when a match ends
void TrackMyStats::HandleGameEnd() {
  if(!this->InValidGame()) {
    return;
  }
  this->GetMMR(uniqueID);
  this->Stringify();
}

// MMR callback (I think)
void TrackMyStats::GetMMR(UniqueIDWrapper id) {
  if (InValidGame() && id.GetUID() == uniqueID.GetUID()) {

    int playlistID = GetCurrentPlaylist().GetPlaylistId();
    float mmrValue = gameWrapper->GetMMRWrapper().GetPlayerMMR(id, playlistID);

    SkillRank rankWrapper = gameWrapper->GetMMRWrapper().GetPlayerRank(id, playlistID);
    SkillRating ratingWrapper = gameWrapper->GetMMRWrapper().GetPlayerSkillRating(id, playlistID);

    float mmrDiff = 0;
    if (currentMMR) {
        mmrDiff = currentMMR - mmrValue;
    }

    int rank = rankWrapper.Tier;
    int division = rankWrapper.Division;
    int playlistGamesPlayed = rankWrapper.MatchesPlayed;
    float mu = ratingWrapper.Mu;
    float sigma = ratingWrapper.Sigma;

    cvarManager->log("MMR: " + to_string(mmrValue));
    cvarManager->log("MMR Diff: " + to_string(mmrDiff));
    cvarManager->log("Rank: " + to_string(rank));
    cvarManager->log("Division: " + to_string(division));
    cvarManager->log("Playlist Games Played: " + to_string(playlistGamesPlayed));
    cvarManager->log("Mu (whatever that is): " + to_string(mu));
    cvarManager->log("Sigma (whatever that is): " + to_string(sigma));
  }
}

GameSettingPlaylistWrapper TrackMyStats::GetCurrentPlaylist() {
    return gameWrapper->GetOnlineGame().GetPlaylist();
}

// Utility to get the player
PriWrapper TrackMyStats::GetPrimaryPlayer() {
  ServerWrapper server = gameWrapper->GetOnlineGame();
  PlayerControllerWrapper playerController = server.GetLocalPrimaryPlayer();
  return playerController.GetPRI();
}

// Utility to check if a player is the primary player
bool TrackMyStats::IsPrimaryPlayer(PriWrapper target) {
  auto targetID = target.GetUniqueIdWrapper();
  auto primaryID = this->GetPrimaryPlayer().GetUniqueIdWrapper();
  return targetID.GetIdString() == primaryID.GetIdString();
}

// Check if this is a valid game to track stats for
bool TrackMyStats::InValidGame(){
  if(
    gameWrapper->IsInOnlineGame() &&
    !gameWrapper->IsInReplay() &&
    !gameWrapper->IsInFreeplay()
  ) {
    ServerWrapper serverWrapper = gameWrapper->GetOnlineGame();
    return serverWrapper.IsOnlineMultiplayer();
  }
  return false;
}

// Convert the stats to a JSON-ish string
void TrackMyStats::Stringify() {
    string output;
    StatMap::iterator i;
    for(i = stats.begin(); i != stats.end(); i++) {
      output += "\"" + i->first + "\": " + std::to_string(i->second) + ",\n";
    }
    output.pop_back();
    output.pop_back();
    cvarManager->log(output);
}

// Utility to increment statistics in a map
void TrackMyStats::IncrementStat(string label) {
  StatMap::iterator i = stats.find(label);
  if (i == stats.end()) {
    stats.insert(std::make_pair(label, 1));
    cvarManager->log("Created Stat: " + label);
  }
  else {
    stats[label]++;
    cvarManager->log("Incremented Stat: " + label);
  }
}

// Utility to handle demolitions, as the player can be either victim or actor
void TrackMyStats::HandleDemolition(PriWrapper actor, PriWrapper victim) {
  if(victim.IsNull()) {
    return;
  }

  if (this->IsPrimaryPlayer(victim)) {
      IncrementStat("deaths");
  }

  if (this->IsPrimaryPlayer(actor)) {
      IncrementStat("demolitions");
  }
}
