# 🏆 Ninjins LeaderBoard — DayZ Mod

[![DayZ](https://img.shields.io/badge/DayZ-1.29+-blue)](https://dayz.com)
[![Enforce Script](https://img.shields.io/badge/Enforce-Script-orange)](https://community.bistudio.com/wiki/DayZ:Enforce_Script_Syntax)
[![Maintainer](https://img.shields.io/badge/Maintainer-Psyern-green)](https://github.com/Psyern)

> **Ein vollständiges PvE- & PvP-Leaderboard- und Kill-Tracking-System für DayZ Standalone Server.**

---

## ⚠️ Wichtiger Hinweis — Maintainer-Wechsel

> Mit **DayZ Update 1.29** beendet **Ninjin** seine Modding-Karriere.
> Die Instandhaltung und Weiterentwicklung dieser Mod wird ab sofort von **Psyern** übernommen.
>
> Vielen Dank an Ninjin für die originale Entwicklung und den Grundstein dieses Projekts! 🙏

---

## 📖 Inhaltsverzeichnis

- [Features](#-features)
- [Abhängigkeiten](#-abhängigkeiten)
- [Installation](#-installation)
- [Konfiguration](#%EF%B8%8F-konfiguration)
  - [Hauptkonfiguration (LeaderBoardConfig.json)](#hauptkonfiguration-leaderboardconfigjson)
  - [PvE-Kategorien (PVE\_Categories.json)](#pve-kategorien-pve_categoriesjson)
  - [PvP-Kategorien (PVP\_Categories.json)](#pvp-kategorien-pvp_categoriesjson)
  - [Reward-System (TrackingModRewardConfig.json)](#reward-system-trackingmodrewardconfigjson)
  - [Logger-Konfiguration](#logger-konfiguration)
- [Tastenbelegung](#-tastenbelegung)
- [Projektstruktur](#-projektstruktur)
- [Technische Details](#-technische-details)
  - [Script-Module](#script-module)
  - [Kill-Tracking-System](#kill-tracking-system)
  - [Punkte-System](#punkte-system)
  - [Reward/Milestone-System](#rewardmilestone-system)
  - [Zonen-Integration (NinjinsPvPPvE)](#zonen-integration-ninjinspvppve)
  - [Daten-Speicherung](#daten-speicherung)
  - [RPC-Kommunikation](#rpc-kommunikation)
  - [UI-System (MVC)](#ui-system-mvc)
- [Serverseitige Dateien](#-serverseitige-dateien)
- [Optionale Mod-Kompatibilität](#-optionale-mod-kompatibilität)
- [Fehlerbehebung](#-fehlerbehebung)
- [Credits](#-credits)
- [Lizenz](#-lizenz)

---

## ✨ Features

### Leaderboard
- **Separates PvE- und PvP-Leaderboard** — jeweils unabhängig schaltbar
- **Kategorie-basiertes Kill-Tracking** — Zombies, Wölfe, Tiere, Spieler, AI und mehr
- **Punkte-System** mit konfigurierbaren Basispunkten und Multiplikatoren pro Kategorie
- **Todes-Strafpunkte** — konfigurierbar für PvE und PvP
- **Online-Status-Anzeige** — zeigt an, welche Spieler gerade online sind
- **Survivor-Typ-Icons** — männliche/weibliche Survivor-Icons im Leaderboard
- **Seitenweise Navigation** — paginiertes Leaderboard mit Client-seitigem Page-Caching
- **Rate-Limiting** — Server-seitige Anfragebegrenzung (500ms) gegen Spam

### Kill-Tracking
- **Spieler-Kills (PvP)** — inkl. Kill-Range-Tracking
- **Zombie-Kills** — alle Infizierten
- **Tier-Kills** — Wölfe, Bären und andere Tiere
- **AI-Kills** — Expansion AI (eAI) Unterstützung
- **Granaten-Kills** — Tracking des Werfers über `DayZPlayerImplementThrowing`
- **Explosiv-Kills** — Tracking des Platzierers (Minen, C4 etc.)
- **Fallen-Kills** — Tracking über `TrapBase`
- **Fahrzeug-Kills** — Roadkills
- **Kill-Ursachen-Erkennung** — Automatische Klassifizierung (Waffe, Unbewaffnet, Granate, Falle, etc.)

### Reward-System
- **Milestone-basierte Belohnungen** — konfigurierbare Meilensteine pro Kategorie
- **Loot-Tabellen** — JSON-basierte Reward-Konfigurationen mit Spawn-Chancen
- **Item-Attachments** — Rekursive Attachments mit eigenen Spawn-Chancen
- **Quantitäts-/Gesundheitskontrolle** — Min/Max-Werte für Munition, Energie, Zustand
- **Currency-Rewards** — Optionale Währungs-Belohnungen

### Zonen-Integration
- **NinjinsPvPPvE Kompatibilität** — vollständige Integration mit dem PvP/PvE-Zonen-System
- **Zonen-basiertes Kill-Tracking** — Kills nur in bestimmten Zonen-Typen tracken
- **Kill-Ausschlüsse pro Zone** — Bestimmte Kill-Typen in bestimmten Zonen ausschließen
- **Zonen-Name-Matching** — Wildcard-Matching für Zonennamen (z.B. `FlagPvPZone_*`)

---

## 📦 Abhängigkeiten

| Mod | Erforderlich | Hinweis |
|-----|:---:|---------|
| **DayZ Standalone** | ✅ | Version 1.29+ |
| **DZ_Data** (Vanilla) | ✅ | Basis-Addon |
| **[Community Framework (CF)](https://steamcommunity.com/sharedfiles/filedetails/?id=1559212036)** | ✅ | Für RPC-Manager, JSON-Laden, Verzeichnis-Operationen |
| **[NinjinsPvPPvE](https://github.com/Psyern)** | ❌ | Optional — für Zonen-Integration (PvP/PvE/SafeZone/Raid-Zonen) |
| **[DayZ Expansion AI (eAI)](https://steamcommunity.com/sharedfiles/filedetails/?id=2792982069)** | ❌ | Optional — für AI-Kill-Tracking |

---

## 🚀 Installation

### Server-seitige Installation

1. **Mod-Ordner kopieren:**
   ```
   Ninjins_LeaderBoard/
   ```
   in den DayZ-Server-Root-Ordner kopieren.

2. **Server-Startparameter erweitern:**
   ```
   -mod=@CF;@Ninjins_LeaderBoard
   ```
   > Falls NinjinsPvPPvE verwendet wird:
   > ```
   > -mod=@CF;@NinjinsPvPPvE;@Ninjins_LeaderBoard
   > ```

3. **Server starten** — Konfigurationsdateien werden automatisch unter `$profile:Ninjins_Tracking_Mod\Data\` erstellt.

4. **Konfigurationen anpassen** und Server neu starten.

### Client-seitige Installation

1. **Mod abonnieren** (Steam Workshop) oder Mod-Ordner in den DayZ-Client-Ordner kopieren.
2. **Mod aktivieren** im DayZ Launcher.

---

## ⚙️ Konfiguration

Alle Konfigurationsdateien werden beim ersten Serverstart automatisch generiert unter:
```
<Server-Profil>/Ninjins_Tracking_Mod/Data/
```

### Hauptkonfiguration (`LeaderBoardConfig.json`)

| Parameter | Typ | Standard | Beschreibung |
|-----------|-----|----------|--------------|
| `AdminIDs` | `string[]` | `[]` | Steam64-IDs der Admins |
| `DeletePlayerFilesOlderThanDays` | `int` | `0` | Auto-Cleanup inaktiver Spieler (0 = deaktiviert) |
| `UseUTCForDates` | `bool` | `false` | UTC-Zeitzone für Datumsstempel verwenden |
| `EnableRewardSystem` | `bool` | `false` | Milestone-Reward-System aktivieren |
| `DisablePVPLeaderboard` | `bool` | `false` | PvP-Leaderboard deaktivieren |
| `DisablePVELeaderboard` | `bool` | `false` | PvE-Leaderboard deaktivieren |
| `MaxPVEPlayersDisplay` | `int` | `10` | Max. angezeigte Spieler im PvE-Leaderboard (max. 100) |
| `MaxPVPPlayersDisplay` | `int` | `24` | Max. angezeigte Spieler im PvP-Leaderboard (max. 100) |
| `ShowPlayerOnlineStatusPVE` | `bool` | `true` | Online-Status im PvE-Leaderboard anzeigen |
| `ShowPlayerOnlineStatusPVP` | `bool` | `true` | Online-Status im PvP-Leaderboard anzeigen |
| `SurvivorIconPathMale` | `string` | `"...male.edds"` | Pfad zum männlichen Survivor-Icon |
| `SurvivorIconPathFemale` | `string` | `"...female.edds"` | Pfad zum weiblichen Survivor-Icon |

#### Kill/Death-Tracking deaktivieren

| Parameter | Beschreibung |
|-----------|--------------|
| `DisableDeathBySuicide` | Tode durch Selbstmord ignorieren |
| `DisableDeathByGrenade` | Tode durch Granaten ignorieren |
| `DisableDeathByTrap` | Tode durch Fallen ignorieren |
| `DisableDeathByZombie` | Tode durch Zombies ignorieren |
| `DisableDeathByAnimal` | Tode durch Tiere ignorieren |
| `DisableDeathByAI` | Tode durch AI ignorieren |
| `DisableDeathByCar` | Tode durch Fahrzeuge ignorieren |
| `DisableDeathByWeapon` | Tode durch Waffen ignorieren |
| `DisableDeathByUnarmed` | Tode durch Unbewaffnete ignorieren |
| `DisableDeathByUnknown` | Tode durch Unbekanntes ignorieren |
| `DisableKillByGrenade` | Granaten-Kills ignorieren |
| `DisableKillByTrap` | Fallen-Kills ignorieren |
| `DisableKillByAnimal` | Tier-Kills ignorieren |
| `DisableKillByZombie` | Zombie-Kills ignorieren |
| `DisableKillByCar` | Fahrzeug-Kills ignorieren |
| `DisableKillByWeapon` | Waffen-Kills ignorieren |
| `DisableKillByUnarmed` | Unbewaffnete Kills ignorieren |
| `DisableKillByUnknown` | Unbekannte Kills ignorieren |
| `DisableKillByUnconsciousSuicide` | Bewusstlos-Selbstmord Kills ignorieren |

#### Zonen-Konfiguration

| Parameter | Typ | Beschreibung |
|-----------|-----|--------------|
| `TrackPVEKillsInZoneTypes` | `string[]` | In welchen Zonen-Typen PvE-Kills getrackt werden (`"everywhere"`, `"pvp"`, `"pve"`, `"visual"`, `"raid"`) |
| `ExcludePVPKillWhenBothInPVE` | `bool` | PvP-Kills ignorieren wenn beide Spieler in PvE-Zone |
| `EnableZoneKillExclusions` | `bool` | Zonen-basierte Kill-Ausschlüsse aktivieren |
| `ExcludeKillsInZones` | `array` | Liste von Ausschluss-Regeln (siehe unten) |

**Beispiel für Zonen-Ausschlüsse:**
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

---

### PvE-Kategorien (`PVE_Categories.json`)

Definiert die Kill-Kategorien für das PvE-Leaderboard.

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

**ClassNames-Format:** `"ClassName:BasePoints:Multiplier"`
- `ClassName` — DayZ-Klassenname (unterstützt Basis-Klassen-Matching)
- `BasePoints` — Punkte pro Kill
- `Multiplier` — Multiplikator (Standard: 1)

**Matching-Logik:**
1. Exaktes Matching: `className == categoryClassName`
2. Prefix-Matching: `eAI_*` für alle eAI-Klassen
3. Unterstrich-Prefix: `Animal_CanisLupus_*`
4. Entity-IsKindOf-Check (Runtime)
5. CfgVehicles Config-Hierarchie-Check

---

### PvP-Kategorien (`PVP_Categories.json`)

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

### Reward-System (`TrackingModRewardConfig.json`)

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

**Reward-Datei Struktur** (z.B. `Wolfes100.json`):
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

### Logger-Konfiguration

#### Server-Logger (`$profile:Ninjins_Tracking_Mod/LoggerConfig.json`)
```json
{
    "Level": 4
}
```

#### Client-Logger (`$profile:Ninjins_LeaderBoard/LoggerConfig.json`)
```json
{
    "Level": 6
}
```

**Log-Level:**
| Level | Beschreibung |
|-------|-------------|
| 0 | Aus |
| 1 | Critical |
| 2 | Error |
| 3 | Warning |
| 4 | Info (Standard Server) |
| 5 | Debug |
| 6 | Trace (Standard Client) |

---

## ⌨️ Tastenbelegung

| Taste | Aktion |
|-------|--------|
| **F5** | PvE-Leaderboard öffnen/schließen |
| **F6** | PvP-Leaderboard öffnen/schließen |

Die Tastenbelegung kann im Spiel unter **Optionen → Steuerung → Tracking Mod** angepasst werden.

---

## 📁 Projektstruktur

```
Ninjins_LeaderBoard/
├── config.cpp                          # Mod-Definition (CfgPatches, CfgMods)
├── stringtable.csv                     # Lokalisierung (EN)
│
├── data/
│   └── modded_inputs.xml               # Tastenbelegung (F5=PvE, F6=PvP)
│
├── gui/
│   ├── icons/
│   │   └── survivor/
│   │       ├── male.edds / male.png    # Männlicher Survivor-Icon
│   │       └── female.png              # Weiblicher Survivor-Icon
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
│   ├── 3_Game/                         # Game-Script-Modul
│   │   ├── General Configs/
│   │   │   ├── Config/
│   │   │   │   ├── TrackingModConfig.c         # Hauptkonfiguration
│   │   │   │   ├── TrackingModRewardConfig.c   # Reward-Konfiguration
│   │   │   │   ├── PVECategoryConfig.c         # PvE-Kategorien
│   │   │   │   ├── PVPCategoryConfig.c         # PvP-Kategorien
│   │   │   │   └── DeathCategoryConfig.c       # Todes-Kategorien
│   │   │   ├── Data/
│   │   │   │   ├── Ninjins_Tracking_Mod_Data.c # Spielerdaten (PlayerDeathData)
│   │   │   │   └── TrackingModLeaderboardData.c# Leaderboard-Datenstrukturen
│   │   │   ├── Rewards/
│   │   │   │   ├── TrackingModMilestoneHelper.c      # Milestone-Berechnung
│   │   │   │   └── TrackingModMilestoneRewardLoader.c # Reward-Datei-Laden
│   │   │   └── Utils/
│   │   │       ├── TrackingModLogger.c         # Server-Logger (TrackingMod)
│   │   │       └── TrackingModZoneUtils.c      # Zonen-Hilfsfunktionen
│   │   └── TrackingModUI/
│   │       ├── TrackingModLeaderboardData.c    # Shared Datenklassen (Client/Server)
│   │       └── TrackingModLogger.c             # Client-Logger (TrackingModUI)
│   │
│   ├── 4_World/                        # World-Script-Modul
│   │   ├── TrackingModRewardHelper.c   # Reward-Vergabe & Item-Spawning
│   │   ├── Zone Related/
│   │   │   └── TrackingModZoneHandler.c# Zonen-Event-Callbacks
│   │   └── entities/
│   │       ├── creatures/
│   │       │   ├── TrackingMod_EntityHelper.c  # Entity-Matching-Helfer
│   │       │   ├── ai/
│   │       │   │   └── eAIBase.c               # Expansion AI Kill-Tracking
│   │       │   ├── animals/
│   │       │   │   └── AnimalBase.c            # Tier-Kill-Tracking
│   │       │   └── infected/
│   │       │       └── ZombieBase.c            # Zombie-Kill-Tracking
│   │       ├── itembase/
│   │       │   ├── ExplosivesBase.c            # Explosiv-Placer-Tracking
│   │       │   ├── Grenade_Base.c              # Granaten-Werfer-Tracking
│   │       │   └── TrapBase.c                  # Fallen-Placer-Tracking
│   │       ├── manbase/
│   │       │   └── PlayerBase.c                # Spieler-Events & Kill-Tracking
│   │       └── DayZPlayerImplementThrowing.c   # Granaten-Werfer-Zuordnung
│   │
│   └── 5_Mission/                      # Mission-Script-Modul
│       ├── MissionServer.c             # Server-Logik, RPC-Handler, Init
│       └── TrackingModUI/
│           ├── TrackingModLeaderboardInputHandler.c  # Input & RPC-Client
│           ├── TrackingModKillCategory.c             # Kill-Kategorie UI-Widget
│           ├── TrackingModKillCategoryController.c   # Kill-Kategorie Controller
│           ├── PvE/
│           │   ├── TrackingModLeaderboardMenu.c      # PvE-Leaderboard-Menü
│           │   ├── TrackingModLeaderboardController.c # PvE-Controller
│           │   ├── TrackingModPlayerEntry.c          # PvE-Spieler-Eintrag
│           │   └── TrackingModPlayerEntryController.c # PvE-Eintrag-Controller
│           └── PvP/
│               ├── TrackingModPvPLeaderboardMenu.c    # PvP-Leaderboard-Menü
│               ├── TrackingModPvPLeaderboardController.c # PvP-Controller
│               ├── TrackingModPvPPlayerEntry.c        # PvP-Spieler-Eintrag
│               └── TrackingModPvPPlayerEntryController.c # PvP-Eintrag-Controller
│
└── todo/
    └── Class_Renaming_Analysis.md      # Analyse für Klassen-Umbenennung
```

---

## 🔧 Technische Details

### Script-Module

Die Mod nutzt das DayZ Enforce-Script-System mit drei Modulen:

| Modul | Pfad | Beschreibung |
|-------|------|--------------|
| **3_Game** | `scripts/3_Game/` | Konfigurationen, Datenstrukturen, Logger, Utilities |
| **4_World** | `scripts/4_World/` | Entity-Modding, Kill-Tracking, Zonen-Handler, Rewards |
| **5_Mission** | `scripts/5_Mission/` | Server-Init, RPC-Handler, UI/Menüs, Input-Handler |

### Kill-Tracking-System

Das Kill-Tracking funktioniert über **modded DayZ-Basisklassen**:

| Klasse | Tracking-Methode | Kill-Typ |
|--------|-----------------|----------|
| `PlayerBase` | `EEHitBy()`, `EEKilled()` | PvP-Kills, Tode |
| `ZombieBase` | `EEKilled()` | Zombie-Kills |
| `AnimalBase` | `EEKilled()`, `EEOnDamageCalculated()` | Tier-Kills |
| `eAIBase` | `EEKilled()`, `EEOnDamageCalculated()` | AI-Kills |
| `Grenade_Base` | `EEOnDamageCalculated()` | Granaten-Zuordnung |
| `ExplosivesBase` | `OnPlacementComplete()` | Explosiv-Zuordnung |
| `TrapBase` | `OnPlacementComplete()` | Fallen-Zuordnung |
| `DayZPlayerImplementThrowing` | `HandleThrowing()` | Werfer-Zuordnung |

**Kill-Ursachen (KillCause):**

| ID | Ursache |
|----|---------|
| 0 | Granate |
| 1 | Falle |
| 2 | Tier |
| 3 | Zombie |
| 4 | Fahrzeug |
| 5 | Waffe |
| 6 | Unbewaffnet |
| 7 | Unbekannt |
| 8 | Bewusstlos-Selbstmord |

### Punkte-System

- **PvE-Punkte:** `Σ (KillsProKategorie × BasePoints × Multiplier) - (Deaths × DeathPenaltyPoints)`
- **PvP-Punkte:** `Σ (KillsProKategorie × BasePoints × Multiplier) - (Deaths × PVPDeathPenaltyPoints)`
- Punkte können nicht unter 0 fallen
- BasePoints und Multiplier werden über das `ClassName:BasePoints:Multiplier`-Format in der Kategorie-Config definiert

### Reward/Milestone-System

1. **Milestones** werden pro Kategorie definiert (z.B. Wolfes: 100, 200, 300 Kills)
2. Erreicht ein Spieler einen Meilenstein, wird dieser in `AvailableMilestones` gespeichert
3. Spieler können Rewards über das Leaderboard-UI **claimen**
4. Rewards werden aus JSON-Dateien geladen und dem Spieler-Inventar hinzugefügt
5. **Loot-Sektionen** haben eigene Spawn-Chancen und ItemsMin/Max
6. **Attachments** werden rekursiv verarbeitet (verschachtelte Attachments möglich)
7. **Item-Eigenschaften** (Gesundheit, Menge, Munition, Energie) sind konfigurierbar

### Zonen-Integration (NinjinsPvPPvE)

Bei aktivierter NinjinsPvPPvE-Mod (`#ifdef NinjinsPvPPvE`):

- **`TrackingModZoneHandler`** — registriert sich als `PlayerStateChangeCallback` und `ZoneEventCallback`
- **`TrackingModZoneUtils`** — verwaltet Spieler-Zonen-Status in Memory-Maps
- **Zonen-Typen:** `pvp`, `pve`, `visual`, `raid`, `safezone`
- **Kill-Ausschlüsse** können pro Zone konfiguriert werden
- **Zonennamen-Matching** unterstützt Wildcard-Prefixe (z.B. `FlagPvPZone_`)

### Daten-Speicherung

```
$profile:Ninjins_Tracking_Mod/
├── Data/
│   ├── LeaderBoardConfig.json          # Hauptkonfiguration
│   ├── PVE_Categories.json             # PvE-Kategorien
│   ├── PVP_Categories.json             # PvP-Kategorien
│   ├── TrackingModRewardConfig.json    # Reward-Konfiguration
│   ├── Players/
│   │   ├── <SteamID64>.json            # Individuelle Spielerdaten
│   │   └── ...
│   └── RewardSystem/
│       ├── Wolfes100.json              # Reward-Dateien
│       ├── Players10.json
│       └── ...
├── Logging/
│   └── Logs/
│       └── TrackingMod_<Datum>.log     # Server-Logs
└── LoggerConfig.json                   # Server-Logger-Config
```

**Spielerdaten (`<SteamID64>.json`):**
```json
{
    "PlayerID": "76561198...",
    "PlayerName": "Spielername",
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

### RPC-Kommunikation

Die Mod nutzt den **Community Framework RPC-Manager** für Client-Server-Kommunikation:

| RPC-Name | Richtung | Beschreibung |
|----------|----------|--------------|
| `RequestTrackingModLeaderboard` | Client → Server | Leaderboard-Daten anfordern (mit Page-Nummer) |
| `ReceiveTrackingModLeaderboard` | Server → Client | Leaderboard-Daten senden |
| `ClaimTrackingModReward` | Client → Server | Milestone-Reward einfordern |
| `ReceiveTrackingModRewardClaim` | Server → Client | Reward-Claim-Ergebnis |
| `ReceivePlayerDataUpdate` | Server → Client | Spielerdaten-Update |

### UI-System (MVC)

Das UI basiert auf dem **Model-View-Controller (MVC)**-Pattern:

- **Layouts** (`.layout`) — XML-basierte UI-Definitionen im `gui/layouts/`-Ordner
- **Menu-Klassen** — `ScriptViewMenu`-basierte Menü-Klassen
- **Controller** — Daten-Binding via Controller-Klassen
- **Page-Caching** — Client-seitiges Caching von Leaderboard-Seiten für schnelle Navigation

---

## 📂 Serverseitige Dateien

Beim ersten Start werden folgende Dateien/Ordner automatisch erstellt:

| Datei/Ordner | Beschreibung |
|-------------|-------------|
| `$profile:Ninjins_Tracking_Mod/` | Root-Ordner |
| `Data/LeaderBoardConfig.json` | Hauptkonfiguration |
| `Data/PVE_Categories.json` | PvE-Kategorien (Standard: Zombies, Wolfes) |
| `Data/PVP_Categories.json` | PvP-Kategorien (Standard: Players) |
| `Data/TrackingModRewardConfig.json` | Reward-System-Konfiguration |
| `Data/Players/` | Spieler-Datendateien |
| `Data/RewardSystem/` | Reward-Konfigurationsdateien |
| `Logging/Logs/` | Log-Dateien |

---

## 🔗 Optionale Mod-Kompatibilität

### DayZ Expansion AI (`#ifdef EXPANSIONMODAI`)
- Automatisches Tracking von eAI-Kills
- `eAIBase`-Klasse wird gemoddet für Damage-Source-Tracking
- eAI-Klassen werden über den `eAI_`-Prefix erkannt

### NinjinsPvPPvE (`#ifdef NinjinsPvPPvE`)
- Zonen-basiertes Kill-Tracking und Ausschlüsse
- `PlayerStateChangeCallback` für PvP/PvE-Zonen-Wechsel
- `ZoneEventCallback` für Zonen-Ein-/Austritt mit Zonennamen
- Kill-Validierung basierend auf Spieler-Zonen-Status

---

## 🐛 Fehlerbehebung

### Leaderboard öffnet sich nicht
- Prüfe ob der **Community Framework (CF)** Mod geladen ist
- Prüfe die DayZ-Client-Logs nach `[TrackingModUI]` Einträgen
- Stelle sicher dass die Tastenbelegung nicht durch andere Mods überschrieben wird

### Keine Kills werden getrackt
- Prüfe die Server-Logs nach `[TrackingMod]` Einträgen
- Prüfe ob die Kategorie-Configs (`PVE_Categories.json`, `PVP_Categories.json`) korrekt sind
- Prüfe ob die Kill-Typen nicht in der `LeaderBoardConfig.json` deaktiviert sind

### Rewards funktionieren nicht
- Stelle sicher dass `EnableRewardSystem` in der Hauptconfig auf `true` steht
- Prüfe ob die Reward-JSON-Dateien im `RewardSystem/`-Ordner existieren
- Prüfe die Server-Logs nach `[TrackingModRewardHelper]` oder `[TrackingModMilestoneRewardLoader]` Einträgen

### Zonen-Integration funktioniert nicht
- Stelle sicher dass **NinjinsPvPPvE** als Mod geladen ist
- Prüfe ob `EnableZoneKillExclusions` auf `true` steht
- Prüfe die Server-Logs nach "Zone callbacks registered with NinjinsPvPPvE"

### Log-Level erhöhen
- Server: `$profile:Ninjins_Tracking_Mod/LoggerConfig.json` → `"Level": 6`
- Client: `$profile:Ninjins_LeaderBoard/LoggerConfig.json` → `"Level": 6`

---

## 👏 Credits

| Rolle | Person |
|-------|--------|
| **Originaler Entwickler** | **Ninjin** |
| **Aktueller Maintainer** | **Psyern** |

---

## 📜 Lizenz

Dieses Projekt ist ein Community-Mod für DayZ Standalone. Alle Rechte am Spiel DayZ liegen bei Bohemia Interactive.

---

> **Fragen, Bugs oder Feature-Requests?** → [GitHub Issues](https://github.com/Psyern/Ninjins_Leaderboard/issues)
