![Image](https://github.com/user-attachments/assets/ce2b08ae-ae83-406d-b49f-b7a9e435daa9)

## About

This program makes sense in two ways:

1. It prevents the virtual desktop from closing, unless you explicitly click Start->Exit desktop
2. It displays the time (and also the date if you hover it)

There are two ways to enable the taskbar in Wine (without which this program makes no sense):

1. wine explorer /desktop=shell,1920x1080 # not recommended
2. setting "shell" as default in regedit (explained as follows)

Go to `HKEY_CURRENT_USER\Software\Wine\Explorer` and set "Desktop" to "shell".
In `HKEY_CURRENT_USER\Software\Wine\Explorer\Desktops` create a new String "shell" and set it to "1920x1080".

Now start wine like so:

```wine "C:/trayclock.exe"```

As noted, the virtual desktop will not close no matter what you do, unless you go to Start->Exit desktop. 

Unfortunately it can happen that winecfg resets "Desktop" to "Default", if you select "emulate virtual desktop" there. This will result in the sudden unexpected absence of the taskbar, until you set it to "shell" again in regedit.

## Compiling

```
x86_64-w64-mingw32-g++ -mwindows -static-libgcc -static-libstdc++ trayclock.cpp -o trayclock.exe 
i686-w64-mingw32-g++ -mwindows -static-libgcc -static-libstdc++ trayclock.cpp -o trayclock32.exe
```

## Maximum Rice

### Firejail, alternative wine version, DXVK, NVAPI, VKD3D & modeset for gaming

```
64BITWINE=; WINEEDIR=/opt/wine64; 0nvidia-reset off; WINERT=/opt/wine-10.11-fixed/ ;firejail --ignore=noinput  --private-etc=@x11,@games,@tls-ca,fonts --private=${WINEEDIR}/fakehome/ --private-dev --whitelist=/mnt/usb1 --whitelist=/mnt/2/GAMES --whitelist=/opt/STORE --whitelist=${WINERT} --whitelist=${WINEEDIR} --env=WINEARCH=win64 --env=WINEPREFIX=${WINEEDIR} --env=WINE=${WINERT}/bin/wine --env=XCURSOR_SIZE=16 --env=WINESERVER=${WINERT}/bin/wineserver --blacklist=/usr/local --blacklist=/bin --blacklist=/sbin --blacklist=/secret.tar.gz --blacklist=/srv --blacklist=/var/lib --blacklist=/var/www --blacklist=/root --blacklist=/boot --blacklist=/home/secret/ --env=VK_ICD_FILENAMES=/usr/share/vulkan/icd.d/nvidia_icd.json --env=VKD3D_FEATURE_LEVEL=12_2 --env=VKD3D_CONFIG=dxr12,dxr --env=WINE_ENABLE_NGX_UPDATER=1 --env=WINEESYNC=1 --env=VKD3D_DEBUG="none" --env=DXVK_ASYNC=1 --env=WINEDLLOVERRIDES="d3dcompiler_47=n;dxgi=n,b;d3d11=n,b" --env=WINE_ENABLE_NVAPI=1 --env=WINE_HIDE_NVIDIA_GPU=0 --env=WEBVIEW2_ADDITIONAL_BROWSER_ARGUMENTS="--no-sandbox" --env=XCURSOR_THEME=core --env=__GL_SYNC_TO_VBLANK=0 --env=__GL_MaxFramesAllowed=1 --env=__GL_GSYNC_ALLOWED=0 --env=__GL_VRR_ALLOWED=0 --env=VDPAU_NVIDIA_SYNC_DISPLAY_DEVICE=DP-4 --env=HOME=/opt/wine64/fakehome/ --env=DXVK_HUD=fps,cpu --env=__GL_THREADED_OPTIMIZATIONS=1 --env=WINEFSYNC=1 --net=none ${WINERT}/bin/wine "C:/trayclock.exe"; 0nvidia-reset
```

### Wallpaper

```
wine reg add "HKCU\Control Panel\Desktop" /v Wallpaper /t REG_SZ /d "C:\\path\\to\\image.bmp" /f
wine reg add "HKCU\Control Panel\Desktop" /v WallpaperStyle /t REG_SZ /d 2 /f
wine reg add "HKCU\Control Panel\Desktop" /v TileWallpaper /t REG_SZ /d 0 /f
```

