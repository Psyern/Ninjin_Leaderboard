# Class Renaming Analysis: Adding `TrackingMod_` Prefix

## Overview
This document outlines the analysis for renaming classes to use the `TrackingMod_` prefix to avoid mod conflicts.

## Estimated Changes: **~200-250 occurrences** across both mods

---

## Classes to Rename (12 classes)

### Main Mod (`Ninjins_Tracking_Mod`):

1. **`PlayerDeathData`** → `TrackingMod_PlayerDeathData`
   - **~52 occurrences** in `Ninjins_Tracking_Mod_Data.c`
   - Used in MissionServer, PlayerBase, etc.
   - **High impact** - Core data class

2. **`CategoryMatch`** → `TrackingMod_CategoryMatch`
   - **~28 occurrences** in `PVECategoryConfig.c`
   - Used for category matching logic
   - **Medium impact**

3. **`PVECategory`** → `TrackingMod_PVECategory`
   - **~28 occurrences** in `PVECategoryConfig.c`
   - Core PVE category class
   - **Medium impact**

4. **`PVECategoryConfigJson`** → `TrackingMod_PVECategoryConfigJson`
   - **~10 occurrences** in `PVECategoryConfig.c`
   - JSON structure class
   - **Low impact**

5. **`PVECategoryConfig`** → `TrackingMod_PVECategoryConfig`
   - **~30+ occurrences** across multiple files
   - Main PVP config class
   - **High impact** - Used throughout codebase

6. **`PVPCategory`** → `TrackingMod_PVPCategory`
   - **~13 occurrences** in `PVPCategoryConfig.c`
   - Core PVP category class
   - **Medium impact**

7. **`PVPCategoryConfigJson`** → `TrackingMod_PVPCategoryConfigJson`
   - **~8 occurrences** in `PVPCategoryConfig.c`
   - JSON structure class
   - **Low impact**

8. **`PVPCategoryConfig`** → `TrackingMod_PVPCategoryConfig`
   - **~25+ occurrences** across multiple files
   - Main PVP config class
   - **High impact** - Used throughout codebase

9. **`RewardConfigEntry`** → `TrackingMod_RewardConfigEntry`
   - **~32 occurrences** in `TrackingModRewardConfig.c`
   - Reward configuration entry
   - **Medium impact**

10. **`ZoneKillExclusion`** → `TrackingMod_ZoneKillExclusion`
    - **~5 occurrences** in `TrackingModConfig.c`
    - Zone exclusion config
    - **Low impact**

11. **`DeathCategory`** → `TrackingMod_DeathCategory`
    - **~5 occurrences** in `DeathCategoryConfig.c`
    - Death category class
    - **Low impact**

12. **`DeathCategoryConfig`** → `TrackingMod_DeathCategoryConfig`
    - **~8 occurrences** in `DeathCategoryConfig.c`
    - Death category config
    - **Low impact**

---

## Already Safe (No Changes Needed)

These classes already have `TrackingMod_` prefix:
- `TrackingModData`
- `TrackingModRewardConfig`
- `TrackingModConfig`
- `TrackingModMilestoneHelper`
- `TrackingModMilestoneRewardLoader`
- `TrackingModRewardHelper`
- `TrackingModLeaderboardData`
- `TrackingModLogger`
- `TrackingModZoneUtils`
- All UI classes (already prefixed)

**Modded classes** (extending DayZ classes - these are safe):
- `AnimalBase` (modded)
- `ZombieBase` (modded)
- `eAIBase` (modded)
- `PlayerBase` (modded)
- `MissionServer` (modded)
- `DayZPlayerImplementThrowing` (modded)
- `ExplosivesBase` (modded)
- `TrapBase` (modded)
- `Grenade_Base` (modded)
- `MissionBase` (modded in UI)

---

## Impact Assessment

### Complexity: **Medium-High**

**Why it's complex:**
1. **12 classes** need renaming
2. **~200-250 total occurrences** across codebase
3. **Cross-file dependencies** - classes used in multiple files
4. **JSON serialization** - class names may be in JSON files
5. **RPC parameters** - if classes are passed via RPC
6. **Static references** - e.g., `PVECategoryConfig.GetInstance()`

### Files Most Affected:

1. **`Ninjins_Tracking_Mod_Data.c`** - ~52 changes for `PlayerDeathData`
2. **`PVECategoryConfig.c`** - ~66 changes total
3. **`PVPCategoryConfig.c`** - ~46 changes total
4. **`TrackingModRewardConfig.c`** - ~32 changes for `RewardConfigEntry`
5. **`MissionServer.c`** - ~29 changes
6. **`PlayerBase.c`** - ~3 changes

---

## Renaming Strategy

### Step-by-Step Approach:

1. **Start with low-impact classes** (DeathCategory, ZoneKillExclusion)
2. **Move to medium-impact** (CategoryMatch, RewardConfigEntry)
3. **Handle high-impact last** (PlayerDeathData, PVECategoryConfig, PVPCategoryConfig)

### For Each Class:

1. Find all occurrences: `grep -r "ClassName" src/`
2. Replace in order:
   - Class declaration: `class ClassName` → `class TrackingMod_ClassName`
   - Type references: `ClassName variable` → `TrackingMod_ClassName variable`
   - Static calls: `ClassName.Method()` → `TrackingMod_ClassName.Method()`
   - JSON class names (if applicable)
3. Test compilation after each class
4. Verify functionality

### Important Notes:

- **JSON files**: Check if class names appear in JSON (they shouldn't, but verify)
- **RPC calls**: Verify RPC parameters still work
- **Static methods**: Update all `ClassName.GetInstance()` calls
- **Type casts**: Update all `ClassName.Cast()` calls
- **Array/Map types**: Update `array<ref ClassName>` → `array<ref TrackingMod_ClassName>`

---

## Recommendation

**Effort Level**: Medium (2-3 hours of careful find/replace)
**Risk Level**: Medium (cross-file dependencies, JSON serialization)
**Benefit**: High (prevents conflicts with other mods)

**Should proceed?** Yes, but do it systematically, file by file, to ensure consistency.

---

## Checklist

- [ ] Create backup of codebase
- [ ] Rename low-impact classes first
- [ ] Test compilation after each class
- [ ] Rename medium-impact classes
- [ ] Test compilation
- [ ] Rename high-impact classes (PlayerDeathData, PVECategoryConfig, PVPCategoryConfig)
- [ ] Full compilation test
- [ ] Test in-game functionality
- [ ] Verify JSON files still work
- [ ] Verify RPC calls still work
- [ ] Update documentation if needed

---

**Last Updated**: 2026-01-22
**Status**: Analysis Complete - Ready for Implementation
