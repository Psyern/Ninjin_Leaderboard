# Ninjins LeaderBoard — DayZ Mod

[![DayZ](https://img.shields.io/badge/DayZ-1.29+-blue)](https://dayz.com)
[![Enforce Script](https://img.shields.io/badge/Enforce-Script-orange)](https://community.bistudio.com/wiki/DayZ:Enforce_Script_Syntax)
[![Maintainer](https://img.shields.io/badge/Maintainer-Psyern-green)](https://github.com/Psyern)

> **A complete PvE & PvP leaderboard and kill-tracking system for DayZ Standalone servers.**

**User Guides:** [English](docs/GUIDE_EN.md) | [Deutsch](docs/GUIDE_DE.md)

---

## Important Notice — Maintainer Change

> With **DayZ Update 1.29**, **Ninjin** has retired from modding.
> Maintenance and further development of this mod is now taken over by **Psyern**.
>
> Many thanks to Ninjin for the original development and the foundation of this project!

---

## Table of Contents

- [Features](#features)
- [Dependencies](#dependencies)
- [Installation](#installation)
- [Configuration](#configuration)
  - [Main Configuration (LeaderBoardConfig.json)](#main-configuration-leaderboardconfigjson)
  - [PvE Categories (PVE\_Categories.json)](#pve-categories-pve_categoriesjson)
  - [PvP Categories (PVP\_Categories.json)](#pvp-categories-pvp_categoriesjson)
  - [Reward System (TrackingModRewardConfig.json)](#reward-system-trackingmodrewardconfigjson)
  - [Logger Configuration](#logger-configuration)
- [Key Bindings](#key-bindings)
- [Project Structure](#project-structure)
- [Technical Details](#technical-details)
  - [Script Modules](#script-modules)
  - [Kill Tracking System](#kill-tracking-system)
  - [Points System](#points-system)
  - [Reward/Milestone System](#rewardmilestone-system)
  - [Zone Integration (NinjinsPvPPvE)](#zone-integration-ninjinspvppve)
  - [Data Storage](#data-storage)
  - [RPC Communication](#rpc-communication)
  - [UI System (MVC)](#ui-system-mvc)
- [Server-Side Files](#server-side-files)
- [Optional Mod Compatibility](#optional-mod-compatibility)
- [Troubleshooting](#troubleshooting)
- [Credits](#credits)
- [License](#license)

---

## Features

### Leaderboard
- **Separate PvE and PvP leaderboards** — each independently toggleable
- **Category-based kill tracking** — Zombies, Wolves, Animals, Players, AI, and more
- **Points system** with configurable base points and multipliers per category
- **Death penalty points** — configurable for PvE and PvP
- **Online status display** — shows which players are currently online
- **Survivor type icons** — male/female survivor icons in the leaderboard
- **Paginated navigation** — paginated leaderboard with client-side page caching
- **Rate limiting** — server-side request throttling (500ms) to prevent spam

### Kill Tracking
- **Player kills (PvP)** — including kill range tracking
- **Zombie kills** — all infected
- **Animal kills** — wolves, bears, and other animals
- **AI kills** — Expansion AI (eAI) support
- **Grenade kills** — thrower tracking via `DayZPlayerImplementThrowing`
- **Explosive kills** — placer tracking (mines, C4, etc.)
- **Trap kills** — tracking via `TrapBase`
- **Vehicle kills** — roadkills
- **Kill cause detection** — automatic classification (weapon, unarmed, grenade, trap, etc.)

### Reward System
- **Milestone-based rewards** — configurable milestones per category
- **Loot tables** — JSON-based reward configurations with spawn chances
- **Item attachments** — recursive attachments with individual spawn chances
- **Quantity/health control** — min/max values for ammo, energy, condition
- **Currency rewards** — optional currency rewards

### Zone Integration
- **NinjinsPvPPvE compatibility** — full integration with the PvP/PvE zone system
- **Zone-based kill tracking** — track kills only in specific zone types
- **Per-zone kill exclusions** — exclude certain kill types in specific zones
- **Zone name matching** — wildcard matching for zone names (e.g. `FlagPvPZone_*`)

---

## Dependencies

| Mod | Required | Note |
|-----|:---:|---------|
| **DayZ Standalone** | ✅ | Version 1.29+ |
| **DZ_Data** (Vanilla) | ✅ | Base addon |
| **[Community Framework (CF)](https://steamcommunity.com/sharedfiles/filedetails/?id=1559212036)** | ✅ | For RPC manager, JSON loading, directory operations |
| **[NinjinsPvPPvE](https://github.com/Psyern)** | ❌ | Optional — for zone integration (PvP/PvE/SafeZone/Raid zones) |
| **[DayZ Expansion AI (eAI)](https://steamcommunity.com/sharedfiles/filedetails/?id=2792982069)** | ❌ | Optional — for AI kill tracking |

---

## Installation

### Server-Side Installation

1. **Copy the mod folder:**
   ```
   Ninjins_LeaderBoard/
   ```
   into the DayZ server root directory.

2. **Extend server startup parameters:**
   ```
   -mod=@CF;@Ninjins_LeaderBoard
   ```
   > If NinjinsPvPPvE is used:
   > ```
   > -mod=@CF;@NinjinsPvPPvE;@Ninjins_LeaderBoard
   > ```

3. **Start the server** — configuration files will be automatically created under `$profile:Ninjins_Tracking_Mod\Data\`.

4. **Adjust configurations** and restart the server.

### Client-Side Installation

1. **Subscribe** (Steam Workshop) or copy the mod folder into the DayZ client directory.
2. **Activate the mod** in the DayZ Launcher.

---

## Configuration

All configuration files are automatically generated on first server start at:
```
<Server Profile>/Ninjins_Tracking_Mod/Data/
```

### Main Configuration (`LeaderBoardConfig.json`)

| Parameter | Type | Default | Description |
|-----------|------|---------|-------------|
| `AdminIDs` | `string[]` | `[]` | Steam64 IDs of admins |
| `DeletePlayerFilesOlderThanDays` | `int` | `0` | Auto-cleanup of inactive players (0 = disabled) |
| `UseUTCForDates` | `bool` | `false` | Use UTC timezone for date stamps |
| `EnableRewardSystem` | `bool` | `false` | Enable milestone reward system |
| `DisablePVPLeaderboard` | `bool` | `false` | Disable PvP leaderboard |
| `DisablePVELeaderboard` | `bool` | `false` | Disable PvE leaderboard |
| `MaxPVEPlayersDisplay` | `int` | `10` | Max. players shown in PvE leaderboard (max. 100) |
| `MaxPVPPlayersDisplay` | `int` | `24` | Max. players shown in PvP leaderboard (max. 100) |
| `ShowPlayerOnlineStatusPVE` | `bool` | `true` | Show online status in PvE leaderboard |
| `ShowPlayerOnlineStatusPVP` | `bool` | `true` | Show online status in PvP leaderboard |
| `SurvivorIconPathMale` | `string` | `"...male.edds"` | Path to male survivor icon |
| `SurvivorIconPathFemale` | `string` | `"...female.edds"` | Path to female survivor icon |
| `EnableWebExport` | `bool` | `true` | Exports leaderboard data as JSON for websites |
| `WebExportFileName` | `string` | `"LeaderboardWebExport.json"` | Filename of the web export JSON under `Data/` |
| `WebExportIncludePlayerIDs` | `bool` | `false` | Include Steam64 IDs in the export |
| `WebExportMaxPlayers` | `int` | `100` | Max. number of exported players per PvE/PvP list |

#### Disabling Kill/Death Tracking

| Parameter | Description |
|-----------|-------------|
| `DisableDeathBySuicide` | Ignore deaths by suicide |
| `DisableDeathByGrenade` | Ignore deaths by grenades |
| `DisableDeathByTrap` | Ignore deaths by traps |
| `DisableDeathByZombie` | Ignore deaths by zombies |
| `DisableDeathByAnimal` | Ignore deaths by animals |
| `DisableDeathByAI` | Ignore deaths by AI |
| `DisableDeathByCar` | Ignore deaths by vehicles |
| `DisableDeathByWeapon` | Ignore deaths by weapons |
| `DisableDeathByUnarmed` | Ignore deaths by unarmed |
| `DisableDeathByUnknown` | Ignore deaths by unknown causes |
| `DisableKillByGrenade` | Ignore grenade kills |
| `DisableKillByTrap` | Ignore trap kills |
| `DisableKillByAnimal` | Ignore animal kills |
| `DisableKillByZombie` | Ignore zombie kills |
| `DisableKillByCar` | Ignore vehicle kills |
| `DisableKillByWeapon` | Ignore weapon kills |
| `DisableKillByUnarmed` | Ignore unarmed kills |
| `DisableKillByUnknown` | Ignore unknown kills |
| `DisableKillByUnconsciousSuicide` | Ignore unconscious suicide kills |

#### Zone Configuration

| Parameter | Type | Description |
|-----------|------|-------------|
| `TrackPVEKillsInZoneTypes` | `string[]` | Which zone types to track PvE kills in (`"everywhere"`, `"pvp"`, `"pve"`, `"visual"`, `"raid"`) |
| `ExcludePVPKillWhenBothInPVE` | `bool` | Ignore PvP kills when both players are in a PvE zone |
| `EnableZoneKillExclusions` | `bool` | Enable zone-based kill exclusions |
| `ExcludeKillsInZones` | `array` | List of exclusion rules (see below) |

**Example zone exclusions:**
```json
{
    "ExcludeKillsInZones": [
        {
            "ZoneType": "safezone",
            "ExcludedClassNames": ["PlayerBase", "AnimalBase", "ZombieBase", "eAIBase"],
            "ZoneNames": []
        },
        {
            "ZoneType": "pvp",
            "ExcludedClassNames": ["PlayerBase"],
            "ZoneNames": ["FlagPvPZone_", "KOTHZone_"]
        }
    ]
}
```

#### Web Export

When `EnableWebExport` is active, the server automatically writes a JSON file after changes and on startup to:

```
$profile:Ninjins_Tracking_Mod/Data/LeaderboardWebExport.json
```

This file can be read by a website, API proxy, or a cron/sync job.

**Example:**
```json
{
    "generatedAt": "2026-03-16 03:10:00",
    "playerOnlineCounter": 12,
    "totalPlayers": 245,
    "includePlayerIDs": false,
    "exportPlayerLimit": 100,
    "topPVEPlayers": [
        {
            "playerName": "Psyern",
            "pvePoints": 5120,
            "pvpPoints": 320,
            "pveDeaths": 4,
            "pvpDeaths": 1,
            "isOnline": 1,
            "lastLoginDate": "2026-03-16 03:09:22"
        }
    ],
    "topPVPPlayers": []
}
```

---

### PvE Categories (`PVE_Categories.json`)

Defines the kill categories for the PvE leaderboard.

```json
{
    "Categories": [
        {
            "CategoryID": "Zombies",
            "ClassNamePreview": "ZmbM_CitizenASkinny",
            "ClassNames": ["ZombieBase:10:1"]
        },
        {
            "CategoryID": "Wolfes",
            "ClassNamePreview": "Animal_CanisLupus_Grey",
            "ClassNames": ["Animal_CanisLupus:25:1"]
        }
    ],
    "PVEDeathPenaltyPoints": 5
}
```

**ClassNames format:** `"ClassName:BasePoints:Multiplier"`
- `ClassName` — DayZ class name (supports base class matching)
- `BasePoints` — points per kill
- `Multiplier` — multiplier (default: 1)

**Matching logic:**
1. Exact match: `className == categoryClassName`
2. Prefix match: `eAI_*` for all eAI classes
3. Underscore prefix: `Animal_CanisLupus_*`
4. Entity IsKindOf check (runtime)
5. CfgVehicles config hierarchy check

---

### PvP Categories (`PVP_Categories.json`)

```json
{
    "Categories": [
        {
            "CategoryID": "Players",
            "DisplayName": "Players",
            "ClassNames": ["PlayerBase:100:1"]
        }
    ],
    "PVPDeathPenaltyPoints": 10
}
```

---

### Reward System (`TrackingModRewardConfig.json`)

```json
{
    "EnableMilestoneRewards": true,
    "RewardConfigFolder": "$profile:Ninjins_Tracking_Mod\\Data\\RewardSystem\\",
    "PVEMilestones": {
        "Wolfes": [100, 200, 300, 500, 1000],
        "Zombies": [50, 100, 200, 500, 1000]
    },
    "PVPMilestones": {
        "Players": [10, 25, 50, 100, 200]
    },
    "PVERewardConfigs": {
        "Wolfes": [
            { "Milestone": "100", "FileName": "Wolfes100.json" },
            { "Milestone": "200", "FileName": "Wolfes200.json" }
        ]
    },
    "PVPRewardConfigs": {
        "Players": [
            { "Milestone": "10", "FileName": "Players10.json" }
        ]
    }
}
```

**Reward file structure** (e.g. `Wolfes100.json`):
```json
{
    "RewardItems": [
        {
            "Name": "Wolf Hunter",
            "SpawnChance": 100.0,
            "ItemsMin": 1,
            "ItemsMax": 1,
            "LootItems": [
                {
                    "ItemClassName": "AKM",
                    "SpawnChance": 100.0,
                    "Amount": 0,
                    "QuantMin": 0.0,
                    "QuantMax": 0.0,
                    "HealthMin": 0.7,
                    "HealthMax": 1.0,
                    "Attachments": [
                        {
                            "ItemClassName": "Mag_AKM_30Rnd",
                            "SpawnChance": 100.0,
                            "Amount": 30,
                            "HealthMin": 0.8,
                            "HealthMax": 1.0
                        }
                    ]
                }
            ]
        }
    ],
    "CurrencyRewards": [
        {
            "ClassName": "TraderPlus_Money_Dollar1",
            "SpawnChance": 100.0,
            "Amount": 1000
        }
    ],
    "CurrencyMin": 1,
    "CurrencyMax": 2
}
```

---

### Logger Configuration

#### Server Logger (`$profile:Ninjins_Tracking_Mod/LoggerConfig.json`)
```json
{
    "Level": 4
}
```

#### Client Logger (`$profile:Ninjins_LeaderBoard/LoggerConfig.json`)
```json
{
    "Level": 6
}
```

**Log levels:**
| Level | Description |
|-------|-------------|
| 0 | Off |
| 1 | Critical |
| 2 | Error |
| 3 | Warning |
| 4 | Info (server default) |
| 5 | Debug |
| 6 | Trace (client default) |

---

## Key Bindings

| Key | Action |
|-----|--------|
| **F5** | Open/close PvE leaderboard |
| **F6** | Open/close PvP leaderboard |

Key bindings can be changed in-game under **Options → Controls → Tracking Mod**.

---

## Project Structure

```
Ninjins_LeaderBoard/
├── config.cpp                          # Mod definition (CfgPatches, CfgMods)
├── stringtable.csv                     # Localization (EN)
│
├── data/
│   └── modded_inputs.xml               # Key bindings (F5=PvE, F6=PvP)
│
├── gui/
│   ├── icons/
│   │   └── survivor/
│   │       ├── male.edds / male.png    # Male survivor icon
│   │       └── female.png              # Female survivor icon
│   └── layouts/
│       ├── PvE/
│       │   ├── trackingModLeaderboard_mvc.layout
│       │   ├── trackingModPlayerEntry_mvc.layout
│       │   └── trackingModKillCategory_mvc.layout
│       └── PvP/
│           ├── trackingModPvPLeaderboard_mvc.layout
│           └── trackingModPvPPlayerEntry_mvc.layout
│
├── scripts/
│   ├── 3_Game/                         # Game script module
│   │   ├── General Configs/
│   │   │   ├── Config/
│   │   │   │   ├── TrackingModConfig.c         # Main configuration
│   │   │   │   ├── TrackingModRewardConfig.c   # Reward configuration
│   │   │   │   ├── PVECategoryConfig.c         # PvE categories
│   │   │   │   ├── PVPCategoryConfig.c         # PvP categories
│   │   │   │   └── DeathCategoryConfig.c       # Death categories
│   │   │   ├── Data/
│   │   │   │   ├── Ninjins_Tracking_Mod_Data.c # Player data (PlayerDeathData)
│   │   │   │   └── TrackingModLeaderboardData.c# Leaderboard data structures
│   │   │   ├── Rewards/
│   │   │   │   ├── TrackingModMilestoneHelper.c      # Milestone calculation
│   │   │   │   └── TrackingModMilestoneRewardLoader.c # Reward file loading
│   │   │   └── Utils/
│   │   │       ├── TrackingModLogger.c         # Server logger (TrackingMod)
│   │   │       └── TrackingModZoneUtils.c      # Zone utility functions
│   │   └── TrackingModUI/
│   │       ├── TrackingModLeaderboardData.c    # Shared data classes (client/server)
│   │       └── TrackingModLogger.c             # Client logger (TrackingModUI)
│   │
│   ├── 4_World/                        # World script module
│   │   ├── TrackingModRewardHelper.c   # Reward distribution & item spawning
│   │   ├── Zone Related/
│   │   │   └── TrackingModZoneHandler.c# Zone event callbacks
│   │   └── entities/
│   │       ├── creatures/
│   │       │   ├── TrackingMod_EntityHelper.c  # Entity matching helper
│   │       │   ├── ai/
│   │       │   │   └── eAIBase.c               # Expansion AI kill tracking
│   │       │   ├── animals/
│   │       │   │   └── AnimalBase.c            # Animal kill tracking
│   │       │   └── infected/
│   │       │       └── ZombieBase.c            # Zombie kill tracking
│   │       ├── itembase/
│   │       │   ├── ExplosivesBase.c            # Explosive placer tracking
│   │       │   ├── Grenade_Base.c              # Grenade thrower tracking
│   │       │   └── TrapBase.c                  # Trap placer tracking
│   │       ├── manbase/
│   │       │   └── PlayerBase.c                # Player events & kill tracking
│   │       └── DayZPlayerImplementThrowing.c   # Grenade thrower assignment
│   │
│   └── 5_Mission/                      # Mission script module
│       ├── MissionServer.c             # Server logic, RPC handler, init
│       └── TrackingModUI/
│           ├── TrackingModLeaderboardInputHandler.c  # Input & RPC client
│           ├── TrackingModKillCategory.c             # Kill category UI widget
│           ├── TrackingModKillCategoryController.c   # Kill category controller
│           ├── PvE/
│           │   ├── TrackingModLeaderboardMenu.c      # PvE leaderboard menu
│           │   ├── TrackingModLeaderboardController.c # PvE controller
│           │   ├── TrackingModPlayerEntry.c          # PvE player entry
│           │   └── TrackingModPlayerEntryController.c # PvE entry controller
│           └── PvP/
│               ├── TrackingModPvPLeaderboardMenu.c    # PvP leaderboard menu
│               ├── TrackingModPvPLeaderboardController.c # PvP controller
│               ├── TrackingModPvPPlayerEntry.c        # PvP player entry
│               └── TrackingModPvPPlayerEntryController.c # PvP entry controller
│
└── todo/
    └── Class_Renaming_Analysis.md      # Analysis for class renaming
```

---

## Technical Details

### Script Modules

The mod uses the DayZ Enforce Script system with three modules:

| Module | Path | Description |
|--------|------|-------------|
| **3_Game** | `scripts/3_Game/` | Configurations, data structures, logger, utilities |
| **4_World** | `scripts/4_World/` | Entity modding, kill tracking, zone handler, rewards |
| **5_Mission** | `scripts/5_Mission/` | Server init, RPC handler, UI/menus, input handler |

### Kill Tracking System

Kill tracking works via **modded DayZ base classes**:

| Class | Tracking Method | Kill Type |
|-------|----------------|-----------|
| `PlayerBase` | `EEHitBy()`, `EEKilled()` | PvP kills, deaths |
| `ZombieBase` | `EEKilled()` | Zombie kills |
| `AnimalBase` | `EEKilled()`, `EEOnDamageCalculated()` | Animal kills |
| `eAIBase` | `EEKilled()`, `EEOnDamageCalculated()` | AI kills |
| `Grenade_Base` | `EEOnDamageCalculated()` | Grenade attribution |
| `ExplosivesBase` | `OnPlacementComplete()` | Explosive attribution |
| `TrapBase` | `OnPlacementComplete()` | Trap attribution |
| `DayZPlayerImplementThrowing` | `HandleThrowing()` | Thrower attribution |

**Kill Causes (KillCause):**

| ID | Cause |
|----|-------|
| 0 | Grenade |
| 1 | Trap |
| 2 | Animal |
| 3 | Zombie |
| 4 | Vehicle |
| 5 | Weapon |
| 6 | Unarmed |
| 7 | Unknown |
| 8 | Unconscious suicide |

### Points System

- **PvE points:** `Σ (KillsPerCategory × BasePoints × Multiplier) - (Deaths × DeathPenaltyPoints)`
- **PvP points:** `Σ (KillsPerCategory × BasePoints × Multiplier) - (Deaths × PVPDeathPenaltyPoints)`
- Points cannot fall below 0
- BasePoints and Multiplier are defined via the `ClassName:BasePoints:Multiplier` format in the category config

### Reward/Milestone System

1. **Milestones** are defined per category (e.g. Wolfes: 100, 200, 300 kills)
2. When a player reaches a milestone, it is saved in `AvailableMilestones`
3. Players can **claim** rewards via the leaderboard UI
4. Rewards are loaded from JSON files and added to the player's inventory
5. **Loot sections** have individual spawn chances and ItemsMin/Max
6. **Attachments** are processed recursively (nested attachments possible)
7. **Item properties** (health, quantity, ammo, energy) are configurable

### Zone Integration (NinjinsPvPPvE)

With the NinjinsPvPPvE mod enabled (`#ifdef NinjinsPvPPvE`):

- **`TrackingModZoneHandler`** — registers as `PlayerStateChangeCallback` and `ZoneEventCallback`
- **`TrackingModZoneUtils`** — manages player zone status in memory maps
- **Zone types:** `pvp`, `pve`, `visual`, `raid`, `safezone`
- **Kill exclusions** can be configured per zone
- **Zone name matching** supports wildcard prefixes (e.g. `FlagPvPZone_`)

### Data Storage

```
$profile:Ninjins_Tracking_Mod/
├── Data/
│   ├── LeaderBoardConfig.json          # Main configuration
│   ├── PVE_Categories.json             # PvE categories
│   ├── PVP_Categories.json             # PvP categories
│   ├── TrackingModRewardConfig.json    # Reward configuration
│   ├── LeaderboardWebExport.json       # Web export for external websites/API bridge
│   ├── Players/
│   │   ├── <SteamID64>.json            # Individual player data
│   │   └── ...
│   └── RewardSystem/
│       ├── Wolfes100.json              # Reward files
│       ├── Players10.json
│       └── ...
├── Logging/
│   └── Logs/
│       └── TrackingMod_<Date>.log      # Server logs
└── LoggerConfig.json                   # Server logger config
```

**Player data (`<SteamID64>.json`):**
```json
{
    "PlayerID": "76561198...",
    "PlayerName": "PlayerName",
    "LastDeathPosition": [1234.5, 100.0, 5678.9],
    "LastKillerName": "Unknown",
    "LastKillerType": "Unknown",
    "CategoryKills": { "Zombies": 150, "Wolfes": 42 },
    "CategoryLongestRanges": { "Players": 350 },
    "CategoryDeaths": { "Players": 5 },
    "PvEPoints": 2520,
    "PvPPoints": 1800,
    "playerIsOnline": 0,
    "survivorType": "Male",
    "LastLoginDate": "2026-03-07",
    "AvailableMilestones": {},
    "ClaimedMilestones": { "Zombies": [50, 100] }
}
```

### RPC Communication

The mod uses the **Community Framework RPC Manager** for client-server communication:

| RPC Name | Direction | Description |
|----------|-----------|-------------|
| `RequestTrackingModLeaderboard` | Client → Server | Request leaderboard data (with page number) |
| `ReceiveTrackingModLeaderboard` | Server → Client | Send leaderboard data |
| `ClaimTrackingModReward` | Client → Server | Claim milestone reward |
| `ReceiveTrackingModRewardClaim` | Server → Client | Reward claim result |
| `ReceivePlayerDataUpdate` | Server → Client | Player data update |

### Web Interface

- There is no built-in HTTP server in the mod.
- Instead, the server generates a machine-readable JSON file for external web integration.
- Recommended setup: a web server or small backend reads `LeaderboardWebExport.json` from the server profile and serves it to the website.

### UI System (MVC)

The UI is based on the **Model-View-Controller (MVC)** pattern:

- **Layouts** (`.layout`) — XML-based UI definitions in the `gui/layouts/` folder
- **Menu classes** — `ScriptViewMenu`-based menu classes
- **Controllers** — data binding via controller classes
- **Page caching** — client-side caching of leaderboard pages for fast navigation

---

## Server-Side Files

The following files/folders are automatically created on first start:

| File/Folder | Description |
|-------------|-------------|
| `$profile:Ninjins_Tracking_Mod/` | Root folder |
| `Data/LeaderBoardConfig.json` | Main configuration |
| `Data/PVE_Categories.json` | PvE categories (default: Zombies, Wolfes) |
| `Data/PVP_Categories.json` | PvP categories (default: Players) |
| `Data/TrackingModRewardConfig.json` | Reward system configuration |
| `Data/Players/` | Player data files |
| `Data/RewardSystem/` | Reward configuration files |
| `Logging/Logs/` | Log files |

---

## Optional Mod Compatibility

### DayZ Expansion AI (`#ifdef EXPANSIONMODAI`)
- Automatic tracking of eAI kills
- `eAIBase` class is modded for damage source tracking
- eAI classes are recognized via the `eAI_` prefix

### NinjinsPvPPvE (`#ifdef NinjinsPvPPvE`)
- Zone-based kill tracking and exclusions
- `PlayerStateChangeCallback` for PvP/PvE zone transitions
- `ZoneEventCallback` for zone entry/exit with zone names
- Kill validation based on player zone status

---

## Troubleshooting

### Leaderboard does not open
- Check whether the **Community Framework (CF)** mod is loaded
- Check the DayZ client logs for `[TrackingModUI]` entries
- Make sure the key binding is not overridden by another mod

### No kills are being tracked
- Check the server logs for `[TrackingMod]` entries
- Check whether the category configs (`PVE_Categories.json`, `PVP_Categories.json`) are correct
- Check whether kill types are not disabled in `LeaderBoardConfig.json`

### Rewards are not working
- Make sure `EnableRewardSystem` is set to `true` in the main config
- Check whether the reward JSON files exist in the `RewardSystem/` folder
- Check the server logs for `[TrackingModRewardHelper]` or `[TrackingModMilestoneRewardLoader]` entries

### Zone integration is not working
- Make sure **NinjinsPvPPvE** is loaded as a mod
- Check whether `EnableZoneKillExclusions` is set to `true`
- Check the server logs for "Zone callbacks registered with NinjinsPvPPvE"

### Increasing the log level
- Server: `$profile:Ninjins_Tracking_Mod/LoggerConfig.json` → `"Level": 6`
- Client: `$profile:Ninjins_LeaderBoard/LoggerConfig.json` → `"Level": 6`

---

## Credits

| Role | Person |
|------|--------|
| **Original Developer** | **Ninjin** |
| **Current Maintainer** | **Psyern** |

---

## License

This project is a community mod for DayZ Standalone. All rights to the game DayZ belong to Bohemia Interactive.

---

> **Questions, bugs or feature requests?** → [GitHub Issues](https://github.com/Psyern/Ninjins_Leaderboard/issues)
