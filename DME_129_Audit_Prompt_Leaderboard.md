# Ninjins_LeaderBoard - DayZ 1.29 Kompatibilitaets-Audit (Multi-Agent Orchestration)

## Auftrag

Fuehre ein vollstaendiges Code-Audit des Mods **Ninjins_LeaderBoard** durch, um alle Fehlerquellen und Inkompatibilitaeten mit **DayZ 1.29+** zu finden. Nutze dafuer parallele Sub-Agenten. Aendere in Phase 1 und 2 NICHTS - nur analysieren und reporten.

## Referenz-Wissensbasis (autoritativ)

Alle Regeln und Patterns stammen aus:
`C:\Users\Administrator\Desktop\Mod Repositories\DAYZ_Enforce-Script-main\`

Lies bei Bedarf die Dateien in diesen Unterordnern:
- `Tips/` - Alle EnScript Best Practices und Common Pitfalls
- `How-To/` - RPC, Actions, Menus, ModStorage, Profile Settings, etc.
- `Frameworks/` - CF, Expansion, Dabs Framework Referenz
- `DayZGame/DayZ-1.29.161219.md` - Breaking Changes 1.29

## Mod-Verzeichnis (Ziel des Audits)

```
C:\Users\Administrator\Desktop\Ninjin_Leaderboard\Ninjins_LeaderBoard\scripts\
```

### Vollstaendige Dateiliste

**3_Game/**
```
Admin/TrackingModAdminData.c
General Configs/Config/DeathCategoryConfig.c
General Configs/Config/PVECategoryConfig.c
General Configs/Config/PVPCategoryConfig.c
General Configs/Config/TrackingModConfig.c
General Configs/Config/TrackingModRewardConfig.c
General Configs/Data/Ninjins_Tracking_Mod_Data.c
General Configs/Data/TrackingModLeaderboardData.c
General Configs/Rewards/TrackingModMilestoneHelper.c
General Configs/Rewards/TrackingModMilestoneRewardLoader.c
General Configs/Utils/TrackingModLogger.c
General Configs/Utils/TrackingModWebExportHelper.c
General Configs/Utils/TrackingModZoneUtils.c
TrackingModUI/TrackingModLeaderboardData.c
TrackingModUI/TrackingModLogger.c
```

**4_World/**
```
entities/DayZPlayerImplementThrowing.c
entities/creatures/ (alle .c Dateien)
entities/itembase/  (alle .c Dateien)
entities/manbase/   (alle .c Dateien)
Integration/trackingmodwarhardlinesync.c
TrackingModRewardHelper.c
Zone Related/TrackingModZoneHandler.c
```

**5_Mission/**
```
MissionServer.c
TrackingModUI/Admin/TrackingModAdminMenu.c
TrackingModUI/Admin/TrackingModAdminMenuController.c
TrackingModUI/PvE/TrackingModLeaderboardController.c
TrackingModUI/PvE/TrackingModLeaderboardMenu.c
TrackingModUI/PvE/TrackingModPlayerEntry.c
TrackingModUI/PvE/TrackingModPlayerEntryController.c
TrackingModUI/PvP/TrackingModPvPLeaderboardController.c
TrackingModUI/PvP/TrackingModPvPLeaderboardMenu.c
TrackingModUI/PvP/TrackingModPvPPlayerEntry.c
TrackingModUI/PvP/TrackingModPvPPlayerEntryController.c
TrackingModUI/TrackingModKillCategory.c
TrackingModUI/TrackingModKillCategoryController.c
TrackingModUI/TrackingModLeaderboardInputHandler.c
```

### Mod-spezifische Rahmenbedingungen

- **RPC-System**: Dieses Mod nutzt **CF RPC** (`GetRPCManager()`) - NICHT natives `g_Game.RPC()`
- **UI-Pattern**: **ScriptViewMenu + ViewController** (MVC) - NICHT `UIScriptedMenu`
- **JSON-Storage**: `JsonFileLoader<T>` gegen `$profile:Ninjins_Tracking_Mod\Data\` (server-only)
- **Klassen-Prefix**: `TrackingMod` fuer alle Klassen, Enums ohne gesonderten Enum-Prefix
- **Frameworks**: Community Framework (CF) und DayZ Expansion muessen vorausgesetzt werden

---

## Phase 1: Parallele Analyse - Starte diese 5 Agenten GLEICHZEITIG

---

### Agent 1: GetGame() -> g_Game Migration

**Aufgabe:** Finde ALLE `GetGame()` Aufrufe und alle unzuverlaessigen Client/Server-Checks im gesamten Mod.

**Regeln (aus `Tips/Tips-g_Game-GetGame.md`):**
- `GetGame()` ist DEPRECATED - darf NICHT mehr verwendet werden ab DayZ 1.29
- Ersetze JEDEN `GetGame()` Aufruf mit `g_Game`
- Vor JEDER `g_Game` Nutzung MUSS ein Null-Check stehen: `if (!g_Game) return;`
- Bei inline-Nutzung: `if (g_Game && g_Game.IsDedicatedServer())`
- Bei mehrfacher Nutzung in einer Funktion: einmal am Anfang pruefen, dann sicher verwenden
- `IsClient()` und `IsServer()` sind UNZUVERLAESSIG - NUR `IsDedicatedServer()` verwenden

**Suche nach (in allen .c Dateien des Mods):**
```
GetGame()
GetGame().
DayZGame game = GetGame()
GetGame().IsClient()
GetGame().IsServer()
g_Game.IsClient()
g_Game.IsServer()
.IsClient()
.IsServer()
```

**Fuer jede Fundstelle zeige:**
1. Datei + Zeilennummer + aktuellen Code
2. Korrigierten Code mit `g_Game` + passendem Null-Check
3. Falls `IsClient()` -> Ersetze durch `!g_Game.IsDedicatedServer()`
4. Falls `IsServer()` -> Ersetze durch `g_Game.IsDedicatedServer()`

**Korrektur-Beispiele:**
```c
// VORHER (falsch):
if (GetGame().IsDedicatedServer()) { ... }
GetGame().GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MyFunc, 1000, false);

