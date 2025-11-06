# 🌌 NebulaSim

**NebulaSim** is a simple, visually dynamic **space simulation project** written in C.  
It models the motion and brightness changes of space particles on a 2D grid — like a nebula slowly evolving in space.  
The project also includes a **basic authentication system** (login, register, forgot password) built using file handling and string operations.

---

## 🧠 Features

- 🔐 **Simple Authentication System**
  - Register new users
  - Login existing users
  - Reset password (Forgot Password)
  - Data stored in plain text (`users.db`)
- 🪐 **Nebula Simulation**
  - Particles move randomly on a grid
  - Collisions cause brightness increase
  - Each step saved as text snapshot
- 💾 **File Handling**
  - Saves particle states for replay
  - Stores user data persistently
- 🧩 **Core C Concepts Used**
  - Arrays & loops
  - Structures & functions
  - Pointers & dynamic memory (`malloc`)
  - File handling (`fopen`, `fprintf`, `fgets`)
  - Conditional logic (`if`, `switch`)

---

## 🏗️ Folder Structure

```
NebulaSim/
├── src/
│   ├── main.c        # Main simulation control + menu
│   ├── nebula.c      # Particle logic, display, movement
│   ├── nebula.h
│   ├── auth.c        # Login / Register / Forgot password
│   ├── auth.h
├── users.db          # User database (auto-created)
├── Makefile          # For easy build/run
└── README.md         # Project documentation
```

---

## ⚙️ Build Instructions

### 🖥️ On macOS / Linux:

```bash
make
./NebulaSim
```

### 🪟 On Windows (PowerShell or CMD):

```bash
gcc src\main.c src\nebula.c src\auth.c -o NebulaSim.exe
NebulaSim.exe
```

If you don’t have `make` on Windows, use the above GCC command instead.

---

## 📖 Usage

1. **Run the program**
2. Choose one of the following options:
   - `1` → Login
   - `2` → Register
   - `3` → Forgot password
   - `4` → Continue as guest
3. After authentication, the **Nebula Simulation** starts.
4. Watch particles move, brighten, and evolve over time.

---

## 💡 Example Output

```
Step 1:
. . * . O .
. * . . . .
. . O * . .
. . . . . .

Step 2:
. . . * O .
. * O . . .
. . . . . .
. * . . . .
```

---

## 👩‍💻 Team

| Member   | Role                         |
| -------- | ---------------------------- |
| Member 1 | Particle & Simulation Logic  |
| Member 2 | Display & File Handling      |
| Member 3 | Authentication & Menu System |

---

## 🧰 Technologies Used

- **Language:** C (C11 Standard)
- **Compiler:** GCC / Clang
- **Platform:** macOS, Windows, Linux
- **Tools:** Makefile, GitHub

---

## 🪙 Credits

Developed as part of the **CSE Project** at **KUET**.  
Guided by: _[Your Instructor’s Name]_  
Project by **Team Nebula** 🌌

> "Where code meets the cosmos."

---

## 🧾 License

This project is open-source for educational use only.
