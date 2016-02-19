@echo off

set file=boost_1_57_0.7z
set boost_src=boost_1_57_0
set success_flag=%cd%\%boost_src%\boost\success_flag
set exec_7za=

if exist "%Windir%\system32\7za.exe" goto _7ZA  
if exist "%cd%\..\..\tools\7zip\bin.win32\7za.exe.win32" goto _THIS7ZA
goto WARNING

:_7ZA
set exec_7za=%Windir%\system32\7za.exe
goto unpack

:_THIS7ZA
copy "%cd%\..\..\tools\7zip\bin.win32\7za.exe.win32" "%cd%\..\..\tools\7zip\bin.win32\7za.exe"
set exec_7za="%cd%\..\..\tools\7zip\bin.win32\7za.exe"
goto unpack

:WARNING
echo warning: please extract %cd%\%file% to %cd%\ by yourself.
goto END

:unpack
if not exist %success_flag% (
	%exec_7za% x -y %file%
	del "%cd%\..\..\tools\7zip\bin.win32\7za.exe"
REM ����������û��Ǳ�Ҫ��, ����û�� 7za ִ����Ϳ�ʼִ�е�
REM	echo=>%success_flag% 
)
goto END


:END
REM pause
