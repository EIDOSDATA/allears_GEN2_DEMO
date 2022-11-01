@echo off
setlocal EnableDelayedExpansion

set "STfilepath=C:\Program Files (x86)\STMicroelectronics\STM32 ST-Link Utility\ST-Link Utility\ST-LINK_CLI.exe"
set "output_cnt=0"
set "hexfile=NONE"
set q=0
set serial[q]=0

for /F "delims=" %%f in ('"!STfilepath!" -List') do (
    set /a output_cnt+=1
    set "output[!output_cnt!]=%%f"
)
for /L %%a in (1 1 !output_cnt!) do call echo !output[%%a]!
pause > nul 

::iterate thru output array to find SN and parse it to get the serial number
for /L %%n in (1 1 !output_cnt!) do (
    for /F "tokens=1,* delims=: " %%1 in ("!output[%%n]!") do (
        if "%%1" == "SN" (            
            set serial[q]=%%2                         
            echo !serial[q]!
            echo !serial[q]! >> a.txt
            set /a q+=1
        )
    )
)


set q-=2

echo serial number: !serial[q]!
for /r %%i in (*.hex) do (set "hexfile=%%i")
echo HexFile: !hexfile!

set "CMD="!STfilepath!" -c SN=!serial[q]! SWD UR FREQ=400 -P "!hexfile!" -V -HardRst"
echo %CMD%
%CMD%
pause