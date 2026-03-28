# Analyseplan: Admin-UI Fehlerdiagnose

## Kontext
Pruefstand fuer das neue Admin-UI-System mit Allgemein-Tab, RPC-Anbindung und serverseitigem Speichern.
Ziel: Nur noch reale Rest-Risiken und verifizierte Aussagen dokumentieren.
Referenz: `copilot-instructions.md` — keine kosmetischen Punkte, keine bereits behobenen Probleme als offene Bugs fuehren.

---

## Bereits behoben

### A.1 Death/Kill-Filter-Labels waren missverstaendlich
- **Status:** Behoben
- **Datei:** `Ninjins_LeaderBoard/gui/layouts/Admin/trackingModAdminMenu.layout`
- **Vorher:** Checkboxen hiessen nur `Suicide`, `Grenade`, `Trap`, usw., speicherten aber `Disable...`-Flags.
- **Jetzt:** Labels sind explizit als `... ignorieren` formuliert.
- **Ergebnis:** UI-Semantik passt jetzt zur gespeicherten Config.

### A.2 Self-Lockout ueber AdminIDs war moeglich
- **Status:** Behoben
- **Dateien:**
  - `Ninjins_LeaderBoard/scripts/5_Mission/TrackingModUI/Admin/TrackingModAdminMenu.c`
  - `Ninjins_LeaderBoard/scripts/5_Mission/MissionServer.c`
- **Vorher:** Ein Admin konnte seine eigene SteamID entfernen oder eine leere `AdminIDs`-Liste speichern.
- **Jetzt:**
  - Client blockiert das Entfernen der eigenen ID.
  - Server lehnt Save ab, wenn die Liste leer ist oder der absendende Admin nicht mehr enthalten waere.
- **Ergebnis:** Kein versehentlicher Verlust des Admin-Zugangs ueber das UI.

### A.3 Wertevalidierung fuer Zahlenfelder fehlte
- **Status:** Behoben
- **Datei:** `Ninjins_LeaderBoard/scripts/5_Mission/TrackingModUI/Admin/TrackingModAdminMenu.c`
- **Jetzt begrenzt:**
  - `DeletePlayerFilesOlderThanDays`: `0..3650`
  - `MaxPVEPlayersDisplay`: `1..100`
  - `MaxPVPPlayersDisplay`: `1..100`
  - `WebExportIntervalSeconds`: `1..86400`
  - `WebExportMaxPlayers`: `1..1000`
- **Ergebnis:** Unsinnige Eingaben werden bereits im UI abgefangen.

---

## Offene Rest-Risiken

## Kategorie 1: Serialisierung / RPC (HOCH)

### 1.1 Auto-Serialisierung von `ref array<string>` muss praktisch verifiziert werden
- **Datei:** `Ninjins_LeaderBoard/scripts/3_Game/Admin/TrackingModAdminData.c`
- **Problem:** `TrackingModGeneralAdminData` enthaelt `ref array<string> AdminIDs` und `ref array<string> TrackPVEKillsInZoneTypes`.
- **Risiko:** Falls DayZ/CF diese Arrays im `Param1<T>`-RPC nicht sauber serialisiert, kommen sie clientseitig leer oder `null` an.
- **Auswirkung:**
  - Admin-Liste bleibt leer obwohl Serverdaten existieren.
  - Zone-Type-Auswahl wird nicht korrekt geladen.
- **Status:** Noch offen, nur per Laufzeittest sicher bestaetigbar.
- **Empfohlene Verifikation:** Admin-Menue oeffnen und pruefen, ob `AdminIDs` und `TrackPVEKillsInZoneTypes` korrekt ankommen.

### 1.2 Grosse Admin-Datenstruktur bleibt versionssensitiv
- **Datei:** `Ninjins_LeaderBoard/scripts/3_Game/Admin/TrackingModAdminData.c`
- **Problem:** Viele Felder in `TrackingModGeneralAdminData` erhoehen das Risiko stiller Deserialisierungsprobleme bei Client-/Server-Versionsabweichung.
- **Auswirkung:** Falsche Werte koennen in falschen Feldern landen, wenn Client und Server nicht dieselbe Mod-Version nutzen.
- **Status:** Architektur-Risiko, kein aktueller Codefehler.

---

## Kategorie 2: RPC-Verhalten (MITTEL)

### 2.1 `player.GetIdentity()` im Client-RPC sollte praktisch bestaetigt werden
- **Dateien:**
  - `Ninjins_LeaderBoard/scripts/5_Mission/TrackingModUI/Admin/TrackingModAdminMenu.c`
  - bestehende Leaderboard-RPC-Aufrufe im Mod
- **Problem:** Das Muster ist im Projekt bereits etabliert, aber fuer das Admin-Menue noch nicht praktisch bestaetigt.
- **Status:** Kein bestaetigter Bug.
- **Verifikation:** Oeffnen, laden, speichern und pruefen, ob `RequestAdminConfig` und `SaveAdminConfig` auf dem Server ankommen.

