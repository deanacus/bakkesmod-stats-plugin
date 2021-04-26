#include "PlayerStats.h"

PlayerStats::PlayerStats() {}

PlaylistRating PlayerStats::GetPlaylistRating(int playlistID) {
  if (playlistMap.find(playlistID) == playlistMap.end()) {
    return;
  }
  return playlistMap[playlistID];
}

void PlayerStats::UpdatePlaylistRating(int playlistID) {
    if (playlistMap.find(playlistID) == playlistMap.end()) {
        return;
    }
    float mmrValue = gameWrapper->GetMMRWrapper().GetPlayerMMR(this->player, playlistID);

    SkillRank rankWrapper = gameWrapper->GetMMRWrapper().GetPlayerRank(this->player, playlistID);

    int rank = rankWrapper.Tier;
    int division = rankWrapper.Division;
    int playlistGamesPlayed = rankWrapper.MatchesPlayed;
}

void PlayerStats::SetPlayer(PriWrapper player) {
    this->player = player.GetUniqueIdWrapper();
    this->playerID = player.GetUniqueIdWrapper().GetIdString();
}
