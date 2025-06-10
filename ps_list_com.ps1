$ports = Get-WmiObject Win32_PnPEntity | Where-Object {
    $_.Name -match 'COM\d+|LPT\d+'
}

foreach ($port in $ports) {
    $port.Name
}