### 2.2 Zwei Antwort-RPCs direkt hintereinander koennen in wechselnder Reihenfolge ankommen
- **Datei:** `Ninjins_LeaderBoard/scripts/5_Mission/MissionServer.c`
- **Ablauf:** Nach Save sendet der Server `ReceiveAdminConfig` und danach `ReceiveAdminConfigSaved`.
- **Risiko:** Reihenfolge ist nicht garantiert.
- **Auswirkung:** Eher kosmetisch — Statusmeldung kann kurz vor oder nach dem Refresh erscheinen.
- **Status:** Kein Funktionsblocker.

---

## Kategorie 3: UI / Bedienlogik (MITTEL)

### 3.1 Doppelklick-Schutz fuer den Wechsel ins Admin-Menue fehlt weiterhin
- **Dateien:**
  - `Ninjins_LeaderBoard/scripts/5_Mission/TrackingModUI/PvE/TrackingModLeaderboardMenu.c`
  - `Ninjins_LeaderBoard/scripts/5_Mission/TrackingModUI/PvP/TrackingModPvPLeaderboardMenu.c`
- **Problem:** `OnClickSettings()` hat keinen expliziten Guard gegen sehr schnelles Mehrfachklicken.
- **Auswirkung:** Wahrscheinlich gering, aber ein Guard-Flag waere robuster.
- **Status:** Niedrige Prioritaet.

### 3.2 Admin-List- und Zone-Type-UI haengen an erfolgreicher RPC-Deserialisierung
- **Datei:** `Ninjins_LeaderBoard/scripts/5_Mission/TrackingModUI/Admin/TrackingModAdminMenu.c`
- **Problem:** Die Menuelogik selbst ist null-sicher genug, aber die Anzeige bleibt falsch oder leer, wenn die Arrays aus 1.1 nicht ankommen.
- **Status:** Folgeeffekt von 1.1, kein separater Bug.

---

## Kategorie 4: Editor-Diagnose

### 4.1 Parser-Fehlalarm fuer Enforce-Script-Datei bleibt bestehen
- **Datei:** `Ninjins_LeaderBoard/scripts/5_Mission/TrackingModUI/Admin/TrackingModAdminMenu.c`
- **Problem:** Der Editor meldet weiterhin eine kollabierte C-Parserdiagnose (`class undefined`, `expected ';'` etc.).
- **Bewertung:** Aktuell kein belastbarer Nachweis fuer einen echten Enforce-Syntaxfehler, da:
  - die Datei byte-sauber ist,
  - die betroffenen Nachbar-Dateien keine echten Compile-Probleme zeigen,
  - die Fehlerkaskade typisch fuer falsche Sprachdiagnose ist.
- **Status:** Beobachten, aber nicht als bestaetigten Codefehler fuehren.

---

## Entfernte / entkraeftete Punkte

### X.1 „Doppelte Zeilennummer in TrackingModAdminData.c"
- **Bewertung:** Kein echter Dateifehler, sondern nur ein Darstellungs- oder Notizartefakt.

### X.2 „Null-Pointer in Add/Remove ist sicher vorhanden"
- **Bewertung:** Nicht pauschal korrekt.
- **Grund:** Die Arrays werden im Konstruktor initialisiert; gefaehrlich wird es nur, wenn die RPC-Deserialisierung sie wider Erwarten `null` liefert.

### X.3 „Disable vs Enable bei PvE/PvP-Leaderboard ist fehlerhaft"
- **Bewertung:** Falsch.
- **Grund:** Die Negierung fuer `DisablePVELeaderboard` und `DisablePVPLeaderboard` ist korrekt implementiert.

---

## Empfohlene Priorisierung

| Prio | Thema | Risiko | Aktion |
|------|-------|--------|--------|
| 1 | Array-RPC-Serialisierung | Funktionsblockierend | Laufzeittest fuer `AdminIDs` und `TrackPVEKillsInZoneTypes` |
| 2 | Admin-RPC End-to-End | Mittel | Oeffnen, laden, speichern und Reload bestaetigen |
| 3 | Doppelklick-Schutz | Niedrig | Optionales Guard-Flag in `OnClickSettings()` |
| 4 | Editor-Fehlalarm | Niedrig | Nur weiter beobachten, nicht ueberbewerten |

---

## Verifikation

1. **Config-Ladetest**
   - Admin-Menue oeffnen
   - Pruefen ob `AdminIDs`-Liste befuellt ist
   - Pruefen ob die Zone-Type-Liste korrekt mit `[x]` / `[ ]` erscheint

2. **Save-Roundtrip**
   - Werte im Allgemein-Tab aendern
   - Speichern
   - Menue schliessen und erneut oeffnen
   - Pruefen ob die Werte korrekt persistiert wurden

3. **Lockout-Test**
   - Versuchen, die eigene Admin-ID zu entfernen
   - Versuchen, eine leere `AdminIDs`-Liste zu speichern
   - Beides muss blockiert werden

4. **Grenzwerte-Test**
   - Negative oder extrem hohe Zahlen in die Felder eingeben
   - Pruefen ob die UI den Wert auf den erlaubten Bereich zuruecksetzt
