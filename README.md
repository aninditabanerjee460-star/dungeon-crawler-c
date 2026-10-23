# C-Dungeon Crawler

A modular, terminal-based dungeon crawler game written from scratch in pure C. Explore the map, hunt down hidden loot, fight monsters, and defeat the boss to conquer the dungeon.

## 🎮 Game Overview
You wake up at the entrance of a forgotten network of caverns. Your primary mission is to find the **Dungeon Master Key** hidden inside the Treasury Vault, open the gates to the Dragon's Lair, and defeat the **Ironscale Drake**.

### Key Features
* **Modular Room Data Structures:** Built using custom struct objects tracking dynamic runtime state data like enemy status, environmental features, and player metrics.
* **Safe Input Sanitization:** Utilizes clean `fgets` buffer handling routines that robustly clear excess console signals, preventing common execution loops and terminal crashes.
* **Turn-Based Combat System:** Fight dynamic encounters utilizing strategic turn commands for striking enemies, executing safe retreat protocols, or consuming recovery elixirs.
* **In-Game Navigational Systems:** Includes contextual maps and structural location descriptors linked via a clean static room grid.

---

## 🗺️ Dungeon Map Layout

```text
                 [Dragon's Lair] (Key Required)
                        │
 [Treasury Vault] ─── [Forgotten Armory] ─── [Cobwebbed Entrance]
```

---

## 🛠️ How to Compile and Run

This game uses standard C libraries (`stdio.h`, `string.h`, `ctype.h`, `limits.h`) and compiles flawlessly on any standard compiler like **GCC** or **Clang**.

### 1. Compile the Game
Open your computer terminal or command prompt inside the game directory and execute the compiler command:
```bash
gcc main.c -o dungeon_crawler
```

### 2. Launch the Application
Run the generated executable output file:
* **Linux / macOS:**
  ```bash
  ./dungeon_crawler
  ```
* **Windows:**
  ```cmd
  dungeon_crawler.exe
  ```

---

## ⌨️ Command References

### Exploration Mode
* `go north` / `north` — Navigate to an adjacent room.
* `search` — Look through rooms for hidden chests and elite equipment.
* `stats` — View your character's current health, attack values, and items.
* `look` — Redisplay descriptions of your immediate surroundings.
* `map` — Print the layout map of the subterranean sector.
* `exit` — Flee the dungeon and close the application safely.

### Active Battle Mode
* `attack` — Strike your current enemy target.
* `potion` — Drink a recovery draft to restore 40 Health Points.
* `flee` — Safely escape combat and retreat back into your last visited room.

---

## 📄 License
This project is open-source software distributed under the terms of the [MIT License](LICENSE).

