### Running GardenGate on Linux

### Keep in mind that this is currently experimental.

# Garden warfare 1
### 1. Install the game ###
* Install the game from ea app, launch it once

### 2. Download and place files to your game's root directory ###
Download the archive from [the releases](https://github.com/nocss42/GardenGate/releases) tab, launch it > go to patcher and press apply patch

### 3. Install the mods ###
Use [this gw1 compatible frosty mod manager](https://github.com/Twig6943/FrostyToolsuiteGW1Linux/releases) instead of the stock one as the stock one doesn't work with wine nor gw1. Get the mods from [here](../Mods/GW1)

### 4. Configure dlloverrides & data dir env var ###
Use `GAME_DATA_DIR` environment variable as ea app won't allow special characters like `/` as launch args

```
GAME_DATA_DIR=path/to/ModData/Default WINEDLLOVERRIDES="dinput8=n,b;winmm=n,b" %command%
```

Open up winetricks > select gw1's pfx > select winecfg > go to libraries > add the following

```
dinput8
winmm
```

* Apply 
* See arguements at the end of this file for joining

# Garden warfare 2/Battle for Neighborville
### 1. Install the game ###
* Install the game from your platform, launch it once
* If prompted about the anti-cheat, select `Yes`
* You should receive an error message like `wine not supported` - click OK

**<img src="./Assets/wine_not_supported.webp" />**

### 2. Disable EA Anti-Cheat ###
#### Navigate to the game's installation directory
#### Rename the following files:
`EAAntiCheat.GameServiceLauncher.exe` to `EAAntiCheat.GameServiceLauncher.exe.bak`

`installScript.vdf` to `installScript.vdf.bak`

### 3. Downgrade your game ###
Download the archive from [the releases](https://github.com/nocss42/GardenGate/releases) tab, launch it > go to patcher and press apply patch

### 4. Install the mods in the right order ###
Use [this frosty mod manager](https://github.com/maniman303/FrostyToolsuite/releases) instead of the stock one as the stock one doesn't work with wine. Get the mods for gw2 [here](../Mods/GW2/GardenGate.fbpack)

**<img src="./Assets/load_order.webp" />**

### 6. Configure dlloverrides ###
Add the following launch option in Steam:
```
WINEDLLOVERRIDES="dinput8=n,b;winmm=n,b" %command% -dataPath ModData/Default
```

Open up protontricks > select gw2 > select winecfg > go to libraries > add the following

```
dinput8
winmm
```

(They should be set to native,builtin)

### 7. Start Garden warfare 2 ###
- Launch gw2/bfn via the launcher if you own it on EA. If you own the game on steam just pass in the launch args yourself;

### Arguements ###
`-name`

`-dataPath`

`-Client.ServerIp`

`-Server.ServerPassword`
