//
// winux的归档文件
//
#include "utilities.hpp"
#include "smartptr.hpp"
#include "strings.hpp"
#include "filesys.hpp"
#include "archives.hpp"

#include "eienexpr.hpp"
#include "eienexpr_errstrs.inl"

#if defined(OS_WIN) // IS_WINDOWS
    #include <mbstring.h>
    #include <tchar.h>
#else
    #include <wchar.h>
    #ifdef UNICODE
    #define _vsntprintf vswprintf
    #define _tcsstr wcsstr
    #define _tcstol wcstol
    #define _tcstoul wcstoul
    #else
    #define _vsntprintf vsnprintf
    #define _tcsstr strstr
    #define _tcstol strtol
    #define _tcstoul strtoul
    #endif
#endif

namespace winux
{
using namespace eienexpr;

#include "is_x_funcs.inl"

// 判断字符串是否为数字串
inline static bool IsNumberString( winux::String const & str )
{
    if ( str.empty() ) return false;

    bool hasPoint = false;
    int n = 0;
    for ( size_t i = 0; i < str.length(); ++i )
    {
        winux::String::value_type ch = str[i];
        if ( IsDigit(ch) )
        {
            n++;
        }
        else if ( ch == '-' || ch == '+' )
        {
            // 符号不出现在开始或者e后面，则不是数字
            if ( !( i == 0 || ( i > 0 && ( str[i - 1] | 0x20 ) == 'e' ) ) )
            {
                return false;
            }
        }
        else if ( ( ch | 0x20 ) == 'e' )
        {
            if ( n == 0 || i == str.length() - 1 ) return false;
        }
        else if ( ch == '.' )
        {
            if ( hasPoint )
            {
                return false;
            }

            hasPoint = true;
        }
        else
        {
            return false;
        }
    }

    return n < 11; // 数字小于11位才算数字串
}

inline static tchar NumberStringToChar( const tchar * number, int base )
{
    tchar * endptr;
    return (tchar)_tcstol( number, &endptr, base );
}

inline static winux::ulong NumberStringToNumber( const tchar * number, int base )
{
    tchar * endptr;
    return (winux::ulong)_tcstoul( number, &endptr, base );
}

// class Configure -----------------------------------------------------------------------------
String const Configure::ConfigVarsSlashChars = $T("\n\r\t\v\a\\\'\"$()");

Configure::Configure()
{

}

Configure::Configure( String const & configFile )
{
    this->load(configFile);
}

int Configure::_FindConfigRef( String const & str, int offset, int * length, String * name )
{
    String ldelim = $T("$(");
    String rdelim = $T(")");
    *length = 0;
    int pos1 = (int)str.find( ldelim, offset );
    if ( pos1 == -1 ) return -1;
    int pos2 = (int)str.find( rdelim, pos1 + ldelim.length() );
    if ( pos2 == -1 ) return -1;
    *length = pos2 + (int)rdelim.length() - pos1;
    *name = str.substr( pos1 + ldelim.length(), pos2 - pos1 - ldelim.length() );
    return pos1;
}

String Configure::_expandVarNoStripSlashes( String const & name, StringArray * chains ) const
{
    if ( !this->has(name) ) return $T("");
    chains->push_back(name);
    String res = _expandValNoStripSlashes( _rawParams.at(name), chains );
    chains->pop_back();
    return res;
}

String Configure::_expandValNoStripSlashes( String const & val, StringArray * chains ) const
{
    if ( val.empty() ) return $T("");
    String res;
    int len;
    String varName;
    int offset = 0;
    int pos;
    while ( ( pos = _FindConfigRef( val, offset, &len, &varName ) ) != -1 )
    {
        res += val.substr( offset, pos - offset );
        offset = pos + len;
        res += !ValueIsInArray( *chains, varName ) ? _expandVarNoStripSlashes( varName, chains ) : $T("");
    }
    res += val.substr(offset);
    return res;
}

inline static bool __GetLine( IFile * f, String * line )
{
    String tmpLine = f->getLine();
    *line = tmpLine.empty() ? $T("") : ( tmpLine[tmpLine.length() - 1] == '\n' ? tmpLine.substr( 0, tmpLine.length() - 1 ) : tmpLine );
    return !tmpLine.empty();
}

int Configure::_load( String const & configFile, StringStringMap * rawParams, StringArray * loadFileChains )
{
    loadFileChains->push_back( RealPath(configFile) );
    int lineCount = 0;
    int varsCount = 0;
    try
    {
        File fin( configFile, $T("r") );
        if ( !fin ) return varsCount;
        String line;
        while ( __GetLine( &fin, &line ) )
        {
            lineCount++;
            String tmp = StrTrim(line);

            if ( tmp.empty() || tmp[0] == '#' ) // '#'行开头表示注释,只有在行开头才表示注释
                continue;

            if ( tmp[0] == '@' ) // '@'开头表示配置命令,目前仅支持 @import other-config-file
            {
                int pos = (int)tmp.find(' ');
                String commandName, commandParam;
                if ( pos == -1 )
                {
                    commandName = tmp.substr(1); // 偏移1是为了 skip '@'
                    commandParam = $T("");
                }
                else
                {
                    commandName = tmp.substr( 1, pos - 1 );
                    commandParam = tmp.substr( pos + 1 );
                }
                if ( commandName == $T("import") ) // 导入外部配置
                {
                    String dirPath = FilePath(configFile);
                    StringArray callChains;
                    // 展开值中引用的变量值
                    String confPath = StripSlashes( _expandValNoStripSlashes( commandParam, &callChains ), ConfigVarsSlashChars );
                    confPath = IsAbsPath(confPath) ? confPath : CombinePath( dirPath, confPath );
                    if ( !ValueIsInArray( *loadFileChains, RealPath(confPath), true ) )
                    {
                        varsCount += _load( confPath, rawParams, loadFileChains );
                    }
                }
                continue;
            }

            int delimPos = (int)line.find('=');
            if ( delimPos == -1 ) // 找不到'='分隔符,就把整行当成参数名
                (*rawParams)[line] = $T("");
            else
                (*rawParams)[ line.substr( 0, delimPos ) ] = line.substr( delimPos + 1 );

            varsCount++;
        }
    }
    catch ( std::exception & )
    {
    }
    //loadFileChains->pop_back(); //注释掉这句，则每一个文件只载入一次
    return varsCount;
}

int Configure::load( String const & configFile )
{
    _configFile = configFile;
    StringArray loadFileChains;
    return _load( configFile, &_rawParams, &loadFileChains );
}

String Configure::get( String const & name, bool stripslashes, bool expand ) const
{
    String value;

    if ( expand )
    {
        StringArray callChains; // 递归调用链，防止无穷递归
        value = _expandVarNoStripSlashes( name, &callChains );
    }
    else
    {
        value = this->has(name) ? _rawParams.at(name) : $T("");
    }

    if ( stripslashes )
        value = StripSlashes( value, ConfigVarsSlashChars );

    return value;
}

String Configure::operator [] ( String const & name ) const
{
    return this->has(name) ? StripSlashes( _rawParams.at(name), ConfigVarsSlashChars ) : $T("");
}

String Configure::operator () ( String const & name ) const
{
    StringArray callChains; // 递归调用链，防止无穷递归
    return StripSlashes( _expandVarNoStripSlashes( name, &callChains ), ConfigVarsSlashChars );
}

void Configure::setRaw( String const & name, String const & value )
{
    _rawParams[name] = value;
}

void Configure::set( String const & name, String const & value )
{
    _rawParams[name] = AddSlashes( value, ConfigVarsSlashChars );
}

bool Configure::del( String const & name )
{
    if ( this->has(name) )
    {
        _rawParams.erase(name);
        return true;
    }
    return false;
}

void Configure::clear()
{
    _rawParams.clear();
}

// additional operators -----------------------------------------------------------------------
// operator @ ---------------------------------------------------------------------------------
static bool __OprFile( Expression const * e, ExprOperand * arOperands[], short n, winux::SimplePointer<ExprOperand> * outRetValue, void * data )
{
    outRetValue->attachNew(NULL);
    if ( n == 1 )
    {
        auto & confSettings = *reinterpret_cast<ConfigureSettings *>(data);
        String path, nl;
        size_t i = 0;
        // 只读取第一行作路径
        StrGetLine( &path, arOperands[0]->val().to<String>(), &i, &nl );
        // 基于配置文件目录计算路径
        path = IsAbsPath(path) ? path : CombinePath( confSettings.getSettingsDir(), path );

        ExprLiteral * ret = new ExprLiteral();
        ret->getValue().createString( FileGetString( path, feMultiByte ) );
        outRetValue->attachNew(ret);
        return true;
    }
    return false;
}

// additional functions -----------------------------------------------------------------------
// file( path[, textmode = true] ) path:文件路径，textmode:文本模式 默认true即默认文本模式
static bool __FuncFile( Expression * e, std::vector<Expression *> const & params, winux::SimplePointer<ExprOperand> * outRetValue, void * data )
{
    outRetValue->attachNew(NULL);
    if ( params.size() > 0 )
    {
        auto & confSettings = *reinterpret_cast<ConfigureSettings *>(data);
        String path, nl;
        size_t i = 0;
        // 只读取第一行作路径
        StrGetLine( &path, params[0]->val().to<String>(), &i, &nl );
        // 基于配置文件目录计算路径
        path = IsAbsPath(path) ? path : CombinePath( confSettings.getSettingsDir(), path );
        // 文本模式
        bool textmode = params.size() > 1 ? params[1]->val().to<bool>() : true;

        ExprLiteral * ret = new ExprLiteral();
        if ( textmode )
            ret->getValue().createString( FileGetString( path, feMultiByte ) );
        else
            ret->getValue().assign( FileGetContentsEx( path, false ) );
        outRetValue->attachNew(ret);
        return true;
    }
    else
    {
        throw ExprError( ExprError::eeFuncParamCountError, EXPRERRSTR_NOT_ENOUGH_PARAMETERS("file") );
    }
    return false;
}

// struct ConfigureSettings_Data --------------------------------------------------------------
struct ConfigureSettings_Data
{
    ExprPackage package; // 表达式语言包，包含自定义函数和算符
    VarContext rootCtx; // 根变量场景

