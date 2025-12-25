# copy-winmd.ps1
# Copies .winmd files from the latest installed Windows App SDK runtime
# into a local "winrt/winmd" folder in the project

param(
    [string]$Destination = "$(Get-Location)\..\winrt\winmd"
)

Write-Host "Searching for Windows App SDK runtimes..." -ForegroundColor Cyan

# Path where WindowsAppRuntime packages are installed
$appsPath = "C:\Program Files\WindowsApps"

# Get all runtime folders
$runtimes = Get-ChildItem $appsPath -Directory -ErrorAction SilentlyContinue |
        Where-Object { $_.Name -like "Microsoft.WindowsAppRuntime.*_x64__8wekyb3d8bbwe" -or
                $_.Name -like "Microsoft.WindowsAppRuntime.*_x86__8wekyb3d8bbwe" -or
                $_.Name -like "Microsoft.WindowsAppRuntime.*_arm64__8wekyb3d8bbwe" }

if (-not $runtimes) {
    Write-Error "No Windows App SDK runtimes found. Is the Windows App SDK installed?"
    exit 1
}

# Sort by package version first, then by build number
# Format: Microsoft.WindowsAppRuntime.X.Y[-experimental]_buildnumber_arch__hash
$latest = $runtimes | Sort-Object {
    if ($_.Name -match "WindowsAppRuntime\.([0-9]+\.[0-9]+)") {
        $packageVersion = [version]"$($matches[1]).0.0"
        # Extract build number
        if ($_.Name -match "_([0-9]+\.[0-9]+\.[0-9]+\.[0-9]+)_") {
            $buildVersion = [version]$matches[1]
        } else {
            $buildVersion = [version]"0.0.0.0"
        }
        # Return composite for sorting: package version as major sort key
        # Multiply package version parts to make them dominant in comparison
        return ($packageVersion.Major * 10000000000 + $packageVersion.Minor * 100000000 + $buildVersion.Major * 1000000 + $buildVersion.Minor * 10000 + $buildVersion.Build * 100 + $buildVersion.Revision)
    } else {
        return 0
    }
} -Descending | Select-Object -First 1

$runtimePath = $latest.FullName
Write-Host "Found latest runtime: $runtimePath" -ForegroundColor Green

# Find .winmd files inside the runtime folder
$winmdFiles = Get-ChildItem -Path $runtimePath -Recurse -Include *.winmd -ErrorAction SilentlyContinue

if (-not $winmdFiles) {
    Write-Error "No .winmd files found in the runtime."
    exit 1
}

# Create destination folder
if (-not (Test-Path $Destination)) {
    New-Item -ItemType Directory -Path $Destination | Out-Null
}

# Copy files
foreach ($file in $winmdFiles) {
    Write-Host "Copying $($file.Name)..."
    Copy-Item $file.FullName -Destination (Join-Path $Destination $file.Name) -Force
}

Write-Host "Copied $($winmdFiles.Count) .winmd files to $Destination" -ForegroundColor Yellow