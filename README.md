# roblox-multi-instance 

a lightweight, bloat-free windows command-line utility written in pure c that allows you to run multiple roblox instances simultaneously. unlike traditional multi-instance tools, this utility does **not** rely on risky memory injection or suspicious dll hooking. instead, it interacts directly with internal windows nt apis to locate the roblox singleton mutex handle and safely closes it via `duplicatehandle` with the `duplicate_close_source` flag, freeing up the os to launch another game client

## 🚀 features 

* **pure c:** no heavy runtimes, no bloat, just minimal and fast native code
* **safe process unlocking:** no memory modification, reducing the risk of anti-cheat flags or game instability
* **command line native:** easily scriptable and built to run quietly in your terminal
* **resource efficient:** only executes what is necessary to clear the global mutex handle

## 🛠️ how it works 

roblox prevents multiple clients from running by creating a named **mutex** (mutual exclusion object) in windows memory when it launches. if a second client starts, it checks for this mutex, finds it, and closes itself down. this tool bypasses that restriction by: 

1. enumerating open system handles using internal nt apis
2. filtering handles to identify the specific roblox singleton mutex
3. using `duplicatehandle` with the `duplicate_close_source` option to safely close the handle from the system without injecting code into the roblox process itself

## 📦 installation & compilation 

to compile the source code from scratch, you will need a standard windows c compiler (like msvc via visual studio command prompt or mingw)

### using msvc (developer command prompt): 

cl.exe /O2 /Fe:roblox-multi-instance.exe main.c

## 💻 usage 

run the compiled executable from an **user** command prompt or powershell window, the utility does not need **admin** permissions.

roblox-multi-instance.exe

### verbose debugging 

if you want to audit what the tool is doing under the hood, you can pass the `--debug` flag. this option will cause the utility to print every system handle it encounters during its scan, along with its associated name, before targeting the specific roblox mutex

roblox-multi-instance.exe --debug

#### example output: 

[+] handle name: \Device\NamedPipe\srvsvc
[+] handle name: \BaseNamedObjects\Local\ZoneTypeCacheCounterMutex
[+] handle name: \BaseNamedObjects\RobloxSingletonMutex
[+] handle name: ROBLOX_singletonEvent.
[+] successful. try opening another process.

once executed, the tool will scan for the active roblox mutex, clear it, and exit. you can then launch another roblox instance immediately

## ⚠️ disclaimer 

this project is for educational and utility purposes only. while it does not modify game memory or inject code, always use third-party tools at your own discretion. the developers are not responsible for any modifications or enforcement actions taken by the platform
