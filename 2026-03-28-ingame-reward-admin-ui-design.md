# In-Game Admin UI fuer Reward- und Leaderboard-Konfiguration

## Uebersicht

Ein vollstaendiges In-Game Admin-Menue fuer den Ninjins Tracking Mod, das es Admins ermoeglicht, alle Reward- und Leaderboard-Einstellungen direkt im Spiel zu konfigurieren, ohne JSON-Dateien manuell bearbeiten zu muessen. Aenderungen werden sofort live angewendet (Server-Reload).

## Zielgruppe

- Nur Server-Admins (SteamIDs aus `LeaderBoardConfig.json > AdminIDs`)
- Sprache: Deutsch
- Kein Concurrent-Editing-Handling noetig

## Zugang

- Ein "Einstellungen"-Button erscheint im bestehenden PvE- und PvP-Leaderboard
- Nur sichtbar wenn die SteamID des Spielers in `AdminIDs` steht
- Server validiert bei jedem Speichervorgang erneut die SteamID (Client wird nie vertraut)
- Alle Schreiboperationen laufen ueber Server-RPCs

## Hauptstruktur

- Vollbild-Overlay (80% x 80%, zentriert, wie bestehende Leaderboards)
- Titelleiste: "Tracking Mod - Admin Einstellungen"
- Tab-Leiste: **Allgemein | PvE | PvP | Milestones | Rewards**
- Inhaltsbereich (wechselt pro Tab, nur aktiver Tab geladen)
- Footer mit "Speichern & Anwenden"-Button (schreibt JSON + loest Server-Reload aus)
- Farbschema: Neutral grau/dunkel (abgegrenzt von gruen=PvE, rot=PvP)
- Architektur: Tab-basiert mit separaten Layout/Controller-Paaren pro Tab (MVC-Pattern wie bestehender Code)

## Tab 1: Allgemein (LeaderBoardConfig.json)

Scrollbares Panel mit gruppierten Einstellungen:

### Gruppe: Admin-Verwaltung
- Liste der Admin-SteamIDs mit Hinzufuegen/Entfernen-Buttons
- Freitext-Eingabe fuer neue SteamID

### Gruppe: Leaderboard
- Checkbox: PvE Leaderboard aktivieren/deaktivieren
- Checkbox: PvP Leaderboard aktivieren/deaktivieren
- Zahlenfeld: Max PvE Spieler-Anzeige
- Zahlenfeld: Max PvP Spieler-Anzeige
- Checkbox: Online-Status PvE anzeigen
- Checkbox: Online-Status PvP anzeigen

### Gruppe: Spielerdaten
- Zahlenfeld: Spielerdateien loeschen nach X Tagen
- Checkbox: UTC fuer Datumsangaben

### Gruppe: Reward-System
- Checkbox: Reward-System aktivieren/deaktivieren

### Gruppe: Death-Tracking Filter
- 10 Checkboxen: Suicide, Grenade, Trap, Zombie, Animal, AI, Car, Weapon, Unarmed, Unknown

### Gruppe: Kill-Tracking Filter
- Checkboxen fuer alle Kill-Ursachen-Filter

### Gruppe: Zonen-Einstellungen
- Checkbox: PvP-Kill ausschliessen wenn beide in PvE-Zone
- Mehrfachauswahl: In welchen Zonentypen PvE-Kills zaehlen

### Gruppe: Web-Export
- Checkbox: Web-Export aktivieren
- Zahlenfeld: Export-Intervall (Sekunden)
- Zahlenfeld: Max Spieler im Export
- Checkbox: Spieler-IDs im Export zeigen

## Tab 2: PvE (PVE_Categories.json)

### Oberer Bereich
- Zahlenfeld: PvE-Todespunkte-Abzug (Death Penalty)

### Kategorien-Liste
Scrollbare Liste aller PvE-Kategorien (aktuell 14). Jede Kategorie als aufklappbare Zeile:

**Zugeklappt:** Kategorie-Name | Anzahl Classnames | Bearbeiten/Loeschen-Buttons

