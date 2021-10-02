# gta2-resurection

Only for education purposes

# Social

Youtube channel where I do streams: https://www.youtube.com/c/h0x91B

Discord: https://discord.gg/dUCcccp

# Prebuild version

Install fresh GTA 2 11.44 (I am recommend https://gtamp.com/gta2/)

Go to [Releases](https://github.com/h0x91b/gta2-resurection/releases)

Install it into the `GTA2` folder

Run the game using the desktop icon or by running `gta2-resurected.exe`

# Ghidra

Ghidra project file: [gta2-resurected.exe.gzf](./gta2-resurected.exe.gzf)

# Cheat engine

Cheat engine table that I use on streams: [gta2-resurected.ct](./gta2-resurected.ct)

# Building from source

```
git clone --recurse-submodules https://github.com/h0x91b/gta2-resurection.git
```

## Compile detours

Open in `Visual studio developer command prompt` directory `deps\detours\` and run `nmake`

## LuaJIT

Open in `Visual studio developer command prompt` directory `deps\LuaJIT\src\` and run `msvcbuild`

## Project itself

Open in Visual studio 2019

* Install GTA2 into `D:\games\gta2\`
* Or go to `View->Other windows->Property manager` open `PropertySheet`, go to `User macros` and change `GTA2Folder` to correct path.

Now you should be able to debug it by pressing F5.

# Building installer

Install `Inno setup 6.2`

* Build release
* Go to installer folder and open `installer.iss`
