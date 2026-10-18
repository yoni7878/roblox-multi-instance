# roblox-multi-instance

A lightweight, bloat-free Windows command-line utility written in pure C that allows you to run multiple Roblox instances simultaneously. 

Unlike traditional multi-instance tools, this utility does **not** rely on risky memory injection or suspicious DLL hooking. Instead, it interacts directly with internal Windows NT APIs to locate the Roblox singleton mutex handle and safely closes it via `DuplicateHandle` with the `DUPLICATE_CLOSE_SOURCE` flag, freeing up the OS to launch another game client.

## 🚀 Features

*   **Pure C:** No heavy runtimes, no bloat, just minimal and fast native code.
*   **Safe Process Unlocking:** No memory modification, reducing the risk of anti-cheat flags or game instability.
*   **Command Line Native:** Easily scriptable and built to run quietly in your terminal.
*   **Resource Efficient:** Only executes what is necessary to clear the global mutex handle.

## 🛠️ How It Works

Roblox prevents multiple clients from running by creating a named **mutex** (mutual exclusion object) in Windows memory when it launches. If a second client starts, it checks for this mutex, finds it, and closes itself down.

This tool bypasses that restriction by:
1. Enumerating open system handles using internal NT APIs.
2. Filtering handles to identify the specific Roblox singleton mutex.
3. Using `DuplicateHandle` with the `DUPLICATE_CLOSE_SOURCE` option to safely close the handle from the system without injecting code into the Roblox process itself.

## 📦 Installation & Compilation

To compile the source code from scratch, you will need a standard Windows C compiler (like MSVC via Visual Studio Command Prompt or MinGW).

### Using MSVC (Developer Command Prompt):
```bash
cl.exe /O2 /Fe:roblox-multi-instance.exe main.c
```

## 💻 Usage

Run the compiled executable from an **Administrator** command prompt or PowerShell window (elevated privileges are required to duplicate and close handles belonging to other processes).

```bash
roblox-multi-instance.exe
```

### Verbose Debugging

If you want to audit what the tool is doing under the hood, you can pass the `--debug` flag. This option will cause the utility to print every system handle it encounters during its scan, along with its associated name, before targeting the specific Roblox mutex.

```bash
roblox-multi-instance.exe --debug
```

#### Example Output:
```text
[DEBUG] Scanning system handles...
[DEBUG] Handle [0x04]: \Device\NamedPipe\srvsvc
[DEBUG] Handle [0x08]: \BaseNamedObjects\Local\ZoneTypeCacheCounterMutex
[DEBUG] Handle [0x0C]: \BaseNamedObjects\RobloxSingletonMutex
[+] Target found! Closing handle 0x0C via DUPLICATE_CLOSE_SOURCE...
[+] successful. try opening another process.
```

Once executed, the tool will scan for the active Roblox mutex, clear it, and exit. You can then launch another Roblox instance immediately.

## ⚠️ Disclaimer

This project is for educational and utility purposes only. While it does not modify game memory or inject code, always use third-party tools at your own discretion. The developers are not responsible for any modifications or enforcement actions taken by the platform.
