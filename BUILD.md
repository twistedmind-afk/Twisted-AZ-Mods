# Build from source

Requires Docker, Git and Python 3.

Prepare the public AZ-MODS SDK build image once:

```powershell
git clone https://github.com/Kyle-Hosman/az-mods-sdk.git .azmod-sdk
git -C .azmod-sdk checkout 51512879d7ae3965d0ca900b447a2a2fb89e0a91
python .azmod-sdk/tools/azmod_pack.py image
```

From this repository, build all three mods:

```powershell
.\Build.ps1
```

Each binary is written to `mods/<mod-id>/out/<mod-id>.so`.

On Linux, run each mod's Makefile inside the SDK image, mounting that mod at `/mod` and this repository's `sdk/include` at `/sdk/include`, then use `make AZ_SDK_INCLUDE=/sdk/include`.

Each mod directory contains its complete implementation, Makefile, manifest, icon and license. `sdk/include/azmod-mod.h` is the public SDK interface required to compile the source; it is not installed on the USB drive.