// NACHHER (korrekt):
if (!g_Game) return;
if (g_Game.IsDedicatedServer()) { ... }
g_Game.GetCallQueue(CALL_CATEGORY_GAMEPLAY).CallLater(MyFunc, 1000, false);

// VORHER (unzuverlaessig):
if (GetGame().IsClient()) { ... }
if (GetGame().IsServer()) { ... }

// NACHHER (zuverlaessig):
if (!g_Game.IsDedicatedServer()) { ... }   // Client-Check
if (g_Game.IsDedicatedServer()) { ... }    // Server-Check
```

**Hinweis fuer diesen Mod:** Der Mod verwendet CF RPC und ScriptViewMenu. Pruefe insbesondere:
- `MissionServer.c` - Server-Checks bei RPC-Handlern
- `TrackingModLeaderboardInputHandler.c` - Client-Checks beim Empfang
- `TrackingModAdminMenu.c` - UI-seitige g_Game Nutzung
- Alle `Zone Related/` und `Integration/` Dateien auf falsche IsServer/IsClient-Checks

---

### Agent 2: EnScript Syntax & Common Pitfalls

**Aufgabe:** Pruefe den gesamten Mod auf verbotene Syntax und bekannte EnScript-Fallen.

**Pruefe auf diese Fehler (aus `Tips/Tips-Common-Pitfalls.md`, `Tips/Tips-Code-Structure.md`):**

1. **Ternary-Operator** (`? :`) - EnScript unterstuetzt KEINEN Ternary. Suche nach Pattern `= expr ? val1 : val2`. Jede Fundstelle -> if-else Ersatz zeigen.

2. **Multi-Variable-Deklarationen** - `int a, b, c;` oder `string x, y;` verursacht Compile-Fehler. Jede Variable muss einzeln deklariert werden.

3. **Mehrzeilige Funktionsaufrufe** - Funktionsaufrufe duerfen NICHT ueber mehrere Zeilen gebrochen werden. Parameter in temporaere Variablen extrahieren wenn noetig.

4. **`delete` Keyword** - NIEMALS verwenden. Stattdessen `= null` setzen.

5. **`auto` Keyword** - Nicht unterstuetzt in vanilla EnScript.

6. **Optional Chaining `?.`** - Nicht unterstuetzt.

7. **Null Coalescing `??`** - Nicht unterstuetzt.

8. **Lambdas / Closures** - Nicht unterstuetzt.

9. **Leere `#ifdef/#endif` Bloecke** - Verursachen Segfaults. Muessen mindestens ein Statement enthalten.

