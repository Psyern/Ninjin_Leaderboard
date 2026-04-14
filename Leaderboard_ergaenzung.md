# Multi-Agent Orchestrierung — Ninjins LeaderBoard Feature-Ergaenzung

> **Ziel:** Alle fehlenden Features implementieren, die im Audit vom 2026-04-14 identifiziert wurden.
> **Mod-Pfad:** `C:\Users\Administrator\Desktop\Ninjin_Leaderboard\Ninjins_LeaderBoard`
> **Regeln:** Alle CLAUDE.md und `.claude/rules/` Regeln gelten. Enforce Script only. Tabs. Kein Ternary. Kein `delete`. Kein `ref` auf Locals. `g_Game` statt `GetGame()`. `override` + `super` first immer.

---

## Uebersicht — 3 Phasen, 9 Agents

```
Phase 1 (Foundation — parallel, keine Abhaengigkeiten)
  Agent 1: Tracked Stats Erweiterung
  Agent 2: Simple Cash Rewards
  Agent 3: Admin Permissions Ordner

Phase 2 (Logik — parallel, haengt von Phase 1 ab)
  Agent 4: Admin Menu Wipe/Award Funktionen
  Agent 5: Kill Streak Notifications (Broadcast + Crate Proximity)
  Agent 6: Current Player Highlighting + Summary Section

Phase 3 (UI — parallel, haengt von Phase 2 ab)
  Agent 7: Reward Shop GUI
  Agent 8: Configurable & Sortable Columns
  Agent 9: Style Settings System
```

---

## Bestandsaufnahme — Bestehende Architektur

### Globale Referenzen
```
ref TrackingModConfig g_TrackingModConfig
ref TrackingModRewardConfig g_TrackingModRewardConfig
ref TrackingModData g_TrackingModData
```

### Dateipfade (Server-Profil)
```
TRACKING_MOD_ROOT_FOLDER = "$profile:Ninjins_Tracking_Mod\"
TRACKING_MOD_DATA_DIR    = "$profile:Ninjins_Tracking_Mod\Data\"
TRACKING_MOD_PLAYERS_DIR = "$profile:Ninjins_Tracking_Mod\Data\Players\"
TRACKING_MOD_REWARD_SYSTEM_DIR = "$profile:Ninjins_Tracking_Mod\Data\RewardSystem\"
```

### Bestehende RPC-Namen (MissionServer.c, Zeile 40-44)
```
"RequestTrackingModLeaderboard"   — Client->Server (Param1<int> page)
"ClaimTrackingModReward"          — Client->Server (Param2<string,int> categoryID, milestone)
"RequestAdminConfig"              — Client->Server (keine Params)
"SaveAdminConfig"                 — Client->Server (Param1<TrackingModGeneralAdminData>)
"SaveAdminFullConfig"             — Client->Server (Param3<General, PVE, PVP>)
```

### PlayerDeathData Felder (Ninjins_Tracking_Mod_Data.c, Zeile 11-696)
```
string PlayerID, PlayerName
ref map<string, int> CategoryKills, CategoryDeaths, CategoryLongestRanges
int PvEPoints, PvPPoints, playerIsOnline
string survivorType, LastLoginDate
ref map<string, ref array<int>> AvailableMilestones, ClaimedMilestones
int pendingRewards
```

### TrackingModConfig Felder (TrackingModConfig.c)
```
autoptr array<string> AdminIDs
bool DisablePVPLeaderboard, DisablePVELeaderboard
bool EnableRewardSystem
int MaxPVEPlayersDisplay, MaxPVPPlayersDisplay (max 100)
bool ShowPlayerOnlineStatusPVE, ShowPlayerOnlineStatusPVP
18x Disable*-Flags fuer Death/Kill causes
Zone-Exclusion-System
Web Export System
```

### Admin Menu Tabs (TrackingModAdminMenu.c)
```
Tab 0: General | Tab 1: PVE | Tab 2: PVP | Tab 3: Milestones | Tab 4: Rewards
Buttons: m_BtnSaveApply, m_BtnClose
```

### PlayerBase Hooks (4_World/entities/manbase/PlayerBase.c)
```
override EEHitBy(...)  — speichert m_LastDamageType, m_LastDamageSource, m_LastDamageAmmo
override EEKilled(...)  — ruft TrackDeath(killer) auf
override OnConnect()    — playerIsOnline = 1
override OnDisconnect() — playerIsOnline = 0
void TrackDeath(Object killer) — zentrale Kill/Death Logik
```

---

## PHASE 1 — Foundation (Parallel)

---

### AGENT 1: Tracked Stats Erweiterung

**Ziel:** Shots Fired, Shots Hit, Headshots, Distance Travelled und Accuracy tracken und im Leaderboard anzeigen.

**Fehlende Stats:**
- Shots Fired
- Shots Hit
- Headshots
- Headshot Percentage (berechnet)
- Distance Travelled
- Accuracy (berechnet: ShotsHit / ShotsFired * 100)

