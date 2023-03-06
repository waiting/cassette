#include "winplus_definitions.hpp"
#include "winplus_strings.hpp"
#include "strings.hpp"
#include <tchar.h>
#include <new>

namespace winplus
{

WINPLUS_FUNC_IMPL(String) Utf8ToString( AnsiString const & utf8 )
{
    UnicodeString unicode;
    INT cch = MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8.c_str(),
        (INT)utf8.length(),
        NULL,
        0
    );
    unicode.resize( cch + 1 );
    MultiByteToWideChar(
        CP_UTF8,
        0,
        utf8.c_str(),
        (INT)utf8.length(),
        &unicode[0],
        cch
    );
    return UnicodeToString( unicode.c_str() );
}

WINPLUS_FUNC_IMPL(AnsiString) StringToUtf8( String const & str )
{
    UnicodeString unicode = StringToUnicode(str);
    AnsiString utf8;
    INT length = WideCharToMultiByte(
        CP_UTF8,
        0,
        unicode.c_str(),
        (INT)unicode.length(),
        NULL,
        0,
        NULL,
        NULL
    );
    utf8.resize( length + 1 );
    WideCharToMultiByte(
        CP_UTF8,
        0,
        unicode.c_str(),
        (INT)unicode.length(),
        &utf8[0],
        length,
        NULL,
        NULL
    );
    return utf8.c_str();
}


StringArray StrSplitComma( String const & str )
{
    StringArray arr;
    StrSplit( str, TEXT(","), &arr );
    return arr;
}

StringArray StrSplitSemicolon(String const & str)
{
    StringArray arr;
    StrSplit( str, TEXT(";"), &arr );
    return arr;
}

WINPLUS_FUNC_IMPL(UnicodeString) LoadStringExW( HMODULE module, UINT id )
{
    UnicodeString result;
    WORD blockId = ( id >> 4 ) + 1;
    WORD strResIndex = id % 16;
    HRSRC resource = FindResource( module, MAKEINTRESOURCE(blockId), RT_STRING );
    if ( resource != NULL )
    {
        DWORD size = SizeofResource( module, resource );
        HGLOBAL loadedRes = LoadResource( module, resource );
        LPBYTE data = (LPBYTE)LockResource(loadedRes);
        DWORD offset = 0;
        INT index = 0;
        while ( offset + sizeof(WORD) <= size )
        {
            WORD w = *(WORD *)( data + offset );
            offset += sizeof(WORD);
            if ( w > 0 )
            {
                if ( index == strResIndex )
                {
                    UnicodeString str;
                    str.resize(w+1);
                    CopyMemory( &str[0], data + offset, w * sizeof(WCHAR) );
                    result = str.c_str();
                    break;
                }
                offset += w * sizeof(WCHAR);
            }
            index++;
        }
        FreeResource(loadedRes);
    }
    return result.c_str();
}

WINPLUS_FUNC_IMPL(String) LoadStringEx( HMODULE module, UINT id )
{
    return UnicodeToString( LoadStringExW( module, id ) );
}

WINPLUS_FUNC_IMPL(String) LoadString( UINT id )
{
    return LoadStringEx( GetModuleHandle(NULL), id );
}


} // namespace winplus
