# 📖 Ninjins LeaderBoard — User Guide (English)

> This guide is aimed at **players** and **server administrators** who want to use or set up the Ninjins LeaderBoard mod.

---

## Table of Contents

- [For Players](#-for-players)
  - [How to open the Leaderboard](#how-to-open-the-leaderboard)
  - [Reading the Leaderboard](#reading-the-leaderboard)
  - [Claiming Milestone Rewards](#claiming-milestone-rewards)
  - [Changing Key Bindings](#changing-key-bindings)
- [For Server Administrators](#-for-server-administrators)
  - [Step 1 — Install the mod](#step-1--install-the-mod)
  - [Step 2 — Configure startup parameters](#step-2--configure-startup-parameters)
  - [Step 3 — First server start](#step-3--first-server-start)
  - [Step 4 — Adjust the main configuration](#step-4--adjust-the-main-configuration)
  - [Step 5 — Configure kill categories](#step-5--configure-kill-categories)
  - [Step 6 — Set up the reward system (optional)](#step-6--set-up-the-reward-system-optional)
  - [Step 7 — Set up zone integration (optional)](#step-7--set-up-zone-integration-optional)
  - [Web export for your website (optional)](#web-export-for-your-website-optional)
  - [Common problems and solutions](#common-problems-and-solutions)

---

## 🎮 For Players

### How to open the Leaderboard

| Key | Action |
|-----|--------|
| **F5** | Open / close the **PvE leaderboard** |
| **F6** | Open / close the **PvP leaderboard** |

> **Note:** The mod must be active in the DayZ Launcher and the server must have it installed. The Community Framework (CF) mod is also required.

---

### Reading the Leaderboard

The leaderboard shows all tracked players sorted by their points.

| Column | Description |
|--------|-------------|
| **Rank** | Your current position |
| **Name** | Player name (🟢 = online, ⚫ = offline) |
| **Points** | Total PvE or PvP points |
| **Kills** | Kills per category (zombies, wolves, players, etc.) |
| **Deaths** | Number of deaths |

**How are points calculated?**

- Each kill in a category awards **base points × multiplier**
- Each death deducts **death penalty points**
- Points **cannot go below 0**

**Example:** 10 zombie kills × 10 points = 100 PvE points. 1 death × 5 penalty = −5 points → **95 PvE points total**

---

### Claiming Milestone Rewards

If the server has the reward system enabled, you can claim rewards for reaching kill milestones:

1. **Open the leaderboard** (F5 or F6)
2. Look for a **"Claim Reward"** button next to your name or in your stats
3. Click it — the items will be spawned directly into your inventory
4. Each milestone can only be claimed **once**

> Milestones are set by the server admin (e.g. 100 wolf kills, 50 zombie kills, 10 player kills).

---

### Changing Key Bindings

1. Open **Options** in the DayZ main menu
2. Go to **Controls**
3. Search for the **"Tracking Mod"** section
4. Reassign **PvE Leaderboard** and **PvP Leaderboard** to your preferred keys

---

## 🛠️ For Server Administrators

### Step 1 — Install the mod

Copy the mod folder into your DayZ server root directory:

```
<DayZ Server Root>/
└── @Ninjins_LeaderBoard/
    └── Ninjins_LeaderBoard/
        ├── config.cpp
        ├── scripts/
        └── ...
```

Make sure the **Community Framework (CF)** mod is also installed:
- Steam Workshop ID: [1559212036](https://steamcommunity.com/sharedfiles/filedetails/?id=1559212036)

---

### Step 2 — Configure startup parameters

Add the mod to your server startup script:

```
-mod=@CF;@Ninjins_LeaderBoard
```

**With optional mods:**

```
# With zone system:
-mod=@CF;@NinjinsPvPPvE;@Ninjins_LeaderBoard

# With Expansion AI:
-mod=@CF;@DayZ-Expansion-AI;@Ninjins_LeaderBoard

# All combined:
-mod=@CF;@NinjinsPvPPvE;@DayZ-Expansion-AI;@Ninjins_LeaderBoard
```

> ⚠️ The order matters. CF must always come **before** Ninjins_LeaderBoard.

---

### Step 3 — First server start

Start the server once. The mod will automatically create all configuration files at:

```
<Server Profile>/Ninjins_Tracking_Mod/Data/
├── LeaderBoardConfig.json
├── PVE_Categories.json
├── PVP_Categories.json
└── TrackingModRewardConfig.json
```

Then **stop the server** and proceed with the configuration.

---

### Step 4 — Adjust the main configuration

Open `LeaderBoardConfig.json` and adjust the most important settings:

```json
{
    "AdminIDs": ["76561198XXXXXXXXX"],
    "DeletePlayerFilesOlderThanDays": 30,
    "UseUTCForDates": false,
    "EnableRewardSystem": false,
    "DisablePVPLeaderboard": false,
    "DisablePVELeaderboard": false,
    "MaxPVEPlayersDisplay": 10,
    "MaxPVPPlayersDisplay": 24,
    "ShowPlayerOnlineStatusPVE": true,
    "ShowPlayerOnlineStatusPVP": true,
    "EnableWebExport": false
}
```

**Key settings explained:**

| Setting | Recommendation |
|---------|---------------|
| `AdminIDs` | Add your Steam64 ID — admins can see additional functions |
| `DeletePlayerFilesOlderThanDays` | Set to `30` to automatically clean up inactive players |
| `DisablePVPLeaderboard` | Set to `true` on pure PvE servers |
| `DisablePVELeaderboard` | Set to `true` on pure PvP servers |
| `EnableRewardSystem` | Set to `true` if you want to use milestone rewards |

---

### Step 5 — Configure kill categories

#### PvE Categories (`PVE_Categories.json`)

Define which entities count as PvE kills and how many points they award:

```json
{
    "Categories": [
        {
            "CategoryID": "Zombies",
            "ClassNamePreview": "ZmbM_CitizenASkinny",
            "ClassNames": ["ZombieBase:10:1"]
        },
        {
            "CategoryID": "Wolves",
            "ClassNamePreview": "Animal_CanisLupus_Grey",
            "ClassNames": ["Animal_CanisLupus:25:1"]
        },
        {
            "CategoryID": "Bears",
            "ClassNamePreview": "Animal_UrsusArctos",
            "ClassNames": ["Animal_UrsusArctos:50:1"]
        }
    ],
    "PVEDeathPenaltyPoints": 5
}
```

**Format:** `"ClassName:BasePoints:Multiplier"`
- `ZombieBase:10:1` → every zombie kill = 10 points
- `Animal_CanisLupus:25:1` → every wolf kill = 25 points

#### PvP Categories (`PVP_Categories.json`)

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

### Step 6 — Set up the reward system (optional)

1. Set `"EnableRewardSystem": true` in `LeaderBoardConfig.json`
2. Open `TrackingModRewardConfig.json` and define your milestones:

```json
{
    "EnableMilestoneRewards": true,
    "RewardConfigFolder": "$profile:Ninjins_Tracking_Mod\\Data\\RewardSystem\\",
    "PVEMilestones": {
        "Zombies": [50, 100, 250, 500, 1000],
        "Wolves": [25, 50, 100, 200, 500]
    },
    "PVPMilestones": {
        "Players": [10, 25, 50, 100]
    },
    "PVERewardConfigs": {
        "Zombies": [
            { "Milestone": "50", "FileName": "Zombies50.json" },
            { "Milestone": "100", "FileName": "Zombies100.json" }
        ]
    },
    "PVPRewardConfigs": {
        "Players": [
            { "Milestone": "10", "FileName": "Players10.json" }
        ]
    }
}
```

3. Create the reward files in `Data/RewardSystem/`, e.g. `Zombies50.json`:

```json
{
    "RewardItems": [
        {
            "Name": "Zombie Slayer Starter Pack",
            "SpawnChance": 100.0,
            "ItemsMin": 1,
            "ItemsMax": 1,
            "LootItems": [
                {
                    "ItemClassName": "AKM",
                    "SpawnChance": 100.0,
                    "Amount": 0,
                    "HealthMin": 0.8,
                    "HealthMax": 1.0,
                    "Attachments": [
                        {
                            "ItemClassName": "Mag_AKM_30Rnd",
                            "SpawnChance": 100.0,
                            "Amount": 30,
                            "HealthMin": 1.0,
                            "HealthMax": 1.0
                        }
                    ]
                }
            ]
        }
    ],
    "CurrencyRewards": [],
    "CurrencyMin": 0,
    "CurrencyMax": 0
}
```

---

### Step 7 — Set up zone integration (optional)

**Requirements:** NinjinsPvPPvE mod must be installed and loaded.

Add zone settings to `LeaderBoardConfig.json`:

```json
{
    "TrackPVEKillsInZoneTypes": ["everywhere"],
    "ExcludePVPKillWhenBothInPVE": true,
    "EnableZoneKillExclusions": true,
    "ExcludeKillsInZones": [
        {
            "ZoneType": "safezone",
            "ExcludedClassNames": ["PlayerBase", "AnimalBase", "ZombieBase", "eAIBase"],
            "ZoneNames": []
        }
    ]
}
```

**Zone types for `TrackPVEKillsInZoneTypes`:**

| Value | Description |
|-------|-------------|
| `"everywhere"` | Track PvE kills everywhere (default) |
| `"pvp"` | Only in PvP zones |
| `"pve"` | Only in PvE zones |
| `"raid"` | Only in raid zones |
| `"visual"` | Only in visual zones |

---

### Web export for your website (optional)

Enable the export in `LeaderBoardConfig.json`:

```json
{
    "EnableWebExport": true,
    "WebExportFileName": "LeaderboardWebExport.json",
    "WebExportIncludePlayerIDs": false,
    "WebExportMaxPlayers": 100
}
```

The server will then automatically generate a JSON file at:
```
<Server Profile>/Ninjins_Tracking_Mod/Data/LeaderboardWebExport.json
```

You can read this file with a web server or a small script and display it on your community website.

---

### Common problems and solutions

| Problem | Solution |
|---------|----------|
| Leaderboard does not open | Check if CF mod is loaded; check client logs for `[TrackingModUI]` |
| No kills tracked | Check server logs for `[TrackingMod]`; verify category config |
| Rewards not working | Set `EnableRewardSystem: true`; check if reward JSON files exist |
| Zone integration broken | Load NinjinsPvPPvE before Ninjins_LeaderBoard; check `EnableZoneKillExclusions` |
| Points not updating | Restart server after config changes |

**Increase log verbosity for debugging:**
```json
// Server: $profile:Ninjins_Tracking_Mod/LoggerConfig.json
{ "Level": 6 }

// Client: $profile:Ninjins_LeaderBoard/LoggerConfig.json
{ "Level": 6 }
```

Log levels: `0` = Off · `1` = Critical · `2` = Error · `3` = Warning · `4` = Info · `5` = Debug · `6` = Trace

---

> **Questions, bugs or feature requests?** → [GitHub Issues](https://github.com/Psyern/Ninjins_Leaderboard/issues)
