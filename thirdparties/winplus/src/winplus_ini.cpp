#include "winplus_definitions.hpp"
#include "winplus_ini.hpp"
#include "strings.hpp"
#include <tchar.h>
#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

namespace winplus
{

Ini::Ini( String const & szIniFile, BOOL bInWindowsDir ) : _strIniFile(szIniFile)
{
    size_t len = _strIniFile.length();
    /* 不是全路径,eg C:\ */
    if ( !( len > 1 && _strIniFile[1] == _T(':') ) )
    {
        // 不想在windows目录
        if ( !bInWindowsDir )
        {
            if ( len > 1 )
            {
                if ( !( _strIniFile[0] == _T('.') && _strIniFile[1] == _T('\\') ) )
                {
                    _strIniFile = _T(".\\") + _strIniFile;
                }
            }
            else
                _strIniFile = _T(".\\") + _strIniFile;
        }
    }
}

BOOL Ini::setValue(LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szValue)
{
    return WritePrivateProfileString( szSection, szKeyName, szValue, _strIniFile.c_str() );
}

BOOL Ini::setValue( LPCTSTR szSection, LPCTSTR szKeyName, INT nValue, BOOL bSigned /*= TRUE*/ )
{
    return setValue( szSection, szKeyName, Format( bSigned ? _T("%d") : _T("%u"), nValue ).c_str() );
}

BOOL Ini::setValue( LPCTSTR szSection, LPCTSTR szKeyName, DOUBLE dblValue )
{
    return setValue( szSection, szKeyName, Format( _T("%lf"), dblValue ).c_str() );
}

BOOL Ini::setData( LPCTSTR szSection, LPCTSTR szKeyName, LPCVOID lpDataBuf, UINT uSizeData )
{
    return WritePrivateProfileStruct( szSection, szKeyName, (LPVOID)lpDataBuf, uSizeData, _strIniFile.c_str() );
}

INT Ini::getValue( LPCTSTR szSection, LPCTSTR szKeyName, INT nDefault )
{
    return (INT)GetPrivateProfileInt( szSection, szKeyName, nDefault, _strIniFile.c_str() );
}

String Ini::getValue( LPCTSTR szSection, LPCTSTR szKeyName, LPCTSTR szDefault )
{
    DWORD nBufferSize = 256;
    DWORD dwCopied = 0, dwTmp = 0;
    String szBuffer;
    do 
    {
        nBufferSize <<= 1;
        szBuffer.resize(nBufferSize);
        dwCopied = GetPrivateProfileString( szSection, szKeyName, szDefault, &szBuffer[0], nBufferSize, _strIniFile.c_str() );
        if ( szSection && szKeyName )
            dwTmp = 1;
        else
            dwTmp = 2;
    }
    while ( nBufferSize - dwTmp == dwCopied );
    return String( szBuffer.c_str(), dwCopied );
}

DOUBLE Ini::getValue( LPCTSTR szSection, LPCTSTR szKeyName, DOUBLE dblDefault )
{
    DOUBLE dblValue;
    String szDefault = Format( _T("%lf"), dblDefault );
    String szValue = getValue( szSection, szKeyName, szDefault.c_str() );
    LPTSTR pszEnd;
    dblValue = _tcstod( szValue.c_str(), &pszEnd );
    return dblValue;
}

BOOL Ini::getData( LPCTSTR szSection, LPCTSTR szKeyName, LPVOID lpDataBuf, UINT uSizeData )
{
    return GetPrivateProfileStruct( szSection, szKeyName, lpDataBuf, uSizeData, _strIniFile.c_str() );
}

//////////////////////////////////////////////////////////////////////////

Ini::Section & Ini::Section::set( String const & key, String const & value )
{
    this->_i->setValue( this->_sectionName, key.c_str(), value.c_str() );
    return *this;
}

Ini::Section & Ini::Section::set( String const & key, int value )
{
    this->_i->setValue( this->_sectionName, key.c_str(), value, TRUE );
    return *this;
}

Ini::Section & Ini::Section::set( String const & key, uint value )
{
    this->_i->setValue( this->_sectionName, key.c_str(), (int)value, FALSE );
    return *this;
}

Ini::Section & Ini::Section::set( String const & key, double value )
{
    this->_i->setValue( this->_sectionName, key.c_str(), value );
    return *this;
}

Ini::Section & Ini::Section::set( String const & key, LPCVOID value, uint size )
{
    this->_i->setData( this->_sectionName, key.c_str(), value, size );
    return *this;
}

String Ini::Section::get( String const & key, String const & defVal ) const
{
    return this->_i->getValue( this->_sectionName, key.c_str(), defVal.c_str() );
}

int Ini::Section::get( String const & key, int defVal ) const
{
    return this->_i->getValue( this->_sectionName, key.c_str(), defVal );
}

uint Ini::Section::get( String const & key, uint defVal ) const
{
    return (uint)this->_i->getValue( this->_sectionName, key.c_str(), (int)defVal );
}

double Ini::Section::get( String const & key, double defVal ) const
{
    return this->_i->getValue( this->_sectionName, key.c_str(), defVal );   
}

bool Ini::Section::get( String const & key, LPVOID data, uint size ) const
{
    return FALSE != this->_i->getData( this->_sectionName, key.c_str(), data, size );
}

} // namespace winplus
