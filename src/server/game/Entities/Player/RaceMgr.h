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

#ifndef __ACORE_RACEMGR_H
#define __ACORE_RACEMGR_H

#include "DatabaseEnv.h"
#include "ObjectGuid.h"
#include "SharedDefines.h"
#include <map>
#include <unordered_map>

class Player;
class WorldPacket;

class RaceMgr
{
private:
    RaceMgr();
    ~RaceMgr();

public:
    static RaceMgr* instance();

    static void LoadRaces();
    static uint8 GetMaxRaces() { return _maxRaces; }
    static void SetMaxRaces(uint8 max) { _maxRaces = max; }
    static uint32 GetPlayableRaceMask() { return _playableRaceMask; }
    static uint32 GetAllianceRaceMask() { return _allianceRaceMask; }
    static uint32 GetHordeRaceMask() { return _hordeRaceMask; }

    /// Loads player_race_template, which the race modpaks fill, so adding a race needs no code.
    static void LoadRaceTemplates();
    /// The stock race whose start area a race shares. A race without a template is its own home.
    static uint8 GetHomeRace(uint8 race);
    /// The race whose looks and language a race shares, or 0 for none. A race without a template is its own kin.
    static uint8 GetKinRace(uint8 race);
    /// The stock race a race can pass for: its kin, or its home if its kin is none or has no stock player model.
    static uint8 GetLookalikeRace(uint8 race);
    /// Whether a race's shapeshift forms follow skin (fur) color instead of hair color.
    static bool UsesSkinColorForms(uint8 race);
private:
    struct RaceTemplate
    {
        uint8 homeRace;
        uint8 kinRace;
        bool shapeshiftSkinColor;
    };

    static uint8 _maxRaces; // Max playable race + 1

    static uint32 _playableRaceMask;
    static uint32 _allianceRaceMask;
    static uint32 _hordeRaceMask;
    static std::unordered_map<uint8, RaceTemplate> _raceTemplates;
};

#define sRaceMgr RaceMgr::instance()

#endif