**Aufgeklappt:**
- Textfeld: CategoryID
- Textfeld: ClassNamePreview (Vorschau-Classname fuer UI-Icon)
- Liste der ClassNames, jeder Eintrag als Zeile:
  - Textfeld: ClassName
  - Zahlenfeld: Basispunkte
  - Zahlenfeld: Multiplier
  - Loeschen-Button
- Button: "Classname hinzufuegen"

**Footer:** Button "Neue Kategorie hinzufuegen"

## Tab 3: PvP (PVP_Categories.json)

Identischer Aufbau wie PvE-Tab, nur mit PvP-Daten:

### Oberer Bereich
- Zahlenfeld: PvP-Todespunkte-Abzug (Death Penalty, aktuell 5)

### Kategorien-Liste
Gleiche Struktur wie PvE - aktuell 1 Kategorie (Players), aber erweiterbar. Selbes aufklappbares System mit ClassNames-Bearbeitung.

## Tab 4: Milestones (TrackingModRewardConfig.json)

### Oberer Bereich
- Checkbox: Milestone-Rewards aktivieren
- Textfeld: RewardConfigFolder-Pfad

### Zwei-Spalten-Layout

**Linke Spalte: PvE Milestones**
Scrollbare Liste aller PvE-Kategorien. Jede Kategorie aufklappbar:

- **Zugeklappt:** Kategorie-Name | Anzahl Milestones (z.B. "Zombies - 5 Stufen")
- **Aufgeklappt:** Liste der Milestone-Stufen, pro Stufe:
  - Zahlenfeld: Kill-Schwelle (z.B. 100, 1000, 5000)
  - Textfeld/Dropdown: Zugeordnete Reward-Datei (aus vorhandenen Dateien im RewardSystem-Ordner waehlbar)
  - Loeschen-Button
- Button: "Stufe hinzufuegen"

**Rechte Spalte: PvP Milestones**
Selbe Struktur, aktuell leer aber voll konfigurierbar.

Kategorie-Namen kommen dynamisch aus den PvE/PvP-Category-Configs. Neue Kategorien erscheinen automatisch.

## Tab 5: Rewards (RewardSystem/*.json)

### Linke Seite: Reward-Datei-Liste (Sidebar)
- Scrollbare Liste aller JSON-Dateien im RewardSystem-Ordner
- Suchfeld oben zum Filtern nach Dateiname
- Button: "Neue Reward-Datei erstellen"
- Button: "Reward-Datei loeschen"
- Aktive Datei hervorgehoben

### Rechte Seite: Editor fuer ausgewaehlte Reward-Datei

#### Bereich: Loot-Sektionen (RewardItems)
- Button: "Sektion hinzufuegen"
- Pro Sektion aufklappbar:
  - Textfeld: Name (z.B. "Wolf Hunter")
  - Zahlenfeld: SpawnChance (0-100%)
  - Zahlenfeld: ItemsMin / ItemsMax
  - **Item-Liste** - pro Item:
    - Textfeld: ItemClassName (mit Validierung ob Klasse im Spiel existiert)
    - Zahlenfeld: SpawnChance (0-100%)
    - Zahlenfeld: Amount
    - Zahlenfeld: QuantMin / QuantMax
    - Zahlenfeld: HealthMin / HealthMax
    - **Attachments** (rekursiv aufklappbar, gleiche Felder wie Items, beliebig verschachtelbar)
    - Button: "Attachment hinzufuegen"
  - Button: "Item hinzufuegen"
  - Loeschen-Button fuer Sektion

#### Bereich: Waehrung (CurrencyRewards)
- Zahlenfeld: CurrencyMin / CurrencyMax
- Pro Waehrungseintrag:
  - Textfeld: ClassName (z.B. `DLD_Kronkorken_100`)
  - Zahlenfeld: SpawnChance (0-100%)
  - Zahlenfeld: Amount
  - Loeschen-Button
- Button: "Waehrung hinzufuegen"

#### Footer
- Button: "Test-Reward an mich vergeben" (spawnt die Reward-Items zum Testen an den Admin)

## Technische Architektur

