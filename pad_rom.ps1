param($inputFile, $outputFile)

$content = Get-Content $inputFile -Encoding Byte
$targetSize = 32768
$currentSize = $content.Length

if ($currentSize -lt $targetSize) {
    $paddingSize = $targetSize - $currentSize
    $padding = New-Object byte[] $paddingSize
    $allContent = $content + $padding
    [System.IO.File]::WriteAllBytes($outputFile, $allContent)
    Write-Host "Padded ROM from $currentSize bytes to $targetSize bytes"
} else {
    Copy-Item $inputFile $outputFile
    Write-Host "ROM already $currentSize bytes, no padding needed"
}