**Schritt 1 — Datenmodell erweitern**

Datei: `scripts/3_Game/General Configs/Data/Ninjins_Tracking_Mod_Data.c`
Klasse: `PlayerDeathData` (ab Zeile 11)

Neue Felder hinzufuegen (nach den bestehenden Feldern, VOR den Methoden):
```c
int ShotsFired;
int ShotsHit;
int Headshots;
float DistanceTravelled;
```

Neue Methoden hinzufuegen:
```c
void AddShotFired()
{
	ShotsFired = ShotsFired + 1;
}

void AddShotHit(bool isHeadshot)
{
	ShotsHit = ShotsHit + 1;
	if (isHeadshot)
	{
		Headshots = Headshots + 1;
	}
}

void AddDistanceTravelled(float distance)
{
	DistanceTravelled = DistanceTravelled + distance;
}

float GetAccuracy()
{
	if (ShotsFired <= 0)
		return 0.0;
	float accuracy = (ShotsHit * 100.0) / ShotsFired;
	accuracy = Math.Round(accuracy * 100.0) / 100.0;
	return accuracy;
}

float GetHeadshotPercentage()
{
	if (ShotsHit <= 0)
		return 0.0;
	float pct = (Headshots * 100.0) / ShotsHit;
	pct = Math.Round(pct * 100.0) / 100.0;
	return pct;
}
```

**Schritt 2 — Tracking-Hooks in PlayerBase**

Datei: `scripts/4_World/entities/manbase/PlayerBase.c`

Neue Member-Variablen:
```c
protected vector m_NJN_LastPosition;
protected float m_NJN_DistanceAccumulator;
protected bool m_NJN_PositionInitialized;
```

**Shot Tracking** — Im bestehenden `EEHitBy` Override oder ueber einen neuen Hook.

