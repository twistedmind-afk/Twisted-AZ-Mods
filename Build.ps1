$ErrorActionPreference = 'Stop'
Push-Location -LiteralPath $PSScriptRoot
try {
    foreach ($modId in @('change-deck-on-load','led-blink-on-air','4-deck-pro-dj-link-plus')) {
        $modDirectory = Join-Path $PSScriptRoot "mods\$modId"
        $sdkInclude = Join-Path $PSScriptRoot 'sdk\include'
        & docker run --rm --pull never --network none --platform linux/arm64 --mount "type=bind,source=$modDirectory,target=/mod" --mount "type=bind,source=$sdkInclude,target=/sdk/include,readonly" -w /mod az-mods-sdk:1 make AZ_SDK_INCLUDE=/sdk/include
        if ($LASTEXITCODE -ne 0) { throw "Build failed: $modId" }
    }
} finally { Pop-Location }
