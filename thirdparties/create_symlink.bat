@set CUR_DIR=%~dp0
if not exist sqlite3secure mkdir sqlite3secure
if not exist sqlite3secure\src mklink /J sqlite3secure\src %FASTDO_PREFIX%\..\thirdparties\sqlite3secure\src

if not exist winux mkdir winux
if not exist winux\include mklink /J winux\include %FASTDO_PREFIX%\..\components\winux\include
if not exist winux\src mklink /J winux\src %FASTDO_PREFIX%\..\components\winux\src

if not exist eienexpr mkdir eienexpr
if not exist eienexpr\include mklink /J eienexpr\include %FASTDO_PREFIX%\..\components\eienexpr\include
if not exist eienexpr\src mklink /J eienexpr\src %FASTDO_PREFIX%\..\components\eienexpr\src

if not exist eiendb mkdir eiendb
if not exist eiendb\include mklink /J eiendb\include %FASTDO_PREFIX%\..\components\eiendb\include
if not exist eiendb\src mklink /J eiendb\src %FASTDO_PREFIX%\..\components\eiendb\src

if not exist winplus mkdir winplus
if not exist winplus\include mklink /J winplus\include %WINPLUS_ROOT%\include
if not exist winplus\src mklink /J winplus\src %WINPLUS_ROOT%\src
