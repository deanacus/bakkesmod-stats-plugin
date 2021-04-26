#pragma once

#include <map>
#include <string>
#include "bakkesmod/plugin/bakkesmodplugin.h"

struct PlaylistRating {
	float mmr;
	int rank;
	int division;
	int games;
};

enum Playlist {
  ONES       = 10,
  TWOS       = 11,
  THREES     = 13,
  HOOPS      = 27,
  RUMBLE     = 28,
  DROPSHOT   = 29,
  SNOWDAY    = 30,
  TOURNAMENT = 34
};

std::map<int, std::string> rankDict{
	{ 22, "SuperSonicLegend" },
	{ 21, "GrandChamp3" },
	{ 20, "GrandChamp2" },
	{ 19, "GrandChamp1" },
	{ 18, "Champ3" },
	{ 17, "Champ2" },
	{ 16, "Champ1" },
	{ 15, "Diamond3" },
	{ 14, "Diamond2" },
	{ 13, "Diamond1" },
	{ 12, "Platinum3" },
	{ 11, "Platinum2" },
	{ 10, "Platinum1" },
	{ 9, "Gold3" },
	{ 8, "Gold2" },
	{ 7, "Gold1" },
	{ 6, "Silver3" },
	{ 5, "Silver2" },
	{ 4, "Silver1" },
	{ 3, "Bronze3" },
	{ 2, "Bronze2" },
	{ 1, "Bronze1" },
	{ 0, "Unranked" },
};

class PlayerStats
{
private:
	UniqueIDWrapper player;
	std::string playerID;
	PlaylistRating ones;
	PlaylistRating twos;
	PlaylistRating threes;
	PlaylistRating rumble;
	PlaylistRating dropshot;
	PlaylistRating hoops;
	PlaylistRating snowday;
	PlaylistRating tournament;
	PlaylistRating casual;

	std::map<int, PlaylistRating> playlistMap {
		{10, ones},
		{11, twos},
		{13, threes},
		{27, hoops},
		{28, rumble},
		{29, dropshot},
		{30, snowday},
		{34, tournament},
	};

public:
	PlaylistRating GetPlaylistRating(int playlistID);
	void UpdatePlaylistRating(int PlaylistID);
	void SetPlayer(PriWrapper player);
	PlayerStats();
};

