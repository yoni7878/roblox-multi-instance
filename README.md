# roblox-multi-instance
a lightweight windows cli utility written in pure c that lets you run multiple roblox instances. instead of using memory injection or dll hooks, it uses internal nt apis to find the singleton mutex handle and safely closes it using duplicate_close_source so you can launch another client. no bloat, just process unlocking.
