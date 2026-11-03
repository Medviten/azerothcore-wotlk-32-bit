/*
 * This file is part of the AzerothCore Project. See AUTHORS file for Copyright information
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#include "RaceMgr.h"
#include "AccountMgr.h"
#include "DatabaseEnv.h"
#include "Log.h"
#include "ObjectMgr.h"
#include "Player.h"
#include "Util.h"
#include "WorldPacket.h"
#include "WorldSession.h"

uint8  RaceMgr::_maxRaces = 0;

uint32 RaceMgr::_playableRaceMask = 0;
uint32 RaceMgr::_allianceRaceMask = 0;
uint32 RaceMgr::_hordeRaceMask = 0;
std::unordered_map<uint8, RaceMgr::RaceTemplate> RaceMgr::_raceTemplates;

RaceMgr::RaceMgr()
{
}

RaceMgr::~RaceMgr()
{
}

RaceMgr* RaceMgr::instance()
{
    static RaceMgr instance;
    return &instance;
}

void RaceMgr::LoadRaces()
{
    SetMaxRaces(0 + 1);
    _playableRaceMask = 0;
    _hordeRaceMask    = 0;
    _allianceRaceMask = 0;

    for (auto const& raceEntry : sChrRacesStore)
    {
        if (!raceEntry)
            continue;

        uint8 alliance = raceEntry->alliance;
        uint8 raceId = raceEntry->RaceID;

        if (raceEntry->Flags & CHRRACES_FLAGS_NOT_PLAYABLE)
            continue;

        if (GetMaxRaces() <= raceId)
            SetMaxRaces(raceId + 1);

        uint32 raceBit = (1 << (raceId - 1));

        _playableRaceMask |= raceBit;

        if (alliance == ALLIANCE_HORDE)
            _hordeRaceMask |= raceBit;
        else if (alliance == ALLIANCE_ALLIANCE)
            _allianceRaceMask |= raceBit;
    }

    LoadRaceTemplates();
}

void RaceMgr::LoadRaceTemplates()
{
    _raceTemplates.clear();

    QueryResult result = WorldDatabase.Query("SELECT race, home_race, kin_race, shapeshift_skin_color FROM player_race_template");
    if (!result)
    {
        LOG_INFO("server.loading", ">> Loaded 0 race templates. DB table `player_race_template` is empty.");
        return;
    }

    do
    {
        Field* fields = result->Fetch();
        _raceTemplates[fields[0].Get<uint8>()] = { fields[1].Get<uint8>(), fields[2].Get<uint8>(), fields[3].Get<uint8>() != 0 };
    } while (result->NextRow());

    LOG_INFO("server.loading", ">> Loaded {} race templates", _raceTemplates.size());
}

uint8 RaceMgr::GetHomeRace(uint8 race)
{
    auto itr = _raceTemplates.find(race);
    return itr != _raceTemplates.end() ? itr->second.homeRace : race;
}

uint8 RaceMgr::GetKinRace(uint8 race)
{
    auto itr = _raceTemplates.find(race);
    return itr != _raceTemplates.end() ? itr->second.kinRace : race;
}

uint8 RaceMgr::GetLookalikeRace(uint8 race)
{
    uint8 const kin = GetKinRace(race);
    return kin && kin != uint8(DisplayRace::Goblin) ? kin : GetHomeRace(race);
}

bool RaceMgr::UsesSkinColorForms(uint8 race)
{
    auto itr = _raceTemplates.find(race);
    return itr != _raceTemplates.end() && itr->second.shapeshiftSkinColor;
}