10. **Variablen-Redekleration** - Gleicher Variablenname in verschachteltem Scope verursacht "Variable already declared" Fehler.

11. **String-Konkatenation in Array-Index-Ausdrucken** - Kann Segfaults erzeugen. Temporaere Variable verwenden.

12. **`foreach` mit Index-Modifikation** - NIEMALS das iterierte Array innerhalb von `foreach` veraendern (Add/Remove).

**Mod-spezifische Schwerpunkte:**
- `Ninjins_Tracking_Mod_Data.c` und `TrackingModLeaderboardData.c` - Datenstrukturen auf Multi-Var-Deklarationen pruefen
- `TrackingModConfig.c` / `PVECategoryConfig.c` / `PVPCategoryConfig.c` - JSON-Laden-Logik auf Syntax-Fallen
- `TrackingModMilestoneHelper.c` / `TrackingModMilestoneRewardLoader.c` - foreach-Schleifen mit Array-Manipulation

**Fuer jede Fundstelle zeige:** Datei + Zeile + aktueller Code + korrigierter Code.

---

### Agent 3: Memory Management, ref & Crash-Risiken

**Aufgabe:** Pruefe `ref` Verwendung, Memory-Patterns und bekannte Crash-Ursachen.

**Regeln (aus `Tips/Tips-Memory-Management.md`, `Tips/Tips-Common-Pitfalls.md`, `Tips/Tips-Best-Practices.md`):**

**ref-Regeln:**
- `ref` auf Member-Variablen: PFLICHT fuer Objekt-Referenzen (verhindert vorzeitige GC)
- `ref` auf Parameter: VERBOTEN (Compile-Fehler)
- `ref` auf Return-Types: VERBOTEN (Compile-Fehler)
- `ref` auf lokale Variablen: VERBOTEN (Compile-Fehler)
- `delete obj;` VERBOTEN - verwende `obj = null;`

**Crash-Risiken:**
- Komplexe Ausdruecke direkt in Array-Index-Zuweisung -> Segfault:
  ```c
  // FALSCH (Segfault):
  m_Array[i] = SomeFunc() <= value;
  // RICHTIG:
  bool temp = SomeFunc() <= value;
  m_Array[i] = temp;
  ```
- `GetObjectsAtPosition` / `GetObjectsAtPosition3D` -> NIEMALS verwenden
- Null-Pointer nach `.Cast()` ohne Check
- Null-Pointer nach `GetParent()`, `FindAttachment()`, `GetIdentity()` ohne Check
- `new` in tight Loops -> GC-Druck, Variable ausserhalb deklarieren
- `g_Game.GetCallQueue()` kann null sein - immer pruefen
- `1 < int.MIN` gibt TRUE in EnScript - Integer-Grenzwert-Vergleiche vermeiden