Pruefe zuerst die Vanilla-Klasse `DayZPlayerImplement` im Referenz-Ordner `C:\Users\Administrator\Desktop\Mod Repositories\scripts - 1.28` nach:
- `OnFire()` oder `Weapon_Base::Fire()` fuer Shots Fired
- `EEHitBy` mit `dmgZone == "Head"` fuer Headshots
- Die Headshot-Detection muss auf der Opfer-Seite (victim's EEHitBy) passieren, der `component` oder `dmgZone` Parameter enthaelt die Trefferzone

**Distance Tracking** — Ueber einen periodischen Check:
- In `OnConnect()`: Position initialisieren
- Neuer Timer oder in einem bestehenden periodischen Call: Distanz seit letzter Position berechnen und addieren
- Nutze `vector.Distance(m_NJN_LastPosition, GetPosition())` — NICHT `DistanceSq`
- Speichere nur wenn Distanz > 0.5m (Rausch-Filter)
- Speichere periodisch (alle 30-60 Sekunden) in PlayerDeathData

**Schritt 3 — Config erweitern**

Datei: `scripts/3_Game/General Configs/Config/TrackingModConfig.c`

Neue Felder in TrackingModConfig:
```c
bool ShowShotsFired;
bool ShowShotsHit;
bool ShowHeadshots;
bool ShowHeadshotPercentage;
bool ShowDistanceTravelled;
bool ShowAccuracy;
```
Defaults: alle `true`. Bestehende `LoadConfig()` und `SaveConfig()` unberuehrt lassen — JSON-Loader fuegt fehlende Felder mit Defaults hinzu.

**Schritt 4 — Leaderboard-UI erweitern**

PvP Layout: `gui/layouts/PvP/trackingModPvPLeaderboard_mvc.layout`
PvP Entry: `gui/layouts/PvP/trackingModPvPPlayerEntry_mvc.layout`
PvE Layout: `gui/layouts/PvE/trackingModLeaderboard_mvc.layout`
PvE Entry: `gui/layouts/PvE/trackingModPlayerEntry_mvc.layout`

Fuer jedes neue Stat-Feld:
- Header-Spalte im Leaderboard-Layout hinzufuegen
- Data-Binding im PlayerEntry-Layout hinzufuegen
- Controller-Property im `TrackingModPlayerEntryController.c` / `TrackingModPvPPlayerEntryController.c`
- Populate-Logik in `TrackingModPlayerEntry.c` / `TrackingModPvPPlayerEntry.c`
- Spalte nur anzeigen wenn Config-Flag `true` ist

**Schritt 5 — RPC-Daten erweitern**

Die Leaderboard-Daten werden per RPC gesendet. Pruefe `MissionServer.c` Zeile 466 (`RequestTrackingModLeaderboard`) — wie die Daten serialisiert werden. Die neuen Felder muessen in der gleichen Reihenfolge geschrieben/gelesen werden.

WICHTIG: Backward-Kompatibilitaet! Alte JSON-Dateien ohne die neuen Felder muessen korrekt geladen werden (Defaults: 0).

**Referenzen zum Pruefen:**
- Vanilla `DayZPlayerImplement` fuer Shot-Events: `C:\Users\Administrator\Desktop\Mod Repositories\scripts - 1.28\4_World\entities\`
- Vanilla `Weapon_Base` fuer Fire-Events: gleicher Ordner
- DmgZone "Head" Erkennung: Vanilla `EEHitBy` Component-System

---

### AGENT 2: Simple Cash Rewards

**Ziel:** Ein System das alle X Kills automatisch eine konfigurierbare Waehrung gibt — unabhaengig vom Milestone-System.

**Schritt 1 — Config erweitern**

Datei: `scripts/3_Game/General Configs/Config/TrackingModRewardConfig.c`

Neue Klasse (nach `TrackingModCurrencyReward`, ca. Zeile 77):
```c
class TrackingModSimpleCashReward
{
	bool Enabled;
	string CurrencyClassName;
	int ZombieKillInterval;
	int ZombieRewardAmount;
	int AnimalKillInterval;
	int AnimalRewardAmount;
	int AIKillInterval;
	int AIRewardAmount;
	int PlayerKillInterval;
	int PlayerRewardAmount;
}
```

Neues Feld in `TrackingModRewardConfig` (Zeile 97+):
```c
ref TrackingModSimpleCashReward SimpleCashRewards;
```

In `CreateDefaultMilestoneConfig()` (Zeile 116) Default-Werte setzen:
```c
SimpleCashRewards = new TrackingModSimpleCashReward();
SimpleCashRewards.Enabled = false;
SimpleCashRewards.CurrencyClassName = "Hryvnia";
SimpleCashRewards.ZombieKillInterval = 10;
SimpleCashRewards.ZombieRewardAmount = 50;
SimpleCashRewards.AnimalKillInterval = 5;
SimpleCashRewards.AnimalRewardAmount = 100;
SimpleCashRewards.AIKillInterval = 5;
SimpleCashRewards.AIRewardAmount = 150;
SimpleCashRewards.PlayerKillInterval = 3;
SimpleCashRewards.PlayerRewardAmount = 200;
```

**Schritt 2 — Reward-Logik**

Datei: `scripts/4_World/TrackingModRewardHelper.c`

Neue statische Methode:
```c
static void CheckSimpleCashReward(PlayerBase player, string playerID, string categoryType, int totalKills)
```

Logik:
1. `g_TrackingModRewardConfig` pruefen ob `SimpleCashRewards` existiert und `Enabled`
2. Je nach `categoryType` ("Zombies", "Animals", "AI", "Players") das passende Interval und Amount holen
3. Pruefen ob `totalKills % interval == 0` (jedes X-te Kill)
4. Currency-Item erzeugen und in Inventar legen (bestehende Logik aus `ClaimTrackingModReward` in MissionServer.c Zeile 1060+ als Referenz nutzen)
5. Wenn Inventar voll: Item am Boden spawnen
6. Notification an Spieler senden

**Schritt 3 — Hook in Kill-Tracking**

Datei: `scripts/3_Game/General Configs/Data/Ninjins_Tracking_Mod_Data.c`

In den Methoden `AddKill()` (Zeile 1487), `AddZombieKill()` (Zeile 1576), `AddAnimalKill()` (Zeile 1627), `AddAIKill()` (Zeile 1678):

Nach dem erfolgreichen Kill-Tracking und Point-Update den Simple Cash Check aufrufen. ACHTUNG: Diese Methoden laufen auf dem Server. Der Player muss ueber `GetGame().GetPlayers()` oder die PlayerIdentity gefunden werden.

**Schritt 4 — Admin Menu Integration**

Im Admin Menu (Agent 4) spaeter eine Section fuer Simple Cash Rewards hinzufuegen. Fuer jetzt: Config wird per JSON editiert.

---

### AGENT 3: Admin Permissions Ordner

**Ziel:** Admins ueber eine separate Datei in einem Permissions-Ordner verwalten, statt nur ueber das AdminIDs-Array in der Config.

**Schritt 1 — Ordnerstruktur**

Neuer Pfad: `$profile:Ninjins_Tracking_Mod\Permissions\`
Datei: `admins.json`

Format:
```json
{
	"AdminSteamIDs": [
		"76561198000000001",
		"76561198000000002"
	]
}
```

**Schritt 2 — Loader-Klasse**

Neue Datei: `scripts/3_Game/General Configs/Config/TrackingModPermissions.c`

```c
class TrackingModPermissionsFile
{
	ref array<string> AdminSteamIDs;
}

class TrackingModPermissions
{
	protected static ref TrackingModPermissions s_NJN_Instance;
	protected ref array<string> m_NJN_AdminIDs;

	// Laedt aus Permissions/admins.json
	// Merged mit g_TrackingModConfig.AdminIDs
	// IsAdmin() prueft beide Quellen
	static TrackingModPermissions GetInstance();
	void LoadPermissions();
	bool IsAdmin(string playerID);
	array<string> GetAllAdminIDs();
}
```

**Schritt 3 — Integration**

Datei: `scripts/3_Game/General Configs/Config/TrackingModConfig.c`

Die bestehende `IsAdmin()` Methode (Zeile 160) erweitern: zusaetzlich `TrackingModPermissions.GetInstance().IsAdmin(playerID)` pruefen.

Datei: `scripts/5_Mission/MissionServer.c`

In `OnInit()`: Permissions laden nach Config-Load.

**Schritt 4 — Auto-Generate**

Wenn `Permissions/admins.json` nicht existiert: Default-Datei mit leerem Array erstellen. Log-Meldung ausgeben.

---

## PHASE 2 — Logik (Parallel, nach Phase 1)

---

### AGENT 4: Admin Menu Wipe/Award Funktionen

**Ziel:** Wipe Player Data, Wipe Points, Award Points, Award Item — alles per Admin Menu.

**Voraussetzung:** Agent 3 (Permissions) muss fertig sein.

**Schritt 1 — Neuer Tab im Admin Menu**

Datei: `scripts/5_Mission/TrackingModUI/Admin/TrackingModAdminMenu.c`

Neuer Tab 5: "Players" (m_TabPlayersPanel, m_BtnTabPlayers)

Widgets:
```
- m_PlayerSearchEdit (EditBoxWidget) — SteamID oder Name eingeben
- m_PlayerSearchBtn (ButtonWidget) — Suchen
- m_PlayerResultList (TextListboxWidget) — Ergebnisse
- m_BtnWipePlayerData (ButtonWidget) — Alle Leaderboard-Daten loeschen
- m_BtnWipePlayerPoints (ButtonWidget) — Nur Points auf 0 setzen
- m_EditAwardPoints (EditBoxWidget) — Punktzahl eingeben
- m_BtnAwardPVEPoints (ButtonWidget) — PvE Points vergeben
- m_BtnAwardPVPPoints (ButtonWidget) — PvP Points vergeben
- m_BtnAwardItem (ButtonWidget) — Item vergeben (nutzt Classname aus EditBox)
- m_EditAwardItemClass (EditBoxWidget) — Item Classname
- m_EditAwardItemAmount (EditBoxWidget) — Anzahl
- m_StatusLabel (TextWidget) — Feedback-Text
```

**Schritt 2 — Layout-Datei erweitern**

Datei: `gui/layouts/Admin/trackingModAdminMenu.layout`

Den neuen Tab-Button und das Players-Panel hinzufuegen. Orientiere dich am bestehenden Tab-Aufbau.

**Schritt 3 — Neue RPCs**

Registrierung in `MissionServer.c` (nach Zeile 44):
```
"AdminSearchPlayer"    — Client->Server (Param1<string> searchTerm)
"AdminWipePlayerData"  — Client->Server (Param1<string> playerID)
"AdminWipePlayerPoints"— Client->Server (Param1<string> playerID)
"AdminAwardPoints"     — Client->Server (Param3<string, int, bool> playerID, amount, isPvE)
"AdminAwardItem"       — Client->Server (Param3<string, string, int> playerID, className, amount)
```

Response-RPCs:
```
"ReceiveAdminPlayerSearch"  — Server->Client (Param1<string> JSON-Liste der gefundenen Spieler)
"ReceiveAdminActionResult"  — Server->Client (Param1<TrackingModAdminSaveResponse>)
```

**Schritt 4 — Server-Handler**

In `MissionServer.c`:

**AdminSearchPlayer:** Durchsuche `g_TrackingModData.m_PlayerDataMap` nach Name oder ID. Sende Ergebnis-Liste zurueck.

**AdminWipePlayerData:** 
1. Admin-Check: `g_TrackingModConfig.IsAdmin(sender.GetPlainId())`
2. JSON-Datei loeschen: `DeleteFile(TRACKING_MOD_PLAYERS_DIR + playerID + ".json")`
3. Aus `m_PlayerDataMap` entfernen
4. `MarkLeaderboardDirty()` aufrufen
5. Erfolgs-Notification senden

**AdminWipePlayerPoints:**
1. Admin-Check
2. PlayerData holen, `PvEPoints = 0`, `PvPPoints = 0` setzen
3. `SavePlayerData()` aufrufen
4. Notification senden

**AdminAwardPoints:**
1. Admin-Check
2. PlayerData holen
3. Je nach `isPvE`: `PvEPoints += amount` oder `PvPPoints += amount`
4. SavePlayerData
5. Notification an Admin + Notification an Spieler (wenn online)

**AdminAwardItem:**
1. Admin-Check
2. Spieler-Entity finden (muss online sein)
3. Item erzeugen mit `player.GetInventory().CreateInInventory(className)`
4. Wenn Inventar voll: am Boden spawnen
5. Notification

**Schritt 5 — Admin Menu Client-Logik**

In `TrackingModAdminMenu.c`:
- RPC-Handler fuer `ReceiveAdminPlayerSearch` und `ReceiveAdminActionResult` registrieren
- Button-Clicks senden die entsprechenden RPCs
- Bestaetigung-Dialog vor destruktiven Aktionen (Wipe)

---

### AGENT 5: Kill Streak Notifications (Broadcast + Crate Proximity)

**Ziel:** Serverweite Benachrichtigung bei Kill Streaks + Crate-Spawn mit Proximity-Hinweis.

**Schritt 1 — Config erweitern**

Datei: `scripts/3_Game/General Configs/Config/TrackingModRewardConfig.c`

Neue Felder in `TrackingModRewardConfig`:
```c
bool EnableServerwideMilestoneNotification;
string ServerwideMilestoneMessage;  // "%1 hat %2 %3 Kills erreicht!"
bool EnableCrateSpawnOnMilestone;
float CrateSpawnRadius;           // Radius um Spieler (5-50m)
string CrateContainerClassName;    // z.B. "WoodenCrate"
bool EnableCrateProximityNotification;
float CrateProximityCheckInterval; // Sekunden
float CrateProximityNotifyRadius;  // Meter
float CrateDespawnTime;            // Sekunden bis Crate despawnt
```

**Schritt 2 — Broadcast Notification**

Datei: `scripts/5_Mission/MissionServer.c`

Wenn ein Spieler einen Milestone erreicht UND `EnableServerwideMilestoneNotification == true`:
```c
// Broadcast an alle Spieler
StringLocaliser title = new StringLocaliser("Kill Streak!");
StringLocaliser msg = new StringLocaliser(string.Format(g_TrackingModRewardConfig.ServerwideMilestoneMessage, playerName, milestoneValue, categoryName));
NotificationSystem.Create(title, msg, "set:dayz_gui image:icon_star", ARGB(255, 255, 215, 0), 8.0, NULL);
```

Der Hook dafuer: In `ClaimTrackingModReward` Handler (MissionServer.c Zeile 1060+) ODER besser direkt in `CheckAndAddMilestones()` (Ninjins_Tracking_Mod_Data.c Zeile 513).

ACHTUNG: `NotificationSystem.Create` mit `NULL` als letzten Parameter = Broadcast an alle.

**Schritt 3 — Crate Spawn**

Wenn `EnableCrateSpawnOnMilestone == true` und Spieler einen Milestone claimed:

1. Position berechnen: zufaelliger Punkt im `CrateSpawnRadius` um den Spieler
2. Container erstellen: `GetGame().CreateObjectEx(crateClassName, spawnPos, ECE_PLACE_ON_SURFACE)`
3. Reward-Items in die Crate legen (bestehende Reward-Item-Logik aus TrackingModRewardHelper.c)
4. Crate-Referenz und Position speichern fuer Proximity-System
5. Despawn-Timer setzen: `GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(DespawnCrate, despawnTime * 1000, false, crate)`

**Schritt 4 — Proximity Notification**

Neuer periodischer Check (in MissionServer oder eigene Helper-Klasse):

1. Timer alle `CrateProximityCheckInterval` Sekunden
2. Fuer jede aktive Reward-Crate: Distanz zu Owner-Spieler pruefen
3. Wenn Spieler innerhalb `CrateProximityNotifyRadius`:
   ```
   "Deine Reward-Crate ist X Meter entfernt!"
   ```
4. Notification nur an den Spieler dem die Crate gehoert
5. Nicht spammen — nur einmal pro Distanz-Schwelle (z.B. bei 50m, 25m, 10m)

**Schritt 5 — Config Toggle im Admin Menu**

In Agent 4's Rewards-Tab oder als eigener Abschnitt: Checkboxen fuer die neuen Flags.

---

### AGENT 6: Current Player Highlighting + Summary Section

**Ziel:** Den eigenen Spieler im Leaderboard visuell hervorheben + separaten Stats-Bereich anzeigen.

**Schritt 1 — Highlighting im Leaderboard**

Dateien:
- `scripts/5_Mission/TrackingModUI/PvE/TrackingModPlayerEntry.c`
- `scripts/5_Mission/TrackingModUI/PvP/TrackingModPvPPlayerEntry.c`

Bestehender Code identifiziert bereits den aktuellen Spieler (Zeile 150/326: `currentPlayerID == m_PlayerData.playerID`). Erweitere diese Pruefung:

Wenn aktueller Spieler == Eintrag:
- Hintergrundfarbe des Entry-Widgets aendern (z.B. leichtes Gold/Highlight)
- Schriftfarbe anpassen oder Bold
- Nutze `widget.SetColor(ARGB(80, 255, 215, 0))` fuer semi-transparentes Gold

Layout-Dateien anpassen:
- `gui/layouts/PvE/trackingModPlayerEntry_mvc.layout` — Background-Widget mit Name `player_highlight_bg` hinzufuegen
- `gui/layouts/PvP/trackingModPvPPlayerEntry_mvc.layout` — gleich

**Schritt 2 — Player Summary Section**

Neue Layout-Widgets in den Leaderboard-Layouts:
- `gui/layouts/PvE/trackingModLeaderboard_mvc.layout`
- `gui/layouts/PvP/trackingModPvPLeaderboard_mvc.layout`

Am unteren Rand oder als ausklappbares Panel:
```
+--[ Deine Stats ]------------------------------------+
| Zombie Kills: 142  | Animal Kills: 38  | AI: 67    |
| Deaths: 12 | K/D: 11.83 | Longest Shot: 423m       |
| PvE Points: 2450 | PvP Points: 890                  |
| [Shots: 1240 | Hits: 387 | Accuracy: 31.2%]         |
+-----------------------------------------------------+
```

Implementation:
1. Neues Panel-Widget `player_summary_panel` im Layout
2. TextWidgets fuer jedes Stat-Feld mit Binding-Namen
3. In den Menu-Klassen (`TrackingModLeaderboardMenu.c`, `TrackingModPvPLeaderboardMenu.c`):
   - Eigene PlayerDeathData aus der empfangenen Liste extrahieren
   - Summary-Widgets populieren
   - Panel verstecken wenn eigener Spieler nicht in Daten

**Schritt 3 — RPC-Erweiterung**

Die Leaderboard-Daten enthalten bereits alle Player-Daten der aktuellen Page. Der eigene Spieler koennte aber auf einer anderen Page sein.

Loesung: Server sendet zusaetzlich die eigenen Stats als separates Datenpaket.

Neuer Response-RPC: `"ReceivePlayerOwnStats"` — wird zusammen mit dem Leaderboard gesendet.

In `MissionServer.c` `RequestTrackingModLeaderboard` Handler:
1. Leaderboard-Page senden (bestehend)
2. Zusaetzlich: eigene PlayerDeathData des anfragenden Spielers finden und separat senden

---

## PHASE 3 — UI (Parallel, nach Phase 2)

---

### AGENT 7: Reward Shop GUI

**Ziel:** Ein kaufbarer Shop wo Spieler ihre verdienten Points fuer Items/Crates ausgeben koennen.

**Voraussetzung:** Points-System existiert bereits (PvEPoints/PvPPoints).

**Schritt 1 — Shop Config**

Neue Datei: `scripts/3_Game/General Configs/Config/TrackingModShopConfig.c`

```c
class TrackingModShopItem
{
	string ItemClassName;
	string DisplayName;
	int PricePvE;      // Kosten in PvE Points (0 = nicht kaufbar mit PvE)
	int PricePvP;      // Kosten in PvP Points (0 = nicht kaufbar mit PvP)
	int MaxPerPurchase; // Max Anzahl pro Kauf
	ref array<ref TrackingModRewardAttachment> Attachments;
}

class TrackingModShopCrate
{
	string CrateName;
	string DisplayName;
	int PricePvE;
	int PricePvP;
	ref array<ref TrackingModRewardItem> PossibleItems;
	int ItemsMin;
	int ItemsMax;
}

class TrackingModShopConfig
{
	bool EnableShop;
	string PointType;  // "PvE", "PvP", "Both"
	ref array<ref TrackingModShopItem> ShopItems;
	ref array<ref TrackingModShopCrate> ShopCrates;
}
```

Config-Datei: `$profile:Ninjins_Tracking_Mod\Data\ShopConfig.json`

**Schritt 2 — Shop Layout**

Neue Datei: `gui/layouts/Shop/trackingModShop.layout`

Aufbau:
```
+--[ Reward Shop ]-----------------------------+
| Deine Points: PvE: XXXX | PvP: XXXX         |
|----------------------------------------------|
| [Items]  [Crates]   (Tab-Buttons)            |
|----------------------------------------------|
| Icon | Name        | Preis | [Kaufen]        |
| Icon | Name        | Preis | [Kaufen]        |
| ...                                          |
|----------------------------------------------|
| [Schliessen]                                 |
+----------------------------------------------+
```

**Schritt 3 — Shop Menu Klasse**

Neue Datei: `scripts/5_Mission/TrackingModUI/Shop/TrackingModShopMenu.c`

- Erbt von `ScriptViewMenu` (MVC Pattern wie bestehende Menus)
- Laedt Shop-Config per RPC vom Server
- Zeigt Items/Crates mit Preisen
- Kaufen-Button: sendet Kauf-RPC an Server
- Mouse-Interaktion: `OnClick()` Handler fuer alle Buttons

Neue Datei: `scripts/5_Mission/TrackingModUI/Shop/TrackingModShopController.c`

**Schritt 4 — Shop RPCs**

Neue RPCs in `MissionServer.c`:
```
"RequestShopConfig"  — Client->Server
"PurchaseShopItem"   — Client->Server (Param3<string, int, bool> itemClass, amount, usePvEPoints)
"PurchaseShopCrate"  — Client->Server (Param2<string, bool> crateName, usePvEPoints)
```

Response:
```
"ReceiveShopConfig"       — Server->Client (ShopConfig Daten)
"ReceiveShopPurchaseResult" — Server->Client (bool success, string message, int remainingPoints)
```

**Schritt 5 — Server Kauf-Logik**

1. Pruefe ob Spieler genug Points hat
2. Points abziehen
3. Item(s) erzeugen (Inventar oder Boden)
4. PlayerData speichern
5. Erfolg/Fehler-Notification

**Schritt 6 — Zugang zum Shop**

Option A: Button im Leaderboard-Menu ("Shop" Tab neben PvE/PvP)
Option B: Eigener Keybind in `data/modded_inputs.xml`
Option C: Beides

Empfehlung: Button im bestehenden Leaderboard-Menu ist am einfachsten. Neuen Tab-Button neben den bestehenden Tabs.

---

### AGENT 8: Configurable & Sortable Columns

**Ziel:** Server-Owner koennen Spalten ein/ausblenden und die Reihenfolge aendern. Spieler koennen per Klick auf Spaltenheader sortieren.

**Schritt 1 — Column Config**

Datei: `scripts/3_Game/General Configs/Config/TrackingModConfig.c`

Neue Klasse:
```c
class TrackingModColumnConfig
{
	string ColumnID;       // z.B. "ZombieKills", "ShotsFired", "KDRatio"
	string DisplayName;    // z.B. "Zombie Kills", "Shots Fired"
	bool Visible;          // true/false
	int SortOrder;         // 0, 1, 2, ... (Reihenfolge)
}
```

Neue Felder in `TrackingModConfig`:
```c
ref array<ref TrackingModColumnConfig> PVEColumns;
ref array<ref TrackingModColumnConfig> PVPColumns;
```

Default-Werte: Alle bestehenden Spalten sichtbar, Reihenfolge wie aktuell.

Verfuegbare Column-IDs:
```
PvE: "Position", "Name", "ZombieKills", "AnimalKills", "AIKills", "Deaths", "PvEPoints",
     "LongestShot", "ShotsFired", "ShotsHit", "Headshots", "HeadshotPct", "Accuracy", "DistanceTravelled"

PvP: "Position", "Name", "PlayerKills", "Deaths", "PlayerDeaths", "KDRatio", "PvPPoints",
     "LongestShot", "ShotsFired", "ShotsHit", "Headshots", "HeadshotPct", "Accuracy", "DistanceTravelled"
```

**Schritt 2 — Dynamisches Layout**

Die Spalten muessen zur Laufzeit erstellt werden statt fest im Layout.

Ansatz: Im Layout eine leere Header-Zeile und Template-Entry bereitstellen. In der Menu-Klasse die Spalten dynamisch basierend auf der Config erstellen.

In `TrackingModLeaderboardMenu.c` und `TrackingModPvPLeaderboardMenu.c`:
```c
void BuildColumnHeaders()
{
	// Config laden
	// Spalten nach SortOrder sortieren
	// Nur sichtbare Spalten anzeigen
	// Header-Widgets dynamisch erzeugen oder ein/ausblenden
}
```

Fuer PlayerEntry: Gleiche Logik — nur sichtbare Spalten populieren.

**Schritt 3 — Sortierbare Spalten**

Klick auf Spalten-Header sortiert die Daten nach dieser Spalte.

```c
protected string m_NJN_CurrentSortColumn;
protected bool m_NJN_SortAscending;

void OnColumnHeaderClick(string columnID)
{
	if (m_NJN_CurrentSortColumn == columnID)
	{
		m_NJN_SortAscending = !m_NJN_SortAscending;
	}
	else
	{
		m_NJN_CurrentSortColumn = columnID;
		m_NJN_SortAscending = false;  // Absteigend als Default
	}
	SortAndRefreshList();
}
```

Sortierung: Client-seitig auf den bereits empfangenen Daten. Keine neue Server-Anfrage noetig.

Visuelles Feedback: Pfeil-Indikator (auf/ab) neben dem aktiven Spaltenheader.

**Schritt 4 — Admin Menu Integration**

Im Admin Menu: Liste der Spalten mit Checkboxen (sichtbar/unsichtbar) und Drag-and-Drop oder Hoch/Runter-Buttons fuer die Reihenfolge.

---

### AGENT 9: Style Settings System

**Ziel:** Server-Owner koennen Farben, Schriftgroessen und Layout-Appearance konfigurieren.

**Schritt 1 — Style Config**

Neue Datei: `scripts/3_Game/General Configs/Config/TrackingModStyleConfig.c`

```c
class TrackingModColorSetting
{
	int R;
	int G;
	int B;
	int A;
}

class TrackingModStyleConfig
{
	// Header
	ref TrackingModColorSetting HeaderBackgroundColor;
	ref TrackingModColorSetting HeaderTextColor;
	int HeaderFontSize;

	// Zeilen
	ref TrackingModColorSetting RowEvenColor;
	ref TrackingModColorSetting RowOddColor;
	ref TrackingModColorSetting RowTextColor;
	int RowFontSize;

	// Hervorhebung
	ref TrackingModColorSetting CurrentPlayerHighlightColor;
	ref TrackingModColorSetting OnlineIndicatorColor;
	ref TrackingModColorSetting OfflineIndicatorColor;

	// Allgemein
	ref TrackingModColorSetting BackgroundColor;
	ref TrackingModColorSetting BorderColor;
	float BackgroundOpacity;
	string FontName;          // z.B. "gui/fonts/amorserifpro"

	// Titel
	ref TrackingModColorSetting TitleColor;
	int TitleFontSize;
	string LeaderboardTitle;  // "Ninjins Leaderboard" oder custom
}
```

Config-Datei: `$profile:Ninjins_Tracking_Mod\Data\StyleConfig.json`

Globale Referenz: `ref TrackingModStyleConfig g_TrackingModStyleConfig`

**Schritt 2 — Style anwenden**

In den Menu-Klassen (`TrackingModLeaderboardMenu.c`, `TrackingModPvPLeaderboardMenu.c`):

```c
void ApplyStyleConfig()
{
	if (!g_TrackingModStyleConfig)
		return;

	// Header
	Widget header = layoutRoot.FindAnyWidget("header_panel");
	if (header)
		header.SetColor(ARGB(style.HeaderBackgroundColor.A, style.HeaderBackgroundColor.R, style.HeaderBackgroundColor.G, style.HeaderBackgroundColor.B));

	// Zeilen-Farben in PlayerEntry weitergeben
	// usw.
}
```

Aufruf: Nach Layout-Load und nach jeder Page-Aktualisierung.

**Schritt 3 — PlayerEntry Styling**

In `TrackingModPlayerEntry.c` und `TrackingModPvPPlayerEntry.c`:

Alternating Row Colors:
```c
void ApplyRowStyle(int rowIndex)
{
	TrackingModStyleConfig style = g_TrackingModStyleConfig;
	if (!style) return;

	TrackingModColorSetting rowColor;
	if (rowIndex % 2 == 0)
		rowColor = style.RowEvenColor;
	else
		rowColor = style.RowOddColor;

	// Widget-Farbe setzen
}
```

**Schritt 4 — Style per RPC senden**

Style-Config muss vom Server an Clients gesendet werden (wie die anderen Configs).

Neuer RPC:
```
"RequestStyleConfig"  — Client->Server
"ReceiveStyleConfig"  — Server->Client
```

Alternativ: Style-Config zusammen mit dem Leaderboard-Request senden (weniger RPCs).

**Schritt 5 — Defaults**

Sinnvolle Defaults die dem aktuellen Look entsprechen:
- Dunkler Hintergrund (DayZ-typisch)
- Helle Schrift
- Gruenes Online / Rotes Offline (bereits vorhanden)
- Goldenes Highlight fuer eigenen Spieler

---

## Ausfuehrungsreihenfolge

```
1. Starte Phase 1: Agent 1 + Agent 2 + Agent 3 parallel
2. Warte bis Phase 1 fertig
3. Compile-Test: Alle Dateien muessen fehlerfrei kompilieren
4. Starte Phase 2: Agent 4 + Agent 5 + Agent 6 parallel
5. Warte bis Phase 2 fertig
6. Compile-Test
7. Starte Phase 3: Agent 7 + Agent 8 + Agent 9 parallel
8. Warte bis Phase 3 fertig
9. Finaler Compile-Test + Integration-Check

Zwischen jeder Phase: Code Review Agent laufen lassen
```

## Wichtige Regeln fuer alle Agents

1. **Enforce Script Syntax:** Kein Ternary, kein `delete`, kein `ref` auf Locals, kein `auto`/`var`, keine Multi-Line Function Calls
2. **Tabs** fuer Indentation, Opening Braces auf gleicher Zeile
3. **`override` + `super.Method()` first** in jedem Override
4. **Null-Checks** nach Cast, GetParent, GetIdentity, g_Game
5. **Backward-Kompatibilitaet:** Alte JSON-Dateien muessen weiter laden (neue Felder = Defaults)
6. **NJN_ Prefix** fuer alle neuen Klassen, `m_NJN_` fuer Member, `ENJN_` fuer Enums
7. **Referenzen pruefen:** Vanilla Source in `C:\Users\Administrator\Desktop\Mod Repositories\DAYZ_Enforce-Script-main\DayZGame\DayZ-1.29.161219.md` und EnScript Reference in `C:\Users\Administrator\Desktop\Mod Repositories\DAYZ_Enforce-Script-main\`
8. **Layer-Regeln:** 3_Game kann keine Entities erstellen. Entity-Logik in 4_World. GUI in 5_Mission.
9. **RPC:** CF RPC Pattern nutzen (String-basiert). Param-Klassen als einzelnes Objekt lesen.
10. **Kein GetGame()** — immer `g_Game`. Kein `IsClient()`/`IsServer()` — immer `IsDedicatedServer()`.
