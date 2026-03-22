# 📖 Ninjins LeaderBoard — Benutzeranleitung (Deutsch)

> Diese Anleitung richtet sich an **Spieler** und **Server-Administratoren**, die die Ninjins LeaderBoard Mod nutzen oder einrichten möchten.

---

## Inhaltsverzeichnis

- [Für Spieler](#-für-spieler)
  - [Leaderboard öffnen](#leaderboard-öffnen)
  - [Das Leaderboard lesen](#das-leaderboard-lesen)
  - [Milestone-Rewards einlösen](#milestone-rewards-einlösen)
  - [Tastenbelegung ändern](#tastenbelegung-ändern)
- [Für Server-Administratoren](#-für-server-administratoren)
  - [Schritt 1 — Mod installieren](#schritt-1--mod-installieren)
  - [Schritt 2 — Startparameter konfigurieren](#schritt-2--startparameter-konfigurieren)
  - [Schritt 3 — Erster Serverstart](#schritt-3--erster-serverstart)
  - [Schritt 4 — Hauptkonfiguration anpassen](#schritt-4--hauptkonfiguration-anpassen)
  - [Schritt 5 — Kill-Kategorien konfigurieren](#schritt-5--kill-kategorien-konfigurieren)
  - [Schritt 6 — Reward-System einrichten (optional)](#schritt-6--reward-system-einrichten-optional)
  - [Schritt 7 — Zonen-Integration einrichten (optional)](#schritt-7--zonen-integration-einrichten-optional)
  - [Web-Export für die eigene Website (optional)](#web-export-für-die-eigene-website-optional)
  - [Häufige Probleme und Lösungen](#häufige-probleme-und-lösungen)

---

## 🎮 Für Spieler

### Leaderboard öffnen

| Taste | Aktion |
|-------|--------|
| **F5** | **PvE-Leaderboard** öffnen / schließen |
| **F6** | **PvP-Leaderboard** öffnen / schließen |

> **Hinweis:** Die Mod muss im DayZ Launcher aktiviert sein und der Server muss die Mod installiert haben. Außerdem ist der Community Framework (CF) Mod erforderlich.

---

### Das Leaderboard lesen

Das Leaderboard zeigt alle getrackten Spieler, sortiert nach ihren Punkten.

| Spalte | Beschreibung |
|--------|-------------|
| **Rang** | Deine aktuelle Position |
| **Name** | Spielername (🟢 = online, ⚫ = offline) |
| **Punkte** | Gesamte PvE- oder PvP-Punkte |
| **Kills** | Kills pro Kategorie (Zombies, Wölfe, Spieler usw.) |
| **Tode** | Anzahl der Tode |

**Wie werden Punkte berechnet?**

- Jeder Kill in einer Kategorie gibt **Basispunkte × Multiplikator**
- Jeder Tod zieht **Strafpunkte** ab
- Punkte können **nicht unter 0 fallen**

**Beispiel:** 10 Zombie-Kills × 10 Punkte = 100 PvE-Punkte. 1 Tod × 5 Strafe = −5 Punkte → **95 PvE-Punkte gesamt**

---

### Milestone-Rewards einlösen

Falls der Server das Reward-System aktiviert hat, kannst du Belohnungen für erreichte Kill-Meilensteine einlösen:

1. **Leaderboard öffnen** (F5 oder F6)
2. Suche nach einem **„Reward einlösen"**-Button neben deinem Namen oder in deinen Stats
3. Klicke darauf — die Items werden direkt in dein Inventar gespawnt
4. Jeder Meilenstein kann nur **einmal** eingelöst werden

> Meilensteine werden vom Server-Admin festgelegt (z.B. 100 Wolf-Kills, 50 Zombie-Kills, 10 Spieler-Kills).

---

### Tastenbelegung ändern

1. Öffne **Optionen** im DayZ-Hauptmenü
2. Gehe zu **Steuerung**
3. Suche den Bereich **„Tracking Mod"**
4. Vergib **PvE-Leaderboard** und **PvP-Leaderboard** neue Tasten nach Wunsch

---

## 🛠️ Für Server-Administratoren

### Schritt 1 — Mod installieren

Kopiere den Mod-Ordner in das DayZ-Server-Stammverzeichnis:

```
<DayZ Server Root>/
└── @Ninjins_LeaderBoard/
    └── Ninjins_LeaderBoard/
        ├── config.cpp
        ├── scripts/
        └── ...
```

Stelle sicher, dass auch der **Community Framework (CF)** Mod installiert ist:
- Steam Workshop ID: [1559212036](https://steamcommunity.com/sharedfiles/filedetails/?id=1559212036)

---

### Schritt 2 — Startparameter konfigurieren

Füge die Mod zum Server-Startskript hinzu:

```
-mod=@CF;@Ninjins_LeaderBoard
```

**Mit optionalen Mods:**

```
# Mit Zonen-System:
-mod=@CF;@NinjinsPvPPvE;@Ninjins_LeaderBoard

# Mit Expansion AI:
-mod=@CF;@DayZ-Expansion-AI;@Ninjins_LeaderBoard

# Alle kombiniert:
-mod=@CF;@NinjinsPvPPvE;@DayZ-Expansion-AI;@Ninjins_LeaderBoard
```

> ⚠️ Die Reihenfolge ist wichtig. CF muss immer **vor** Ninjins_LeaderBoard stehen.

---

### Schritt 3 — Erster Serverstart

Starte den Server einmal. Die Mod erstellt automatisch alle Konfigurationsdateien unter:

```
<Server-Profil>/Ninjins_Tracking_Mod/Data/
├── LeaderBoardConfig.json
├── PVE_Categories.json
├── PVP_Categories.json
└── TrackingModRewardConfig.json
```

Dann den Server **stoppen** und mit der Konfiguration fortfahren.

---

### Schritt 4 — Hauptkonfiguration anpassen

Öffne `LeaderBoardConfig.json` und passe die wichtigsten Einstellungen an:

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

**Die wichtigsten Einstellungen erklärt:**

| Einstellung | Empfehlung |
|-------------|-----------|
| `AdminIDs` | Deine Steam64-ID eintragen — Admins sehen zusätzliche Funktionen |
| `DeletePlayerFilesOlderThanDays` | Auf `30` setzen, um inaktive Spieler automatisch zu bereinigen |
| `DisablePVPLeaderboard` | Auf `true` setzen bei reinen PvE-Servern |
| `DisablePVELeaderboard` | Auf `true` setzen bei reinen PvP-Servern |
| `EnableRewardSystem` | Auf `true` setzen, wenn Milestone-Rewards genutzt werden sollen |

---

### Schritt 5 — Kill-Kategorien konfigurieren

#### PvE-Kategorien (`PVE_Categories.json`)

Definiere, welche Entitäten als PvE-Kills zählen und wie viele Punkte sie geben:

```json
{
    "Categories": [
        {
            "CategoryID": "Zombies",
            "ClassNamePreview": "ZmbM_CitizenASkinny",
            "ClassNames": ["ZombieBase:10:1"]
        },
        {
            "CategoryID": "Woelfe",
            "ClassNamePreview": "Animal_CanisLupus_Grey",
            "ClassNames": ["Animal_CanisLupus:25:1"]
        },
        {
            "CategoryID": "Baeren",
            "ClassNamePreview": "Animal_UrsusArctos",
            "ClassNames": ["Animal_UrsusArctos:50:1"]
        }
    ],
    "PVEDeathPenaltyPoints": 5
}
```

**Format:** `"ClassName:Basispunkte:Multiplikator"`
- `ZombieBase:10:1` → jeder Zombie-Kill = 10 Punkte
- `Animal_CanisLupus:25:1` → jeder Wolf-Kill = 25 Punkte

#### PvP-Kategorien (`PVP_Categories.json`)

```json
{
    "Categories": [
        {
            "CategoryID": "Players",
            "DisplayName": "Spieler",
            "ClassNames": ["PlayerBase:100:1"]
        }
    ],
    "PVPDeathPenaltyPoints": 10
}
```

---

### Schritt 6 — Reward-System einrichten (optional)

1. `"EnableRewardSystem": true` in `LeaderBoardConfig.json` setzen
2. `TrackingModRewardConfig.json` öffnen und Meilensteine definieren:

```json
{
    "EnableMilestoneRewards": true,
    "RewardConfigFolder": "$profile:Ninjins_Tracking_Mod\\Data\\RewardSystem\\",
    "PVEMilestones": {
        "Zombies": [50, 100, 250, 500, 1000],
        "Woelfe": [25, 50, 100, 200, 500]
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

3. Reward-Dateien im Ordner `Data/RewardSystem/` erstellen, z.B. `Zombies50.json`:

```json
{
    "RewardItems": [
        {
            "Name": "Zombie Jaeger Starterpaket",
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

### Schritt 7 — Zonen-Integration einrichten (optional)

**Voraussetzung:** NinjinsPvPPvE Mod muss installiert und geladen sein.

Zonen-Einstellungen zur `LeaderBoardConfig.json` hinzufügen:

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

**Zonen-Typen für `TrackPVEKillsInZoneTypes`:**

| Wert | Beschreibung |
|------|-------------|
| `"everywhere"` | PvE-Kills überall tracken (Standard) |
| `"pvp"` | Nur in PvP-Zonen |
| `"pve"` | Nur in PvE-Zonen |
| `"raid"` | Nur in Raid-Zonen |
| `"visual"` | Nur in visuellen Zonen |

---

### Web-Export für die eigene Website (optional)

Export in `LeaderBoardConfig.json` aktivieren:

```json
{
    "EnableWebExport": true,
    "WebExportFileName": "LeaderboardWebExport.json",
    "WebExportIncludePlayerIDs": false,
    "WebExportMaxPlayers": 100
}
```

Der Server generiert dann automatisch eine JSON-Datei unter:
```
<Server-Profil>/Ninjins_Tracking_Mod/Data/LeaderboardWebExport.json
```

Diese Datei kann mit einem Webserver oder einem kleinen Skript eingelesen und auf der Community-Website angezeigt werden.

---

### Häufige Probleme und Lösungen

| Problem | Lösung |
|---------|--------|
| Leaderboard öffnet sich nicht | Prüfen ob CF-Mod geladen ist; Client-Logs nach `[TrackingModUI]` durchsuchen |
| Keine Kills werden getrackt | Server-Logs nach `[TrackingMod]` prüfen; Kategorie-Config überprüfen |
| Rewards funktionieren nicht | `EnableRewardSystem: true` setzen; prüfen ob Reward-JSON-Dateien vorhanden sind |
| Zonen-Integration funktioniert nicht | NinjinsPvPPvE vor Ninjins_LeaderBoard laden; `EnableZoneKillExclusions` prüfen |
| Punkte werden nicht aktualisiert | Server nach Config-Änderungen neu starten |

**Log-Level für Debugging erhöhen:**
```json
// Server: $profile:Ninjins_Tracking_Mod/LoggerConfig.json
{ "Level": 6 }

// Client: $profile:Ninjins_LeaderBoard/LoggerConfig.json
{ "Level": 6 }
```

Log-Level: `0` = Aus · `1` = Critical · `2` = Error · `3` = Warning · `4` = Info · `5` = Debug · `6` = Trace

---

> **Fragen, Bugs oder Feature-Requests?** → [GitHub Issues](https://github.com/Psyern/Ninjins_Leaderboard/issues)
