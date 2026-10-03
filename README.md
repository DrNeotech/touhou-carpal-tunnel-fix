# touhou-carpal-tunnel-fix
A mod to fix some of the really annoying keybinds in certain Touhou games.

This was made with Touhou 12.8 in mind but I accidentally made it work pretty much flawlessly on Touhou 9 too, although I didn't test this much since this whole thing was kind of a rush job. If there's any bugs with it just make a new issue (or if you don't have a Github account just message me on Twitter).

It might mess with menu navigation a little. If that gets too unbearable pressing the Enter key instead of Z for menus will work fine. I'll probably fix it later.

# Installing
If you're on Windows just drag and drop dinput8.dll into the same folder as the exe. 

On Linux do the same then add `WINEDLLOVERRIDES="dinput8=n,b" %command%` to your launch options in Steam. If you're not using Steam to launch it then it's just the same as any environment variable really, just drop the `%command%`.

# Usage
All the controls pretty much work like in Touhou 19.
- Hold Z to fire
- Hold X to charge your Spell Attack/Ice Barrier
- Tap C to use Spell Attack/Perfect Freeze

# Resources
The whole code to fix this was like 20 lines max, everything else is just a slightly modified version of this proxy DLL. 
https://code.google.com/archive/p/dinput-proxy-dll/