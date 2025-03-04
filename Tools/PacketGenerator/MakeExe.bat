pushd %~dp0

pyinstaller --onefile PacketGenerator.py
MOVE .\dist\PacketGenerator.exe .\PacketGenerator.exe
@RD /S /Q .\build
@RD /S /Q .\dist
DEL /S /F /Q .\PacketGenerator.spec
PAUSE