    ConfigureSettings_Data()
    {
        this->package.setFunc( $T("file"), __FuncFile );
        this->package.addOpr( $T("@"), true, true, 1000, __OprFile );
    }
};

// class ConfigureSettings --------------------------------------------------------------------
ConfigureSettings::ConfigureSettings( String const & settingsFile )
{
    this->_collectionVal.createCollection();
    this->_collectionExpr.createCollection();
    _self->rootCtx.setMixedCollection(&this->_collectionVal);

    if ( !settingsFile.empty() )
    {
        this->load(settingsFile);
    }
}

ConfigureSettings::~ConfigureSettings()
{
}

ConfigureSettings::ConfigureSettings( ConfigureSettings const & other )
{
    _self = other._self;
}

ConfigureSettings::ConfigureSettings( ConfigureSettings && other ) : _self( std::move(other._self) )
{
}

ConfigureSettings & ConfigureSettings::operator = ( ConfigureSettings const & other )
{
    if ( this != &other )
    {
        _self = other._self;
    }
    return *this;
}

ConfigureSettings & ConfigureSettings::operator = ( ConfigureSettings && other )
{
    if ( this != &other )
    {
        _self = std::move(other._self);
    }
    return *this;
}

// 配置解析场景标记flag
enum ConfigureSettings_ParseContext
{
    cpcName,
    cpcValue,
    cpcString,
    cpcStrAntiSlashes,
    cpcExpr,
    cpcCollection,
};

// 一个值类型标记flag
enum OneValueType
{
    ovtAuto, // 0:自动判断数字或字符串
    ovtString, // 1:字符串
    ovtExpr // 2:表达式串
};

static void ConfigureSettings_ParseCollection( std::vector<ConfigureSettings_ParseContext> & cpc, winux::String const & str, int & i, Expression * exprCtx, winux::Mixed * collAsVal,  winux::Mixed * collAsExpr );

static void ConfigureSettings_ParseStrAntiSlashes( std::vector<ConfigureSettings_ParseContext> & cpc, winux::String const & str, int & i, winux::String * v )
{
    winux::String & result = *v;

    ++i; // skip '\\'

    while ( i < (int)str.length() )
    {
        winux::String::value_type ch = str[i];
        if ( ch == 'a' )
        {
            result += '\a';
            ++i;
            break;
        }
        else if ( ch == 'b' )
        {
            result += '\b';
            ++i;
            break;
        }
        else if ( ch == 't' )
        {
            result += '\t';
            ++i;
            break;
        }
        else if ( ch == 'n' )
        {
            result += '\n';
            ++i;
            break;
        }
        else if ( ch == 'v' )
        {
            result += '\v';
            ++i;
            break;
        }
        else if ( ch == 'f' )
        {
            result += '\f';
            ++i;
            break;
        }
        else if ( ch == 'r' )
        {
            result += '\r';
            ++i;
            break;
        }
        else if ( IsOct(ch) )
        {
            winux::String octStr;
            for ( ; i < (int)str.length(); ++i )
            {
                ch = str[i];
                if ( IsOct(ch) && octStr.length() < 3 )
                {
                    octStr += ch;
                }
                else
                {
                    break;
                }
            }

            result += NumberStringToChar( octStr.c_str(), 8 );

            break;
        }
        else if ( ( ch | 0x20 ) == 'x' ) // 解析\xHH
        {
            ++i; // skip 'x'
            winux::String hexStr;
            for ( ; i < (int)str.length(); ++i )
            {
                ch = str[i];
                if ( IsHex(ch) && hexStr.length() < 2 )
                {
                    hexStr += ch;
                }
                else
                {
                    break;
                }
            }

            result += NumberStringToChar( hexStr.c_str(), 16 );

            break;
        }
        else // 其余加\的字符都照原样输出
        {
            result += ch;
            ++i;
            break;
        }
    }
}

static void ConfigureSettings_ParseString( std::vector<ConfigureSettings_ParseContext> & cpc, winux::String const & str, int & i, winux::String * v )
{
    winux::String::value_type quote = str[i];

    v->assign( $T("") );
    winux::String & result = *v;

    i++; // skip left quote

    while ( i < (int)str.length() )
    {
        winux::String::value_type ch = str[i];
        if ( ch == quote )
        {
            i++; // skip right quote
            break;
        }
        else if ( ch == '\\' )
        {
            cpc.push_back(cpcStrAntiSlashes);
            ConfigureSettings_ParseStrAntiSlashes( cpc, str, i, v );
            cpc.pop_back();
        }
        else
        {
            result += ch;
            i++;
        }
    }
}

static void ConfigureSettings_ParseName( std::vector<ConfigureSettings_ParseContext> & cpc, winux::String const & str, int & i, winux::String * v )
{
    int start = i;

    v->assign( $T("") );
    winux::String & result = *v;

    while ( i < (int)str.length() )
    {
        winux::String::value_type ch = str[i];

        if ( IsSpace(ch) || ch == ';' )
        {
            break;
        }
        else if ( start == i && ( ch == '\'' || ch == '\"' ) )
        {
            cpc.push_back(cpcString);
            ConfigureSettings_ParseString( cpc, str, i, v );
            cpc.pop_back();

            break;
        }
        else if ( ch == '}' )
        {
            break;
        }
        else
        {
            result += ch;
            i++;
        }
    }
}

static void ConfigureSettings_ParseExpr( std::vector<ConfigureSettings_ParseContext> & cpc, winux::String const & str, int & i, winux::String * v )
{
    i++; // skip '('
    int brackets = 1; // 括号配对。0时表示一个表达式结束

    v->assign( $T("") );

    while ( i < (int)str.length() )
    {
        winux::String::value_type ch = str[i];
        if ( ch == ';' )
        {
            //i++;
            break;
        }
        else if ( ch == '\'' || ch == '\"' )
        {
            cpc.push_back(cpcString);
            winux::String tmp;
            ConfigureSettings_ParseString( cpc, str, i, &tmp );
            cpc.pop_back();

            *v += ch + winux::AddCSlashes(tmp) + ch;
        }
        else if ( ch == '(' )
        {
            ++brackets;

            *v += ch;
            i++;

        }
        else if ( ch == ')' )
        {
            if ( --brackets < 1 )
            {
                i++; // skip ')'
                break;
            }
            else
            {
                *v += ch;
                i++;
            }
        }
        else
        {
            *v += ch;
            i++;
        }
    }
}

static void _StoreValue( winux::String const & oneValue, OneValueType oneValueType, Expression * exprCtx, winux::Mixed & arr, winux::Mixed & arrExpr )
{
    if ( oneValue.empty() )
    {
        arr.add( $T("") );
        arrExpr.add( $T("\"\"") );
    }
    else
    {
        switch ( oneValueType )
        {
        case ovtAuto:
            if ( IsNumberString(oneValue) )
            {
                winux::Mixed r;
                try
                {
                    exprCtx->clear();
                    ExprParser().parse( exprCtx, oneValue );
                    r = exprCtx->val();
                }
                catch ( ExprError const & /*e*/ )
                {
                    //ColorOutput( fgRed, e.what() );
                }

                arr.add(r);
                arrExpr.add(oneValue);
            }
            else
            {
                arr.add(oneValue);
                arrExpr.add( $T("\"") + AddCSlashes(oneValue) + $T("\"") );
            }
            break;
        case ovtString:
            {
                arr.add(oneValue);
                arrExpr.add( $T("\"") + AddCSlashes(oneValue) + $T("\"") );
            }
            break;
        case ovtExpr:
            {
                winux::Mixed r;
                try
                {
                    exprCtx->clear();
                    ExprParser().parse( exprCtx, oneValue );
                    r = exprCtx->val();
                }
                catch ( ExprError const & /*e*/ )
                {
                    //ColorOutput( fgRed, e.what() );
                }

                arr.add(r);
                arrExpr.add( $T("(") + oneValue + $T(")") );
            }
        }
    }
}

static void ConfigureSettings_ParseValue( std::vector<ConfigureSettings_ParseContext> & cpc, winux::String const & str, int & i, Expression * exprCtx, winux::Mixed * value, winux::Mixed * valExpr )
{
    int start = i;

    winux::Mixed arr; // 数组，可能含有多个
    arr.createArray();
    winux::Mixed arrExpr; // 数组，可能含有多个
    arrExpr.createArray();

    winux::String oneValue; // 单个值串
    OneValueType oneValueType = ovtAuto; // 值串类型 0:自动判断数字或字符串  1:字符串  2:表达式串

    oneValue.assign( $T("") );

    while ( i < (int)str.length() )
    {
        winux::String::value_type ch = str[i];

        if ( ch == ';' ) // 遇到分号，说明值结束
        {
            if ( !oneValue.empty() )
            {
                _StoreValue( oneValue, oneValueType, exprCtx, arr, arrExpr );
                oneValue.assign( $T("") );
                oneValueType = ovtAuto;
            }
            else if ( oneValue.empty() && arr._pArr->size() == 0 ) // 遇到了结束分号却依旧没有读取到值，就加个空Mixed作值
            {
                _StoreValue( oneValue, oneValueType, exprCtx, arr, arrExpr );
                oneValue.assign( $T("") );
                oneValueType = ovtAuto;
            }

            i++; // skip ';'
            break;
        }
        else if ( IsSpace(ch) )
        {
            if ( !oneValue.empty() )
            {
                _StoreValue( oneValue, oneValueType, exprCtx, arr, arrExpr );
                oneValue.assign( $T("") );
                oneValueType = ovtAuto;
            }

            i++; // skip space-char
        }
        else if ( start == i && ( ch == '\'' || ch == '\"' ) )
        {
            cpc.push_back(cpcString);
            ConfigureSettings_ParseString( cpc, str, i, &oneValue );
            cpc.pop_back();

            oneValueType = ovtString;
        }
        else if ( start == i && ch == '(' ) // 表达式
        {
            cpc.push_back(cpcExpr);
            ConfigureSettings_ParseExpr( cpc, str, i, &oneValue );
            cpc.pop_back();

            oneValueType = ovtExpr;
        }
        else if ( ch == '{' ) // 又进入一个Collection
        {
            i++; // skip '{'

            winux::Mixed value;       // 存值
            winux::Mixed valExpr;     // 存值表达式串

            cpc.push_back(cpcCollection);

            value.createCollection();
            valExpr.createCollection();

            VarContext varCtx(&value);
            Expression exprSubScope( exprCtx->getPackage(), &varCtx, exprCtx, nullptr );

            ConfigureSettings_ParseCollection( cpc, str, i, &exprSubScope, &value, &valExpr );
            cpc.pop_back();

            arr.add(value);
            arrExpr.add(valExpr);

            oneValue.assign( $T("") );
            oneValueType = ovtAuto;

            // 如果是"}\n"，也说明值结束
            if ( i < (int)str.length() )
            {
                i++; // skip '}'
                if ( i < (int)str.length() )
                {
                    ch = str[i];
                    if ( ch == '\n' || ch == '\r' )
                    {
                        i++; // skip '\n' or '\r'
                        break;
                    }
                }
            }
        }
        else if ( ch == '}' )
        {
            break;
        }
        else
        {
            oneValue += ch;
            i++;
        }
    }

    if ( !oneValue.empty() )
    {
        _StoreValue( oneValue, oneValueType, exprCtx, arr, arrExpr );
        oneValue.assign( $T("") );
        oneValueType = ovtAuto;
    }

    if ( arr.getCount() == 1 )
    {
        *value = std::move(arr[0]);
    }
    else if ( arr.getCount() == 0 )
    {
        value->assign( $T("") );
    }
    else
    {
        *value = std::move(arr);
    }

    if ( arrExpr.getCount() == 1 )
    {
        *valExpr = std::move(arrExpr[0]);
    }
    else if ( arrExpr.getCount() == 0 )
    {
        valExpr->assign( $T("") );
    }
    else
    {
        *valExpr = std::move(arrExpr);
    }
}

static void ConfigureSettings_ParseCollection( std::vector<ConfigureSettings_ParseContext> & cpc, winux::String const & str, int & i, Expression * exprCtx, winux::Mixed * collAsVal,  winux::Mixed * collAsExpr )
{
    bool currIsName = true;   // 当前是否解析名字
    winux::String name;       // 存名字
    winux::Mixed value;       // 存值
    winux::Mixed valExpr;     // 存值表达式串
    bool hasName = false;     // 是否有名字
    bool hasValue = false;    // 是否有值

    while ( i < (int)str.length() )
    {
        winux::String::value_type ch = str[i];
        if ( IsSpace(ch) )
        {
            i++;
        }
        else if ( ch == '#' ) // 行注释
        {
            i++; // skip '#'

            while ( i < (int)str.length() && str[i] != '\n' ) i++; // 一直读取，直到'\n'

            if ( i < (int)str.length() ) i++; // skip '\n' if not end
        }
        else if ( ch == '}' )
        {
            break;
        }
        else
        {
            if ( currIsName )
            {
                cpc.push_back(cpcName);
                ConfigureSettings_ParseName( cpc, str, i, &name );
                cpc.pop_back();

                hasName = true;
                currIsName = false;
            }
            else
            {
                cpc.push_back(cpcValue);
                ConfigureSettings_ParseValue( cpc, str, i, exprCtx, &value, &valExpr );
                cpc.pop_back();

                hasValue = true;
                currIsName = true;

                if ( hasName && hasValue )
                {
                    exprCtx->getVarContext()->set( name, value );
                    (*collAsExpr)[name] = valExpr;

                    name.assign( $T("") );
                    value.free();
                    valExpr.free();
                    currIsName = true;
                    hasName = hasValue = false;
                }
                else if ( hasName )
                {
                    exprCtx->getVarContext()->set( name, winux::mxNull );
                    (*collAsExpr)[name] = valExpr;

                    name.assign( $T("") );
                    value.free();
                    valExpr.free();
                    currIsName = true;
                    hasName = hasValue = false;
                }
            }
        }
    }

    if ( hasName && hasValue )
    {
        exprCtx->getVarContext()->set( name, value );
        (*collAsExpr)[name] = valExpr;

    }
    else if ( hasName )
    {
        exprCtx->getVarContext()->set( name, winux::mxNull );
        (*collAsExpr)[name] = valExpr;
    }

}

size_t ConfigureSettings::_load( String const & settingsFile, winux::Mixed * collAsVal, winux::Mixed * collAsExpr, StringArray * loadFileChains )
{
    String settingsFileRealPath = RealPath(settingsFile);
    loadFileChains->push_back(settingsFileRealPath);

    String settingsContent = FileGetString( settingsFileRealPath, feMultiByte );
    {
        std::vector<ConfigureSettings_ParseContext> cpc;
        cpc.push_back(cpcCollection);

        if ( !collAsVal->isCollection() ) collAsVal->createCollection();
        if ( !collAsExpr->isCollection() ) collAsExpr->createCollection();

        VarContext varCtx(collAsVal);
        Expression exprCtx( &_self->package, &varCtx, nullptr, this );

        int i = 0;
        ConfigureSettings_ParseCollection( cpc, settingsContent, i, &exprCtx, collAsVal, collAsExpr );

        cpc.pop_back();
    }

    //loadFileChains->pop_back(); //注释掉这句，则每一个文件只载入一次

    _self->rootCtx.setMixedCollection(&this->_collectionVal); // 每次载入完设置后重新设置Root变量场景

    return this->_collectionVal.getCount();
}

size_t ConfigureSettings::load( String const & settingsFile )
{
    this->_settingsFile = settingsFile;
    StringArray loadFileChains;
    return this->_load( settingsFile, &this->_collectionVal, &this->_collectionExpr, &loadFileChains );
}

static void _Update( struct ConfigureSettings_Data & sd, Expression * parent, Mixed & collVal, Mixed & collExpr, StringArray const & names, String const & updateExprStr, Mixed * * v )
{
    if ( collVal.isCollection() ) // 集合，应该创建表达式变量场景对象
    {
        if ( names.size() > 0 )
        {
            VarContext ctx(&collVal);
            Expression exprCtx( &sd.package, &ctx, parent, ( parent ? parent->getDataPtr() : nullptr ) );

            StringArray newNames( names.begin() + 1, names.end() );

            _Update( sd, &exprCtx, collVal[ names[0] ], collExpr[ names[0] ], newNames, updateExprStr, v );
        }
        else
        {
            *v = &collVal;
        }
    }
    else if ( collVal.isArray() ) // 数组
    {
        if ( names.size() > 0 )
        {
            StringArray newNames( names.begin() + 1, names.end() );

            _Update( sd, parent, collVal[ names[0] ], collExpr[ names[0] ], newNames, updateExprStr, v );
        }
        else
        {
            *v = &collVal;
        }
    }
    else // 非容器，应该通过表达式计算值
    {
        Expression expr( &sd.package, nullptr, parent, ( parent ? parent->getDataPtr() : nullptr ) );

        if ( !updateExprStr.empty() )
        {
            collExpr = updateExprStr;
        }

        ExprParser().parse( &expr, collExpr );
        collVal = expr.val();
        *v = &collVal;
    }
}

Mixed & ConfigureSettings::update( String const & multiname, String const & updateExprStr )
{
    StringArray names;

    // 这里并不是计算，而是解析层次，记下每层的名字
    Expression te( &_self->package, nullptr, nullptr, nullptr );
    ExprParser().parse( &te, multiname );

    for ( ExprAtom * atom : te._suffixAtoms )
    {
        if ( atom->getAtomType() == ExprAtom::eatOperand )
        {
            ExprOperand * opd = (ExprOperand *)atom;

            if ( opd->getOperandType() == ExprOperand::eotLiteral )
            {
                names.push_back( ((ExprLiteral *)opd)->getValue() );
            }
            else
            {
                names.push_back( atom->toString() );
            }
        }
    }

    Mixed * v = nullptr;
    _Update( _self, nullptr, this->_collectionVal, this->_collectionExpr, names, updateExprStr, &v );
    return *v;
}

Mixed & ConfigureSettings::execRef( String const & exprStr ) const
{
    thread_local Mixed inner;
    try
    {
        Expression expr( const_cast<ExprPackage*>(&_self->package), const_cast<VarContext*>(&_self->rootCtx), nullptr, nullptr );
        ExprParser().parse( &expr, exprStr );
        Mixed * v = nullptr;
        if ( expr.evaluateMixedPtr(&v) )
        {
            return *v;
        }
    }
    catch ( winux::ExprError const & /*e*/ )
    {
    }
    inner.free();
    return inner;
}

Mixed ConfigureSettings::execVal( String const & exprStr, Mixed const & defval ) const
{
    try
    {
        Expression expr( const_cast<ExprPackage*>(&_self->package), const_cast<VarContext*>(&_self->rootCtx), nullptr, nullptr );
        ExprParser().parse( &expr, exprStr );
        return expr.val();
    }
    catch ( winux::ExprError const & /*e*/ )
    {
    }
    return defval;
}

Mixed const & ConfigureSettings::operator [] ( String const & name ) const
{
    return this->get(name);
}

Mixed & ConfigureSettings::operator [] ( String const & name )
{
    return this->_collectionVal[name];
}

bool ConfigureSettings::has( String const & name ) const
{
    return this->_collectionVal.has(name);
}

winux::Mixed const & ConfigureSettings::get( String const & name ) const
{
    thread_local Mixed const inner;
    return this->_collectionVal.has(name) ? this->_collectionVal[name] : inner;
}

ConfigureSettings & ConfigureSettings::set( String const & name, Mixed const & value )
{
    this->_collectionVal[name] = value;
    return *this;
}

Mixed const & ConfigureSettings::val() const
{
    return this->_collectionVal;
}

Mixed & ConfigureSettings::val()
{
    return this->_collectionVal;
}

Mixed const & ConfigureSettings::expr() const
{
    return this->_collectionExpr;
}

Mixed & ConfigureSettings::expr()
{
    return this->_collectionExpr;
}

String ConfigureSettings::getSettingsDir() const
{
    return FilePath(this->_settingsFile);
}

// class CsvWriter ------------------------------------------------------------------------
inline static String __JudgeAddQuotes( String const & valStr )
{
    String::const_iterator it = valStr.begin();
    for ( ; it != valStr.end(); ++it )
    {
        if ( *it == ',' || *it == '\"' ||  *it == '\'' || *it == '\n' )
        {
            break;
        }
    }
    if ( it != valStr.end() )
    {
        return $T("\"") + AddQuotes( valStr, Literal<String::value_type>::quoteChar ) + $T("\"");
    }
    else
    {
        return valStr;
    }
}

CsvWriter::CsvWriter( IFile * outputFile ) : _outputFile(outputFile)
{
}

void CsvWriter::write( Mixed const & records, Mixed const & columnHeaders )
{
    if ( columnHeaders.isArray() )
    {
        this->writeRecord(columnHeaders);
    }
    if ( records.isArray() ) // 多条记录
    {
        size_t i, n = records.getCount();
        for ( i = 0; i < n; i++ )
        {
            this->writeRecord(records[i]);
        }
    }
    else // 只有一条记录
    {
        this->writeRecord(records);
    }
}

void CsvWriter::writeRecord( Mixed const & record )
{
    if ( record.isArray() ) // 多个列
    {
        size_t i, n = record.getCount();
        String strRecord;
        for ( i = 0; i < n; i++ )
        {
            if ( i != 0 ) strRecord += $T(",");
            strRecord += __JudgeAddQuotes(record[i]);
        }
        _outputFile->puts( strRecord + $T("\n") );
    }
    else if ( record.isCollection() )
    {
        size_t i, n = record.getCount();
        String strRecord;
        for ( i = 0; i < n; i++ )
        {
            if ( i != 0 ) strRecord += $T(",");
            strRecord += __JudgeAddQuotes( record.getPair(i).second );
        }
        _outputFile->puts( strRecord + $T("\n") );
    }
    else // 只有1列
    {
        _outputFile->puts( __JudgeAddQuotes(record) + $T("\n") );
    }
}

// class CsvReader ----------------------------------------------------------------------------
CsvReader::CsvReader( IFile * inputFile, bool hasColumnHeaders )
{
    if ( inputFile ) this->read( inputFile->entire(), hasColumnHeaders );
}

CsvReader::CsvReader( String const & content, bool hasColumnHeaders )
{
    if ( !content.empty() ) this->read( content, hasColumnHeaders );
}

inline static void _ReadString( String const & str, size_t * pI, String * valStr )
{
    size_t & i = *pI;
    ++i; // skip '\"'
    while ( i < str.length() )
    {
        String::value_type ch = str[i];
        if ( ch == '\"' )
        {
            if ( i + 1 < str.length() && str[i + 1] == '\"' ) // double '\"' 解析成一个'\"'
            {
                *valStr += '\"';
                i++; // skip "
                i++; // skip "
            }
            else
            {
                i++; // skip 作为字符串结束的尾"
                break;
            }
        }
        else
        {
            *valStr += ch;
            i++;
        }
    }
}

inline static void _ReadRecord( String const & str, size_t * pI, Mixed * record )
{
    size_t & i = *pI;
    record->createArray();
    String valStr;
    while ( i < str.length() )
    {
        String::value_type ch = str[i];
        if ( ch == '\n' ) // 结束一条记录
        {
            record->add(valStr);
            i++; // skip '\n'
            break;
        }
        else if ( ch == ',' ) // 结束一个值
        {
            record->add(valStr);
            valStr.clear();
            i++; // skip ','
        }
        else if ( ch == '\"' )
        {
            // 去除之前可能的空白
            if ( StrTrim(valStr).empty() )
            {
                valStr.clear(); // 去除之前可能获得的空白字符
                _ReadString( str, &i, &valStr );
                //record.add(valStr);
                //valStr.clear();
                // 跳过','以避免再次加入这个值,或者跳过换行符
                //while ( i < str.length() && str[i] != ',' && str[i] != '\n' ) i++;
            }
            else
            {
                valStr += ch;
                i++;
            }
        }
        else
        {
            valStr += ch;
            i++;
        }
    }

    if ( str.length() > 0 && '\n' != str[i - 1] ) // 最后一个字符不是换行符
    {
        record->add(valStr);
    }
}

void CsvReader::read( String const & content, bool hasColumnHeaders )
{
    size_t i = 0;
    if ( hasColumnHeaders )
    {
        Mixed hdrs;
        _ReadRecord( content, &i, &hdrs );
        _columns.createCollection();
        for ( size_t i = 0, n = hdrs.getCount(); i < n; ++i )
        {
            _columns[ hdrs[i] ] = i;
        }
    }

    _records.createArray();
    while ( i < content.length() )
    {
        Mixed & record = _records[ _records.add(mxNull) ];
        _ReadRecord( content, &i, &record );
    }
}


// class TextArchive --------------------------------------------------------------------------
void TextArchive::saveEx( winux::Buffer const & content, winux::AnsiString const & encoding, winux::GrowBuffer * output, FileEncoding fileEncoding )
{
    switch ( fileEncoding )
    {
    case feMultiByte:
        {
            winux::Conv conv{ encoding, this->_mbsEncoding };
            char * buf;
            size_t n = conv.convert( content.getBuf<char>(), content.getSize(), &buf );
            output->appendString( NewlineToFile( buf, n, false ) );
            Buffer::Free(buf);
        }
        break;
    case feUtf8:
        {
            winux::Conv conv{ encoding, "UTF-8" };
            char * buf;
            size_t n = conv.convert( content.getBuf<char>(), content.getSize(), &buf );
            output->appendString( NewlineToFile( buf, n, false ) );
            Buffer::Free(buf);
        }
        break;
    case feUtf8Bom:
        {
            winux::Conv conv{ encoding, "UTF-8" };
            char * buf;
            size_t n = conv.convert( content.getBuf<char>(), content.getSize(), &buf );
            // write BOM
            output->appendType( { '\xef', '\xbb', '\xbf' } );
            output->appendString( NewlineToFile( buf, n, false ) );
            Buffer::Free(buf);
        }
        break;
    case feUtf16Le:
        {
            winux::Conv conv{ encoding, "UTF-16LE" };
            char * buf;
            size_t n = conv.convert( content.getBuf<char>(), content.getSize(), &buf );
            // write BOM
            output->appendType( { '\xff', '\xfe' } );
            winux::Utf16String res2 = NewlineToFile( (winux::Utf16String::value_type *)buf, n / sizeof(winux::Utf16String::value_type), IsBigEndian() );
            Buffer::Free(buf);
            output->appendString(res2);
        }
        break;
    case feUtf16Be:
        {
            winux::Conv conv{ encoding, "UTF-16BE" };
            char * buf;
            size_t n = conv.convert( content.getBuf<char>(), content.getSize(), &buf );
            // write BOM
            output->appendType( { '\xfe', '\xff' } );
            winux::Utf16String res2 = NewlineToFile( (winux::Utf16String::value_type *)buf, n / sizeof(winux::Utf16String::value_type), IsLittleEndian() );
            Buffer::Free(buf);
            output->appendString(res2);
        }
        break;
    case feUtf32Le:
        {
            winux::Conv conv{ encoding, "UTF-32LE" };
            char * buf;
            size_t n = conv.convert( content.getBuf<char>(), content.getSize(), &buf );
            // write BOM
            output->appendType( { '\xff', '\xfe', '\0', '\0' } );
            winux::Utf32String res2 = NewlineToFile( (winux::Utf32String::value_type *)buf, n / sizeof(winux::Utf32String::value_type), IsBigEndian() );
            Buffer::Free(buf);
            output->appendString(res2);
        }
        break;
    case feUtf32Be:
        {
            winux::Conv conv{ encoding, "UTF-32BE" };
            char * buf;
            size_t n = conv.convert( content.getBuf<char>(), content.getSize(), &buf );
            // write BOM
            output->appendType( { '\0', '\0', '\xfe', '\xff' } );
            winux::Utf32String res2 = NewlineToFile( (winux::Utf32String::value_type *)buf, n / sizeof(winux::Utf32String::value_type), IsLittleEndian() );
            Buffer::Free(buf);
            output->appendString(res2);
        }
        break;
    }
}

void TextArchive::_processContent( winux::Buffer const & content, bool isConvert, winux::AnsiString const & encoding )
{
    switch ( _fileEncoding )
    {
    case feMultiByte:
        {
            winux::XString<char> strContent = NewlineFromFile( content.get<char>(), content.size(), false );
            if ( isConvert )
            {
                winux::Conv conv{ this->_mbsEncoding, encoding };
                char * buf;
                size_t n = conv.convert( (char const *)strContent.c_str(), strContent.length() * sizeof(char), &buf );
                this->_pureContent.setBuf( buf, n, false );
                Buffer::Free(buf);
                this->setContentEncoding(encoding);
            }
            else
            {
                this->_pureContent.setBuf( (void *)strContent.c_str(), strContent.length() * sizeof(char), false );
                this->setContentEncoding(this->_mbsEncoding);
            }
        }
        break;
    case feUtf8:
        {
            winux::XString<char> strContent = NewlineFromFile( content.get<char>(), content.size(), false );
            if ( isConvert )
            {
                winux::Conv conv{ "UTF-8", encoding };
                char * buf;
                size_t n = conv.convert( (char const *)strContent.c_str(), strContent.length() * sizeof(char), &buf );
                this->_pureContent.setBuf( buf, n, false );
                Buffer::Free(buf);
                this->setContentEncoding(encoding);
            }
            else
            {
                this->_pureContent.setBuf( (void *)strContent.c_str(), strContent.length() * sizeof(char), false );
                this->setContentEncoding("UTF-8");
            }
        }
        break;
    case feUtf8Bom:
        {
            winux::XString<char> strContent = NewlineFromFile( content.get<char>(), content.size(), false );
            if ( isConvert )
            {
                winux::Conv conv{ "UTF-8", encoding };
                char * buf;
                size_t n = conv.convert( (char const *)strContent.c_str(), strContent.length() * sizeof(char), &buf );
                this->_pureContent.setBuf( buf, n, false );
                Buffer::Free(buf);
                this->setContentEncoding(encoding);
            }
            else
            {
                this->_pureContent.setBuf( (void *)strContent.c_str(), strContent.length() * sizeof(char), false );
                this->setContentEncoding("UTF-8");
            }
        }
        break;
    case feUtf16Le:
        {
            winux::XString<char16> strContent = NewlineFromFile( content.get<char16>(), content.size() / sizeof(char16), IsBigEndian() );
            if ( isConvert )
            {
                winux::Conv conv{ "UTF-16LE", encoding };
                char * buf;
                size_t n = conv.convert( (char const *)strContent.c_str(), strContent.length() * sizeof(char16), &buf );
                this->_pureContent.setBuf( buf, n, false );
                Buffer::Free(buf);
                this->setContentEncoding(encoding);
            }
            else
            {
                this->_pureContent.setBuf( (void *)strContent.c_str(), strContent.length() * sizeof(char16), false );
                this->setContentEncoding("UTF-16LE");
            }
        }
        break;
    case feUtf16Be:
        {
            winux::XString<char16> strContent = NewlineFromFile( content.get<char16>(), content.size() / sizeof(char16), IsLittleEndian() );
            if ( isConvert )
            {
                winux::Conv conv{ "UTF-16BE", encoding };
                char * buf;
                size_t n = conv.convert( (char const *)strContent.c_str(), strContent.length() * sizeof(char16), &buf );
                this->_pureContent.setBuf( buf, n, false );
                Buffer::Free(buf);
                this->setContentEncoding(encoding);
            }
            else
            {
                this->_pureContent.setBuf( (void *)strContent.c_str(), strContent.length() * sizeof(char16), false );
                this->setContentEncoding("UTF-16BE");
            }
        }
        break;
    case feUtf32Le:
        {
            winux::XString<char32> strContent = NewlineFromFile( content.get<char32>(), content.size() / sizeof(char32), IsBigEndian() );
            if ( isConvert )
            {
                winux::Conv conv{ "UTF-32LE", encoding };
                char * buf;
                size_t n = conv.convert( (char const *)strContent.c_str(), strContent.length() * sizeof(char32), &buf );
                this->_pureContent.setBuf( buf, n, false );
                Buffer::Free(buf);
                this->setContentEncoding(encoding);
            }
            else
            {
                this->_pureContent.setBuf( (void *)strContent.c_str(), strContent.length() * sizeof(char32), false );
                this->setContentEncoding("UTF-32LE");
            }
        }
        break;
    case feUtf32Be:
        {
            winux::XString<char32> strContent = NewlineFromFile( content.get<char32>(), content.size() / sizeof(char32), IsLittleEndian() );
            if ( isConvert )
            {
                winux::Conv conv{ "UTF-32BE", encoding };
                char * buf;
                size_t n = conv.convert( (char const *)strContent.c_str(), strContent.length() * sizeof(char32), &buf );
                this->_pureContent.setBuf( buf, n, false );
                Buffer::Free(buf);
                this->setContentEncoding(encoding);
            }
            else
            {
                this->_pureContent.setBuf( (void *)strContent.c_str(), strContent.length() * sizeof(char32), false );
                this->setContentEncoding("UTF-32BE");
            }
        }
        break;
    }
}

void TextArchive::_recognizeEncode( winux::Buffer const & content, size_t * pI )
{
    this->_fileEncoding = RecognizeFileEncoding( content, pI, 1024 );
}


} // namespace winux
