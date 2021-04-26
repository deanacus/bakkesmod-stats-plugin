#include "Stats.h"

Stats::Stats() {
    stats["aerialGoals"] = 0;
    stats["aerialHits"] = 0;
    stats["backwardsGoals"] = 0;
    stats["ballTouches"] = 0;
    stats["bicycleGoals"] = 0;
    stats["bicycleHits"] = 0;
    stats["boostPickups"] = 0;
    stats["centers"] = 0;
    stats["clears"] = 0;
    stats["damages"] = 0;
    stats["epicSaves"] = 0;
    stats["exterms"] = 0;
    stats["firstTouches"] = 0;
    stats["hatTricks"] = 0;
    stats["highFives"] = 0;
    stats["longGoals"] = 0;
    stats["lowFives"] = 0;
    stats["overtimeGoals"] = 0;
    stats["playmakers"] = 0;
    stats["poolShots"] = 0;
    stats["saves"] = 0;
    stats["saviors"] = 0;
    stats["swishes"] = 0;
    stats["turtleGoals"] = 0;
    stats["ultraDamages"] = 0;
}

void Stats::SetPlayer(PriWrapper player) {
    this->player = player.GetUniqueIdWrapper().GetIdString();
}

std::string Stats::ToJSON() {
    std::string statsString;
    StatMap::iterator i;
    for (i = stats.begin(); i != stats.end(); i++) {
        statsString += "\"" + i->first + "\": " + std::to_string(i->second) + ",\n";
    }
    statsString.pop_back();
    statsString.pop_back();
    return "{" + statsString + "}";
}

void Stats::Increment(std::string statKey) {
    if (stats.find(statKey) == stats.end()) {
        return;
    }

    stats[statKey] += 1;
}

void Stats::OnStatEvent(void* args) {
    auto EvtArgs = (StatEvent*)args;
    PriWrapper actor = PriWrapper(EvtArgs->Actor);
    StatEventWrapper statEvent = StatEventWrapper(EvtArgs->StatEvent);

    std::string statLabel = statEvent.GetLabel().ToString();
    auto statDictItem = statLabelDict.find(statLabel);

    if (statDictItem != statLabelDict.end()) {
        this->Increment(statDictItem->second);
    }
}

void Stats::OnStatTickerEvent(void* args) {
    auto EvtArgs = (StatHUDEvent*)args;
    PriWrapper actor = PriWrapper(EvtArgs->Actor);
    PriWrapper victim = PriWrapper(EvtArgs->Victim);
    StatEventWrapper statEvent = StatEventWrapper(EvtArgs->StatEvent);

    std::string statLabel = statEvent.GetLabel().ToString();
    auto statDictItem = statLabelDict.find(statLabel);

    /*if (statLabel == "Demolition") {
        this->HandleDemolition(actor, victim);
    }*/

    if (statLabel != "Demolition" && statDictItem != statLabelDict.end()) {
        this->Increment(statDictItem->second);
    }
}

bool Stats::IsPlayer(PriWrapper target) {
    UniqueIDWrapper targetID = target.GetUniqueIdWrapper();
    return targetID.GetIdString() == this->player;
}

void Stats::HandleDemolition(PriWrapper actor, PriWrapper victim) {
    if (victim.IsNull()) {
        return;
    }

    if (this->IsPlayer(victim)) {
        this->Increment("deaths");
    }

    if (this->IsPlayer(actor)) {
        this->Increment("demolitions");
    }
}