**Mod-spezifische Schwerpunkte:**
- `Ninjins_Tracking_Mod_Data.c` - `ref array<ref TrackingModPlayerData>` und aehnliche verschachtelte Referenzketten
- `TrackingModAdminData.c` - Neu hinzugefuegte PVE/PVP Datenklassen mit `ref array<ref PVECategory>` etc.
- `TrackingModMilestoneRewardLoader.c` - On-demand Laden von Reward-Dateien: NULL-Check nach JsonLoadFile
- `TrackingModRewardHelper.c` - Reward-Zuweisung: Entity-Null-Checks nach `GetInventory()`, `FindAttachment()`
- `TrackingModZoneHandler.c` - Zone-Lookup: Null-Checks nach `GetObjectsAtPosition3D` (falls verwendet)
- `trackingmodwarhardlinesync.c` - Integration mit Expansion Hardline: Cast-Checks
- `entities/manbase/` und `entities/creatures/` - Override-Methoden: `super.` Aufrufe vor GC-sensiblen Operationen

**Fuer jede Fundstelle zeige:** Datei + Zeile + Problem + Fix-Vorschlag.

---

### Agent 4: CF RPC System & Netzwerk-Korrektheit

**Aufgabe:** Pruefe alle RPC-Implementierungen auf Korrektheit. Dieser Mod nutzt **Community Framework (CF) RPC**, NICHT natives `g_Game.RPC()`.

**Regeln (aus `Frameworks/Community-Framework/CF/How-To-CF-RPC.md`, `How-To/How-To-RPC.md`):**

**CF RPC Grundregeln:**
- MUSS `GetRPCManager().AddRPC(...)` zur Registrierung verwenden
- MUSS `GetRPCManager().SendRPC(...)` fuer das Senden verwenden
- NIEMALS `g_Game.RPC()` fuer CF-registrierte RPCs mischen
- Registrierung MUSS vor dem ersten Aufruf erfolgen (typisch in `Init()` oder `OnInit()`)
- Server-RPCs werden in `MissionServer.c` registriert
- Client-RPCs werden in `TrackingModLeaderboardInputHandler.c` registriert (oder analog)

**RPC-Handler Signatur (CF):**
```c
// CF RPC Handler - korrekte Signatur:
void MyRPCHandler(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)

// Parameter lesen:
Param1<TrackingModGeneralAdminData> params = new Param1<TrackingModGeneralAdminData>(new TrackingModGeneralAdminData());
if (!ctx.Read(params)) return;
TrackingModGeneralAdminData data = params.param1;
```

**Param-Klassen Regeln:**
- `Param1<T>` fuer 1 Parameter, `Param2<T1,T2>` fuer 2, `Param3<T1,T2,T3>` fuer 3
- Typen `T` MUESSEN via `JsonSerializer` serialisierbar sein (primitive, Klassen mit `GetType()`, Arrays)
- Komplexe verschachtelte Klassen als RPC-Payload koennen silent scheitern - pruefen
- `ctx.Read()` Rueckgabewert MUSS geprueft werden: `if (!ctx.Read(params)) { return; }`

**CallType Checks:**
- Server-Handler: `if (type != CallType.Server) return;`
- Client-Handler: `if (type != CallType.Client) return;`
- NIEMALS ohne CallType-Check - kein Fehler, einfach falsches Verhalten

**Null-Checks:**
- `PlayerIdentity` vor Zugriff (sender kann null sein bei Broadcast)
- Alle gelesenen Parameter nach `ctx.Read()` pruefen
- `GetRPCManager()` selbst sollte nicht null sein, aber defensiv pruefen wenn fraglich

**RPC-Namen Konsistenz:**
- AddRPC-Name (Server) MUSS exakt dem SendRPC-Name (Client) entsprechen und umgekehrt
- Tippfehler in RPC-Namen fuehren zu silent failures - KEINE Fehlermeldung

**Pruefe diese Dateien mit hoher Prioritaet:**
```
5_Mission/MissionServer.c
5_Mission/TrackingModUI/TrackingModLeaderboardInputHandler.c
```