### Dateistruktur (neue Dateien)
```
gui/layouts/Admin/
  trackingModAdminMenu.layout              (Hauptcontainer + Tabs)
  trackingModAdminGeneral.layout           (Tab Allgemein)
  trackingModAdminPvE.layout               (Tab PvE)
  trackingModAdminPvP.layout               (Tab PvP)
  trackingModAdminMilestones.layout        (Tab Milestones)
  trackingModAdminRewards.layout           (Tab Rewards)

scripts/5_Mission/TrackingModUI/Admin/
  TrackingModAdminMenu.c                   (Hauptmenue + Tab-Wechsel)
  TrackingModAdminMenuController.c         (Hauptcontroller)
  TrackingModAdminGeneralTab.c             (Allgemein-Tab Logik)
  TrackingModAdminGeneralController.c      (Allgemein-Tab Controller)
  TrackingModAdminPvETab.c                 (PvE-Tab Logik)
  TrackingModAdminPvEController.c          (PvE-Tab Controller)
  TrackingModAdminPvPTab.c                 (PvP-Tab Logik)
  TrackingModAdminPvPController.c          (PvP-Tab Controller)
  TrackingModAdminMilestonesTab.c          (Milestones-Tab Logik)
  TrackingModAdminMilestonesController.c   (Milestones-Tab Controller)
  TrackingModAdminRewardsTab.c             (Rewards-Tab Logik)
  TrackingModAdminRewardsController.c      (Rewards-Tab Controller)

scripts/4_World/Admin/
  TrackingModAdminRPCHandler.c             (Server-seitige RPC-Verarbeitung)
  TrackingModAdminConfigWriter.c           (JSON-Schreib/Lese-Logik)
  TrackingModAdminValidator.c              (Admin-Validierung + Item-Classname-Validierung)

scripts/3_Game/Admin/
  TrackingModAdminData.c                   (Datenstrukturen fuer Admin-UI Serialisierung)
```

### RPC-Kommunikation

**Client -> Server:**
- `RequestAdminConfig` - Laedt alle Config-Daten zum Client
- `SaveAdminConfig` - Sendet geaenderte Config zum Server
- `ValidateItemClassName` - Prueft ob ein Classname im Spiel existiert
- `TestRewardSpawn` - Spawnt Test-Reward an Admin
- `RequestRewardFileList` - Liste aller Reward-JSONs
- `RequestRewardFileContent` - Inhalt einer Reward-JSON
- `SaveRewardFile` - Speichert Reward-JSON
- `DeleteRewardFile` - Loescht Reward-JSON
- `CreateRewardFile` - Erstellt neue Reward-JSON

**Server -> Client:**
- `ReceiveAdminConfig` - Config-Daten an Client
- `ReceiveAdminConfigSaved` - Bestaetigung nach Speichern
- `ReceiveItemValidation` - Ergebnis der Classname-Validierung
- `ReceiveRewardFileList` - Liste der Reward-Dateien
- `ReceiveRewardFileContent` - Inhalt einer Reward-Datei
- `ReceiveTestRewardResult` - Ergebnis des Test-Spawns

### Server-Reload nach Speichern

Nach erfolgreichem Schreiben der JSON-Dateien:
1. `TrackingModConfig` wird neu geladen
2. `PVECategoryConfig` wird neu geladen
3. `PVPCategoryConfig` wird neu geladen
4. `TrackingModRewardConfig` wird neu geladen
5. Bestaetigungsmeldung an den Admin-Client

### Item-Classname-Validierung

Validierung laeuft serverseitig:
- Server prueft via `GetGame().IsKindOf(className, "ItemBase")` ob die Klasse existiert
- Ergebnis wird per RPC zurueck an Client gesendet
- UI zeigt gruenes Haekchen (gueltig) oder rotes X (ungueltig) neben dem Eingabefeld

### Sicherheit

- Jeder RPC zum Server prueft `AdminIDs.Find(playerSteamID) != -1`
- Ungueltige Anfragen werden geloggt und ignoriert
- Kein Client-seitiges Vertrauen - alle Datenvalidierung auf dem Server
