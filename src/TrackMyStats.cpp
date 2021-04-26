#include "TrackMyStats.h"
#include <map>

using namespace std;

BAKKESMOD_PLUGIN(TrackMyStats, "TrackMyStats", "0.1.0", 0);

// Initialise StatMap
map<string, string> rating;

// Bakkesmode onLoad callback
void TrackMyStats::onLoad() {
  cvarManager->log("TrackMyStats Loaded");
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

void TrackMyStats::HandleStatTickerEvent(ServerWrapper caller, void *args) {
  if(!this->InValidGame()) {
    return;
  }

  stats.OnStatTickerEvent(args);
}

void TrackMyStats::HandleStatEvent(ServerWrapper caller, void *args) {
  if(!this->InValidGame()) {
    return;
  }

  stats.OnStatEvent(args);
}

// Called when a game is started or joined
void TrackMyStats::HandleGameStart() {
  if(!this->InValidGame()) {
    return;
  }

  this->stats = Stats();
  this->stats.SetPlayer(GetPrimaryPlayer());

  map<string, string> startRating = this->GetMMR(uniqueID);
  rating["startingMMR"] = startRating["mmr"];
  rating["startingRank"] = startRating["rank"];
  rating["startingDivision"] = startRating["division"];
}

// called when a match ends
void TrackMyStats::HandleGameEnd() {
  if(!this->InValidGame()) {
    return;
  }
  map<string, string> startRating = this->GetMMR(uniqueID);
  rating["startingMMR"] = startRating["mmr"];
  rating["startingRank"] = startRating["rank"];
  rating["startingDivision"] = startRating["division"];
  this->Stringify();
}

// MMR getter
map<string, string> TrackMyStats::GetMMR(UniqueIDWrapper id) {
  if (InValidGame() && id.GetUID() == uniqueID.GetUID()) {

    int playlistID = GetCurrentPlaylist().GetPlaylistId();
    float mmrValue = gameWrapper->GetMMRWrapper().GetPlayerMMR(id, playlistID);

    SkillRank rankWrapper = gameWrapper->GetMMRWrapper().GetPlayerRank(id, playlistID);

    int rank = rankWrapper.Tier;
    int division = rankWrapper.Division;
    int playlistGamesPlayed = rankWrapper.MatchesPlayed;

    map<string, string> output;
    output["mmr"] = to_string(mmrValue);
    output["rank"] = to_string(rank);
    output["division"] = to_string(division);

    return output;
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
    string ratingString;
    map<string, string>::iterator i;
    for (i = rating.begin(); i != rating.end(); i++) {
        ratingString += "\"" + i->first + "\": " + i->second  + ",\n";
    }
    ratingString.pop_back();
    ratingString.pop_back();
    ratingString = "\"rating\": {" + ratingString + "}";

    string statsString = "\"rating\": {" + this->stats.ToJSON() + "}";

    output = "{ " + ratingString + ",\n" + statsString + "}";
}

void TrackMyStats::ResetRating() {
    rating.clear();
}