**Konkrete RPC-Kette, die zuletzt modifiziert wurde (pruefe besonders sorgfaeltig):**
```
Client sendet:  "RequestAdminConfig"    -> Server empfaengt und baut Daten
Server sendet:  "ReceiveAdminConfig"    -> Client empfaengt (Param1<TrackingModGeneralAdminData>)
Server sendet:  "ReceiveAdminPVEConfig" -> Client empfaengt (Param1<TrackingModPVEAdminData>)
Server sendet:  "ReceiveAdminPVPConfig" -> Client empfaengt (Param1<TrackingModPVPAdminData>)
Client sendet:  "SaveAdminConfig"       -> Server: nur General-Settings speichern
Client sendet:  "SaveAdminFullConfig"   -> Server: Param3<General,PVE,PVP> - alle speichern
Server sendet:  "ReceiveAdminConfigSaved" -> Client bestaetigung
```

**Pruefe fuer jede dieser RPCs:**
1. Korrekte AddRPC-Registrierung auf BEIDEN Seiten (Server + Client)
2. Korrekte CallType-Checks in den Handlern
3. Korrekte `ctx.Read()` mit Rueckgabewert-Pruefung
4. Dass `Param3<TrackingModGeneralAdminData, TrackingModPVEAdminData, TrackingModPVPAdminData>` serialisierbar ist
5. Null-Checks auf alle gelesenen Datenobjekte

**Fuer jede Fundstelle zeige:** Datei + Zeile + Problem + korrigierter Code.

---

### Agent 5: ScriptViewMenu, ViewController, Layout-Bindings & Script-Layer

**Aufgabe:** Pruefe den UI-Code (ScriptViewMenu/ViewController), Layout-Widget-Bindungen, JSON-Config-Loading und Script-Layer-Verletzungen.

#### 5a: ScriptViewMenu / ViewController Pattern

**Regeln (aus `How-To/How-To-UI-Menus.md`, `How-To/How-To-Layout-Controls.md`):**

**ScriptViewMenu Regeln:**
- Klasse MUSS von `ScriptViewMenu` erben
- `GetLayoutFile()` MUSS den korrekten Layout-Pfad zurueckgeben
- Widget-Caching in `OnWidgetScriptInit()` oder `Init()` mit `FindAnyWidget("name")`
- Widget-Name in `FindAnyWidget()` MUSS exakt mit dem Namen im `.layout`-File uebereinstimmen (case-sensitive)
- `Relay_Command` in Layout: Methodenname MUSS in der gebundenen Klasse (`scriptclass`) existieren

**ViewController Regeln:**
- Controller muss via `GetDayZGame().GetUIManager().ShowScriptedMenu()` oder `ScriptViewMenu`-System instanziiert werden
- Kein direktes `new TrackingModAdminMenuController()` ohne Registrierung

**Layout-Widget-Bindungen pruefen:**
```
gui/layouts/Admin/trackingModAdminMenu.layout
```
- Fuer jeden `Relay_Command`-Wert: Existiert die Methode in `TrackingModAdminMenu.c`?
- Fuer jede Widget-Referenz in `FindAnyWidget("name")`: Existiert das Widget mit exakt diesem Namen im Layout?
- `scriptclass` im Layout-Root: Zeigt auf die korrekte C-Klasse?
- `content_container` Position: Pruefe ob `position 0 50` mit `vexactpos 1` (Pixel-Y) korrekt ist oder ob dadurch Inhalt ausserhalb des sichtbaren Bereichs gerendert wird

**Bekanntes Problem (pruefe zuerst):**
Der `content_container` im Admin-Menu-Layout hat laut letzter Analyse:
```
position 0 50
vexactpos 1     <- Y=50 ist in Pixel, nicht relativ (0-1)
hexactpos 0
size 1 0.82
```
Dies kann dazu fuehren, dass ALLE Tab-Inhalte ausserhalb des sichtbaren Bereichs gerendert werden.
Vergleiche mit dem Leaderboard-Layout (`trackingModLeaderboard_mvc.layout`) um den korrekten Wert zu bestimmen.

**Pruefe alle Layout-Dateien in:**
```
gui/layouts/Admin/
gui/layouts/        (alle .layout Dateien)
```

#### 5b: JSON Config Loading (Server-only)

**Regeln (aus `How-To/How-To-Profile-Settings.md`, `How-To/How-To-Validate-Config-Data.md`):**

- `JsonFileLoader<T>.JsonLoadFile()` und `JsonSaveFile()` sind **NUR server-seitig** valide
- NIEMALS auf Client-Seite ohne `IsDedicatedServer()`-Guard aufrufen
- Pfade mit `$profile:` Prefix: Exakt pruefen (`$profile:Ninjins_Tracking_Mod\Data\`)
- Rueckgabewert von `JsonLoadFile()` MUSS geprueft werden:
  ```c
  if (!JsonFileLoader<TrackingModConfig>.JsonLoadFile(path, config)) {
      // Fallback oder Default erstellen
  }
  ```
- Ordner-Existenz pruefen bevor in Unterordner geschrieben wird:
  ```c
  MakeDirectory(TRACKING_MOD_DATA_DIR);
  ```

**Pruefe diese Dateien:**
```
3_Game/General Configs/Config/TrackingModConfig.c
3_Game/General Configs/Config/PVECategoryConfig.c
3_Game/General Configs/Config/PVPCategoryConfig.c
3_Game/General Configs/Config/DeathCategoryConfig.c
3_Game/General Configs/Config/TrackingModRewardConfig.c
3_Game/General Configs/Rewards/TrackingModMilestoneRewardLoader.c
```

Pruefe:
1. Ist jeder `JsonLoadFile`/`JsonSaveFile` Aufruf hinter einem `IsDedicatedServer()`-Guard?
2. Wird der Rueckgabewert von `JsonLoadFile` geprueft?
3. Wird `MakeDirectory()` aufgerufen bevor Unterordner beschrieben werden?
4. Sind Pfade korrekt (einfache Backslashes, nicht doppelte)?

#### 5c: Script Layer Regeln

**Regeln (aus `How-To/Script-Layers-Guide.md`, `Tips/Tips-Modded-Classes.md`):**

- `3_Game/` - Nur Konstanten, Enums, Daten-Container-Klassen, Config-Klassen
- `4_World/` - `modded class PlayerBase`, `modded class EntityAI`, `modded class ItemBase`, Actions, Zone-Handler
- `5_Mission/` - `MissionServer`, `MissionGameplay`, alle UI-Klassen (ScriptViewMenu), alle RPC-Handler
- **Niedrigere Layer duerfen KEINE hoehere Layer referenzieren** (z.B. 3_Game darf NICHT 5_Mission referenzieren)

**Pruefe auf Layer-Verletzungen:**
- `3_Game/TrackingModUI/` - Enthaelt UI-bezogene Datei(en) in 3_Game. Sind das wirklich nur Daten-Klassen (ok) oder UI-Logik (nicht ok)?
- `3_Game/General Configs/Utils/TrackingModWebExportHelper.c` - Schreibt in Dateien? Nur server-side? Gibt es Layer-Probleme?
- `4_World/TrackingModRewardHelper.c` - Referenziert dieser 5_Mission-Klassen?

#### 5d: Modded Classes & Naming

**Regeln (aus `Tips/Tips-Modded-Classes.md`, `Tips/Tips-Override-Keyword.md`, `Tips/Tips-Prefixes-Naming.md`):**

- `modded class X` darf KEINE Vererbungssyntax haben (`: SomeClass`)
- `override` ist PFLICHT auf allen ueberschriebenen Methoden
- `super.Method()` muss aufgerufen werden (besonders bei Serialisierung und Lifecycle-Hooks)
- Member-Variablen in modded Classes MUESSEN Mod-Prefix haben: `m_TrackingMod_VarName`

**Pruefe alle Dateien in:**
```
4_World/entities/creatures/
4_World/entities/manbase/
4_World/entities/itembase/
4_World/entities/DayZPlayerImplementThrowing.c
4_World/Integration/trackingmodwarhardlinesync.c
```

Pruefe:
1. Fehlende `override` Keywords (FIX-ME Warnings im Log)
2. `modded class X : SomeClass` - verbotene Vererbungs-Syntax
3. Fehlende `super.Method()` Aufrufe
4. Member-Variablen ohne Mod-Prefix in modded Classes

**config.cpp Regeln:**
```
Ninjins_LeaderBoard/config.cpp
```
- `requiredVersion` muss `0.1` sein
- `requiredAddons[]` nur `"DZ_Data"` (und CF/Expansion falls hardte Dependency)
- `files[]` Pfade muessen mit tatsaechlicher Ordnerstruktur uebereinstimmen
- Pfade in config.cpp: **einfache Backslashes** (KEINE doppelten `\\`)

**Fuer jede Fundstelle zeige:** Datei + Zeile + Problem + Fix.

---

## Phase 2: Zusammenfuehrung & Priorisierte Fix-Liste

Nachdem alle 5 Agenten fertig sind, erstelle:

### 1. Zusammenfassung
- Anzahl Probleme pro Agent/Kategorie
- Betroffene Dateien auflisten

### 2. Priorisierte Fix-Liste
Sortiert nach Schwere:

| Prio | Kategorie | Beschreibung |
|------|-----------|-------------|
| P0 | Compile-Fehler | Blockiert Build komplett (Ternary, Multi-Var, delete, auto, falsches ref) |
| P1 | Segfault/Crash | Runtime-Crashes (leere ifdef, Array-Segfault, fehlende Null-Checks, GetObjectsAtPosition3D) |
| P2 | 1.29 Breaking | GetGame() Migration, IsClient/IsServer Ersatz |
| P3 | Silent Failures | Fehlende RPC-Registrierung, CallType-Checks, ctx.Read-Pruefung, fehlendes `override` |
| P4 | Layout/UI Bugs | Widget-Namen-Mismatch, falsche Relay_Command-Bindings, content_container Position |
| P5 | Best Practices | ref-Korrekturen, Naming, Layer-Fixes, JSON-Guards |

### 3. Frage den User
- Sollen alle Fixes automatisch angewendet werden?
- Oder Kategorie fuer Kategorie mit Review dazwischen?
- Oder nur bestimmte Prioritaeten (Empfehlung: P0-P3 zuerst, dann P4 fuer den Admin-Menu-Bug)?

---

## Phase 3: Fixes anwenden (NUR nach User-Bestaetigung)

Wende Fixes in dieser Reihenfolge an:
1. P0: Compile-Fehler beheben
2. P1: Crash-Risiken eliminieren
3. P2: GetGame() -> g_Game Migration
4. P3: Silent Failures fixen (RPC, Override)
5. P4: Layout/UI Bugs fixen (content_container, Widget-Bindings)
6. P5: Best Practices anwenden

Nutze parallele Agents (max 5) um mehrere Dateien gleichzeitig zu fixen.
Nach allen Fixes: `git diff` zeigen zur finalen Review.

---

## Absolute Regeln fuer ALLE Agents

1. **Phase 1+2: NUR LESEN** - Keine Dateien aendern
2. **Vollstaendige Fundstellen**: Datei + Zeile + aktueller Code + vorgeschlagener Fix
3. **Keine Logik-Aenderungen**: Nur syntaktische/API-Korrekturen
4. **Referenz-Dateien nutzen**: Bei Unsicherheit die Dateien in `DAYZ_Enforce-Script-main/` lesen
5. **Keine falschen Positiven**: Nur echte, verifizierte Probleme melden
6. **Kontext beachten**: Code im Kontext der umgebenden Funktion lesen, nicht isoliert
7. **Bestehende Funktionalitaet erhalten**: Keine neuen Features, kein Refactoring
8. **Server/Client Replikation**: Pruefen ob Aenderungen die Authority-Regeln brechen koennten
9. **CF RPC != native RPC**: NIEMALS `g_Game.RPC()` als Fix fuer CF-RPC-Probleme vorschlagen
10. **ScriptViewMenu != UIScriptedMenu**: Nicht die UI-Pattern mischen
