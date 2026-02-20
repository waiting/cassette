#include "eiendb_common.hpp"
#include "eiendb_pgsql.hpp"

#ifdef HAVE_DB_PGSQL

#include <libpq-fe.h>
#define CASHOID	MONEYOID
#define LSNOID	PG_LSNOID

#define BOOLOID 16
#define BYTEAOID 17
#define CHAROID 18
#define NAMEOID 19
#define INT8OID 20
#define INT2OID 21
#define INT2VECTOROID 22
#define INT4OID 23
#define REGPROCOID 24
#define TEXTOID 25
#define OIDOID 26
#define TIDOID 27
#define XIDOID 28
#define CIDOID 29
#define OIDVECTOROID 30
#define JSONOID 114
#define XMLOID 142
#define PG_NODE_TREEOID 194
#define PG_NDISTINCTOID 3361
#define PG_DEPENDENCIESOID 3402
#define PG_MCV_LISTOID 5017
#define PG_DDL_COMMANDOID 32
#define XID8OID 5069
#define POINTOID 600
#define LSEGOID 601
#define PATHOID 602
#define BOXOID 603
#define POLYGONOID 604
#define LINEOID 628
#define FLOAT4OID 700
#define FLOAT8OID 701
#define UNKNOWNOID 705
#define CIRCLEOID 718
#define MONEYOID 790
#define MACADDROID 829
#define INETOID 869
#define CIDROID 650
#define MACADDR8OID 774
#define ACLITEMOID 1033
#define BPCHAROID 1042
#define VARCHAROID 1043
#define DATEOID 1082
#define TIMEOID 1083
#define TIMESTAMPOID 1114
#define TIMESTAMPTZOID 1184
#define INTERVALOID 1186
#define TIMETZOID 1266
#define BITOID 1560
#define VARBITOID 1562
#define NUMERICOID 1700
#define REFCURSOROID 1790
#define REGPROCEDUREOID 2202
#define REGOPEROID 2203
#define REGOPERATOROID 2204
#define REGCLASSOID 2205
#define REGCOLLATIONOID 4191
#define REGTYPEOID 2206
#define REGROLEOID 4096
#define REGNAMESPACEOID 4089
#define UUIDOID 2950
#define PG_LSNOID 3220
#define TSVECTOROID 3614
#define GTSVECTOROID 3642
#define TSQUERYOID 3615
#define REGCONFIGOID 3734
#define REGDICTIONARYOID 3769
#define JSONBOID 3802
#define JSONPATHOID 4072
#define TXID_SNAPSHOTOID 2970
#define PG_SNAPSHOTOID 5038
#define INT4RANGEOID 3904
#define NUMRANGEOID 3906
#define TSRANGEOID 3908
#define TSTZRANGEOID 3910
#define DATERANGEOID 3912
#define INT8RANGEOID 3926
#define INT4MULTIRANGEOID 4451
#define NUMMULTIRANGEOID 4532
#define TSMULTIRANGEOID 4533
#define TSTZMULTIRANGEOID 4534
#define DATEMULTIRANGEOID 4535
#define INT8MULTIRANGEOID 4536
#define RECORDOID 2249
#define RECORDARRAYOID 2287
#define CSTRINGOID 2275
#define ANYOID 2276
#define ANYARRAYOID 2277
#define VOIDOID 2278
#define TRIGGEROID 2279
#define EVENT_TRIGGEROID 3838
#define LANGUAGE_HANDLEROID 2280
#define INTERNALOID 2281
#define ANYELEMENTOID 2283
#define ANYNONARRAYOID 2776
#define ANYENUMOID 3500
#define FDW_HANDLEROID 3115
#define INDEX_AM_HANDLEROID 325
#define TSM_HANDLEROID 3310
#define TABLE_AM_HANDLEROID 269
#define ANYRANGEOID 3831
#define ANYCOMPATIBLEOID 5077
#define ANYCOMPATIBLEARRAYOID 5078
#define ANYCOMPATIBLENONARRAYOID 5079
#define ANYCOMPATIBLERANGEOID 5080
#define ANYMULTIRANGEOID 4537
#define ANYCOMPATIBLEMULTIRANGEOID 4538
#define PG_BRIN_BLOOM_SUMMARYOID 4600
#define PG_BRIN_MINMAX_MULTI_SUMMARYOID 4601
#define BOOLARRAYOID 1000
#define BYTEAARRAYOID 1001
#define CHARARRAYOID 1002
#define NAMEARRAYOID 1003
#define INT8ARRAYOID 1016
#define INT2ARRAYOID 1005
#define INT2VECTORARRAYOID 1006
#define INT4ARRAYOID 1007
#define REGPROCARRAYOID 1008
#define TEXTARRAYOID 1009
#define OIDARRAYOID 1028
#define TIDARRAYOID 1010
#define XIDARRAYOID 1011
#define CIDARRAYOID 1012
#define OIDVECTORARRAYOID 1013
#define PG_TYPEARRAYOID 210
#define PG_ATTRIBUTEARRAYOID 270
#define PG_PROCARRAYOID 272
#define PG_CLASSARRAYOID 273
#define JSONARRAYOID 199
#define XMLARRAYOID 143
#define XID8ARRAYOID 271
#define POINTARRAYOID 1017
#define LSEGARRAYOID 1018
#define PATHARRAYOID 1019
#define BOXARRAYOID 1020
#define POLYGONARRAYOID 1027
#define LINEARRAYOID 629
#define FLOAT4ARRAYOID 1021
#define FLOAT8ARRAYOID 1022
#define CIRCLEARRAYOID 719
#define MONEYARRAYOID 791
#define MACADDRARRAYOID 1040
#define INETARRAYOID 1041
#define CIDRARRAYOID 651
#define MACADDR8ARRAYOID 775
#define ACLITEMARRAYOID 1034
#define BPCHARARRAYOID 1014
#define VARCHARARRAYOID 1015
#define DATEARRAYOID 1182
#define TIMEARRAYOID 1183
#define TIMESTAMPARRAYOID 1115
#define TIMESTAMPTZARRAYOID 1185
#define INTERVALARRAYOID 1187
#define TIMETZARRAYOID 1270
#define BITARRAYOID 1561
#define VARBITARRAYOID 1563
#define NUMERICARRAYOID 1231
#define REFCURSORARRAYOID 2201
#define REGPROCEDUREARRAYOID 2207
#define REGOPERARRAYOID 2208
#define REGOPERATORARRAYOID 2209
#define REGCLASSARRAYOID 2210
#define REGCOLLATIONARRAYOID 4192
#define REGTYPEARRAYOID 2211
#define REGROLEARRAYOID 4097
#define REGNAMESPACEARRAYOID 4090
#define UUIDARRAYOID 2951
#define PG_LSNARRAYOID 3221
#define TSVECTORARRAYOID 3643
#define GTSVECTORARRAYOID 3644
#define TSQUERYARRAYOID 3645
#define REGCONFIGARRAYOID 3735
#define REGDICTIONARYARRAYOID 3770
#define JSONBARRAYOID 3807
#define JSONPATHARRAYOID 4073
#define TXID_SNAPSHOTARRAYOID 2949
#define PG_SNAPSHOTARRAYOID 5039
#define INT4RANGEARRAYOID 3905
#define NUMRANGEARRAYOID 3907
#define TSRANGEARRAYOID 3909
#define TSTZRANGEARRAYOID 3911
#define DATERANGEARRAYOID 3913
#define INT8RANGEARRAYOID 3927
#define INT4MULTIRANGEARRAYOID 6150
#define NUMMULTIRANGEARRAYOID 6151
#define TSMULTIRANGEARRAYOID 6152
#define TSTZMULTIRANGEARRAYOID 6153
#define DATEMULTIRANGEARRAYOID 6155
#define INT8MULTIRANGEARRAYOID 6157
#define CSTRINGARRAYOID 1263

#endif

namespace eiendb
{
#ifdef HAVE_DB_PGSQL

namespace internal
{
#include "is_x_funcs.inl"

#define TRY_LOAD_LINE( ifile, i, line ) ( i < line.length() || ( i = 0, ( line = ifile->getLine() ).length() ) )

static winux::String __PgsqlLoadSqlFile_String( winux::IFile * ifile, size_t & i, winux::String & line )
{
    winux::String::value_type quote = line[i];
    winux::String s;

    i++; // skip quote
    s += quote;

    while ( TRY_LOAD_LINE( ifile, i, line ) )
    {
        winux::String::value_type ch = line[i];
        if ( ch == quote )
        {
            s += ch;
            i++;
            // 判断后面还有字符没有
            if ( TRY_LOAD_LINE( ifile, i, line ) ) // 还有字符
            {
                // 判断引号之后是不是还连着一个引号
                ch = line[i];
                if ( ch == quote )
                {
                    s += ch;
                    i++;
                }
                else // 后面是其他字符
                {
                    // 结束字符串解析
                    break;
                }
            }
            else // 没有字符了
            {
                // 结束字符串解析
                break;
            }
        }
        else if ( ch == '\\' ) // 转义字符
        {
            s += ch;
            i++;
            while ( TRY_LOAD_LINE( ifile, i, line ) )
            {
                s += line[i];
                i++;
                break;
            }
        }
        else // 其他字符
        {
            s += ch;
            i++;
        }
    }

    return s;
}

static size_t __PgsqlLoadSqlFile( winux::IFile * ifile, winux::StringArray * arrSql, IDbConnection * cnn )
{
    winux::String sql;
    winux::String line;
    size_t nSql = 0;
    size_t i = 0;

    while ( TRY_LOAD_LINE( ifile, i, line ) )
    {
        winux::String::value_type ch = line[i];
        if ( i == 0 && ch == '-' ) // 一行开头
        {
            i++; // skip first '-'
            if ( TRY_LOAD_LINE( ifile, i, line ) )
            {
                ch = line[i];
                if ( ch == '-' ) // equal "--" start
                {
                    i = line.length(); // skip a line
                }
                else
                {
                    sql += '-';
                    sql += ch;
                    i++;
                }
            }
            else
            {
                break;
            }
        }
        else if ( ch == '\'' || ch == '\"' )
        {
            sql += __PgsqlLoadSqlFile_String( ifile, i, line );
        }
        else if ( ch == ';' )
        {
            sql += ch;
            arrSql->push_back(sql);
            sql.clear();
            nSql++;
            i++;
        }
        else
        {
            // 当sql不为空时，空白字符才予加入
            if ( sql.empty() )
            {
                if ( !IsSpace(ch) )
                {
                    sql += ch;
                }
            }
            else
            {
                sql += ch;
            }
            i++;
        }
    }

    if ( winux::StrTrim(sql).length() > 0 )
    {
        arrSql->push_back(sql);
        sql.clear();
        nSql++;
    }
    return nSql;
}

// PostgreSQL Wrappers

// 类型名
inline static winux::String __TypeName( Oid id )
{
    switch ( id )
    {
    case BOOLOID: // 16
        return "bool";
    case BYTEAOID: // 17
        return "bytea";
    case CHAROID: // 18
        return "char";
    case NAMEOID: // 19
        return "name";
    case INT8OID: // 20
        return "int8";
    case INT2OID: // 21
        return "int2";
    case INT2VECTOROID: // 22
        return "int2vector";
    case INT4OID: // 23
        return "int4";
    case REGPROCOID: // 24
        return "regproc";
    case TEXTOID: // 25
        return "text";
    case OIDOID: // 26
        return "oid";
    case TIDOID: // 27
        return "tid";
    case XIDOID: // 28
        return "xid";
    case CIDOID: // 29
        return "cid";
    case OIDVECTOROID: // 30
        return "oidvector";
    case JSONOID: // 114
        return "json";
    case XMLOID: // 142
        return "xml";
    case POINTOID: // 600
        return "point";
    case LSEGOID: // 601
        return "lseg";
    case PATHOID: // 602
        return "path";
    case BOXOID: // 603
        return "box";
    case POLYGONOID: // 604
        return "polygon";
    case LINEOID: // 628
        return "line";
    case CIDROID: // 650
        return "cidr";
    case FLOAT4OID: // 700
        return "float4";
    case FLOAT8OID: // 701
        return "float8";
    case CIRCLEOID: // 718
        return "circle";
    case MACADDR8OID: // 774
        return "macaddr8";
    case MONEYOID: // 790
        return "money";
    case MACADDROID: // 829
        return "macaddr";
    case INETOID: // 869
        return "inet";
    case ACLITEMOID: // 1033
        return "aclitem";
    case BPCHAROID: // 1042
        return "bpchar";
    case VARCHAROID: // 1043
        return "varchar";
    case DATEOID: // 1082
        return "date";
    case TIMEOID: // 1083
        return "time";
    case TIMESTAMPOID: // 1114
        return "timestamp";
    case TIMESTAMPTZOID: // 1184
        return "timestamptz";
    case INTERVALOID: // 1186
        return "interval";
    case TIMETZOID: // 1266
        return "timetz";
    case BITOID: // 1560
        return "bit";
    case VARBITOID: // 1562
        return "varbit";
    case NUMERICOID: // 1700
        return "numeric";
    case REFCURSOROID: // 1790
        return "refcursor";
    case REGPROCEDUREOID: // 2202
        return "regprocedure";
    case REGOPEROID: // 2203
        return "regoper";
    case REGOPERATOROID: // 2204
        return "regoperator";
    case REGCLASSOID: // 2205
        return "regclass";
    case REGTYPEOID: // 2206
        return "regtype";
    case UUIDOID: // 2950
        return "uuid";
    case TXID_SNAPSHOTOID: // 2970
        return "txid_snapshot";
    case TSVECTOROID: // 3614
        return "tsvector";
    case TSQUERYOID: // 3615
        return "tsquery";
    case GTSVECTOROID: // 3642
        return "gtsvector";
    case REGCONFIGOID: // 3734
        return "regconfig";
    case REGDICTIONARYOID: // 3769
        return "regdictionary";
    case JSONBOID: // 3802
        return "jsonb";
    case JSONPATHOID: // 4072
        return "jsonpath";
    case REGNAMESPACEOID: // 4089
        return "regnamespace";
    case REGROLEOID: // 4096
        return "regrole";
    case REGCOLLATIONOID: // 4191
        return "regcollation";
    case XID8OID: // 5069
        return "xid8";
    default:
        return "unknown";
    }
}

enum DataFormat
{
    dataTextual,
    dataBinary
};

struct PgResultWrapper
{
public:
    PgResultWrapper( PGresult * res, bool isNewRes ) : _res(res), _isNewRes(isNewRes) { }
    ~PgResultWrapper()
    {
        this->clear();
    }
    PgResultWrapper( PgResultWrapper && other )
    {
        _res = other._res;
        _isNewRes = other._isNewRes;
        other._res = nullptr;
        other._isNewRes = false;
    }
    PgResultWrapper & operator = ( PgResultWrapper && other )
    {
        if ( this != &other )
        {
            this->clear();

            _res = other._res;
            _isNewRes = other._isNewRes;
            other._res = nullptr;
            other._isNewRes = false;
        }
        return *this;
    }

    void clear()
    {
        if ( _res )
        {
            if ( _isNewRes ) PQclear(_res);
            _res = nullptr;
            _isNewRes = false;
        }
    }

    int ntuples()
    {
        return PQntuples(_res);
    }

    int nfields()
    {
        return PQnfields(_res);
    }

    winux::String fname( int columnIndex )
    {
        return winux::CharSeqToString( PQfname( _res, columnIndex ) );
    }

    int findex( winux::String const & columnName )
    {
        return PQfnumber( _res, columnName.c_str() );
    }

    DataFormat fformat( int columnIndex )
    {
        return (DataFormat)PQfformat( _res, columnIndex );
    }

    Oid ftype( int columnIndex )
    {
        return PQftype( _res, columnIndex );
    }

    int fmodifier( int columnIndex )
    {
        return PQfmod( _res, columnIndex );
    }

    int fsize( int columnIndex )
    {
        return PQfsize( _res, columnIndex );
    }

    char * getvalue( int rowIndex, int columnIndex )
    {
        return PQgetvalue( _res, rowIndex, columnIndex );
    }

    int getlength( int rowIndex, int columnIndex )
    {
        return PQgetlength( _res, rowIndex, columnIndex );
    }

    bool getisnull( int rowIndex, int columnIndex )
    {
        return (bool)PQgetisnull( _res, rowIndex, columnIndex );
    }

    winux::String error()
    {
        return winux::CharSeqToString( PQresultErrorMessage(_res) );
    }

    ExecStatusType status()
    {
        return PQresultStatus(_res);
    }

    PGresult * _res;
    bool _isNewRes;

    DISABLE_OBJECT_COPY(PgResultWrapper)
};

struct PgConnWrapper
{
public:
    PgConnWrapper( PGconn * cnn, bool isNewCnn ) : _cnn(cnn), _isNewCnn(isNewCnn) { }
    ~PgConnWrapper()
    {
        this->finish();
    }
    PgConnWrapper( PgConnWrapper && other )
    {
        _cnn = other._cnn;
        _isNewCnn = other._isNewCnn;
        other._cnn = nullptr;
        other._isNewCnn = false;
    }
    PgConnWrapper & operator = ( PgConnWrapper && other )
    {
        if ( this != &other )
        {
            this->finish();

            _cnn = other._cnn;
            _isNewCnn = other._isNewCnn;
            other._cnn = nullptr;
            other._isNewCnn = false;
        }
        return *this;
    }

    bool setDbLogin(
        const char *pghost,
        const char *pgport,
        const char *pgoptions,
        const char *pgtty,
        const char *dbname,
        const char *login,
        const char *pwd
    )
    {
        this->finish();
        _cnn = PQsetdbLogin( pghost, pgport, pgoptions, pgtty, dbname, login, pwd );
        _isNewCnn = true;
        return this->status() == CONNECTION_OK;
    }

    void finish()
    {
        if ( _cnn )
        {
            if ( _isNewCnn ) PQfinish(_cnn);
            _cnn = nullptr;
            _isNewCnn = false;
        }
    }

    bool setClientEncoding( winux::String const & charset )
    {
        return PQsetClientEncoding( _cnn, charset.c_str() ) == 0;
    }

    PgResultWrapper exec( winux::String const & sql )
    {
        return PgResultWrapper( PQexec( _cnn, sql.c_str() ), true );
    }

    bool sendQuery( winux::String const & sql )
    {
        return (bool)PQsendQuery( _cnn, sql.c_str() );
    }

    bool setSingleRowMode()
    {
        return (bool)PQsetSingleRowMode(_cnn);
    }

    PgResultWrapper getResult()
    {
        return PgResultWrapper( PQgetResult(_cnn), true );
    }

    winux::String error()
    {
        return winux::CharSeqToString( PQerrorMessage(_cnn) );
    }

    ConnStatusType status()
    {
        return PQstatus(_cnn);
    }

    winux::String escapeString( winux::String const & str )
    {
        winux::Buffer buf;
        int err;
        buf.alloc( str.length() * 2 + 1 );
        size_t n = PQescapeStringConn( _cnn, buf.get<char>(), str.c_str(), str.length(), &err );
        buf._setSize(n);
        return buf.toString<winux::String::value_type>();
    }

    winux::String escapeBytea( winux::Buffer const & buf )
    {
        size_t len;
        winux::byte * p = PQescapeByteaConn( _cnn, buf.get<winux::byte>(), buf.size(), &len );
        if ( p )
        {
            winux::String res( (winux::String::value_type*)p, len );
            PQfreemem(p);
            return res;
        }
        return winux::Literal<winux::String::value_type>::nulStr;
    }

    static winux::Buffer UnescapeBytea( winux::String const & binHexStr )
    {
        size_t size = 0;
        winux::byte * p = PQunescapeBytea( (winux::byte*)binHexStr.c_str(), &size );
        if ( p )
        {
            winux::Buffer buf( p, size );
            PQfreemem(p);
            return buf;
        }
        return winux::Buffer();
    }

    winux::String escapeLiteral( winux::String const & literal )
    {
        char * p = PQescapeLiteral( _cnn, literal.c_str(), literal.length() );
        if ( p )
        {
            winux::String str = p;
            PQfreemem(p);
            return str;
        }
        return winux::Literal<winux::String::value_type>::nulStr;
    }

    winux::String escapeIdentifier( winux::String const & identifier )
    {
        char * p = PQescapeIdentifier( _cnn, identifier.c_str(), identifier.length() );
        if ( p )
        {
            winux::String str = p;
            PQfreemem(p);
            return str;
        }
        return winux::Literal<winux::String::value_type>::nulStr;
    }

    PGconn * _cnn;
    bool _isNewCnn;

    DISABLE_OBJECT_COPY(PgConnWrapper)
};


} // namespace internal


// 字符集编码串处理。因为PostgreSQL识别的字符编码串和HTML的有一定区别，在此进行转换，统一用HTML式的
static inline winux::String HtmlCharsetToPgsqlCharset( winux::String const & charset )
{
    thread_local winux::StringStringMap::value_type ps[] = {
        winux::StringStringMap::value_type( "utf-8", "utf8" ),
    };
    thread_local winux::StringStringMap m( ps, ps + countof(ps) );
    if ( m.find(charset) != m.end() )
    {
        return m[charset];
    }
    else
    {
        return charset;
    }
}


// struct PgsqlConnection_Data --------------------------------------------------------------
struct PgsqlConnection_Data
{
    internal::PgConnWrapper cnn;    // PostgreSQL连接包装
    internal::PgResultWrapper result; // PostgreSQL结果集

    PgsqlConnection_Data() : cnn( nullptr, false ), result( nullptr, false )
    {
    }
};

// class PgsqlConnection --------------------------------------------------------------------
PgsqlConnection::PgsqlConnection(
    winux::String const & host,
    winux::String const & user,
    winux::String const & pwd,
    winux::String const & dbName,
    winux::String const & linkCharset,
    winux::String const & schema,
    winux::String const & options,
    winux::Mixed * dbConfig,
    bool doConnect
)
: _affectedRows(0), _insertId(0), _dbConfig(nullptr)
{
    // 解析主机、端口
    winux::StringArray hostArr, portArr;
    for ( auto && hostStr : winux::StrSplit( host, "," ) )
    {
        size_t pos = hostStr.find(':');
        if ( pos != winux::String::npos )
        {
            hostArr.push_back( hostStr.substr( 0, pos ) );
            portArr.push_back( hostStr.substr( pos + 1 ) );
        }
        else
        {
            hostArr.push_back(host);
            portArr.push_back("");
        }
    }
    this->_host = winux::StrJoin( ",", hostArr );
    this->_port = winux::StrJoin( ",", portArr );
    this->_user = user;
    this->_pwd = pwd;
    this->_dbName = dbName;
    this->_linkCharset = winux::StrLower(linkCharset);
    this->_schema = schema;
    this->_options = options;
    this->_dbConfig = dbConfig;

    doConnect && this->connect();
}

PgsqlConnection::~PgsqlConnection()
{
    this->close();
}

bool PgsqlConnection::connect()
{
    this->close();
    bool r = _self->cnn.setDbLogin( this->_host.c_str(), this->_port.c_str(), this->_options.c_str(), nullptr, this->_dbName.c_str(), this->_user.c_str(), this->_pwd.c_str() );
    if ( !r )
    {
        winux::String err = this->error();
        int errNo = this->errNo();
        this->close();
        throw PgsqlDbError( errNo, err );
    }
    // 设置连接校验编码
    this->setLinkCharset(this->_linkCharset);
    return r;
}

bool PgsqlConnection::close()
{
    _self->cnn.finish();
    return true;
}

void PgsqlConnection::alive()
{
    if ( _self->cnn.status() != CONNECTION_OK )
    {
        this->connect();
    }
}

bool PgsqlConnection::selectDb( winux::String const & database )
{
    this->close();
    this->_dbName = database;
    return this->connect();
}

bool PgsqlConnection::setLinkCharset( winux::String const & charset )
{
    //return this->exec( "SET client_encoding TO " + charset + ";" );
    if ( charset.empty() ) return false;
    return _self->cnn.setClientEncoding(charset);
}

bool PgsqlConnection::createDb( winux::String const & database )
{
    return this->exec( "CREATE DATABASE " + this->symbolQuotes(database) + ";" );
}

bool PgsqlConnection::dropDb( winux::String const & database )
{
    return this->exec( "DROP DATABASE " + this->symbolQuotes(database) + ";" );
}

size_t PgsqlConnection::affectedRows()
{
    return this->_affectedRows;
}

size_t PgsqlConnection::insertId()
{
    return this->_insertId;
}

int PgsqlConnection::errNo()
{
    return _self->cnn.status();
}

winux::String PgsqlConnection::error()
{
    winux::String err = _self->cnn.error();
    return err.length() > 0 ? err : _self->result.error();
}

inline static bool __JudgeSingleRowMode( winux::Mixed * dbConfig )
{
    bool isSingleRow = false;
    if ( dbConfig )
    {
        int singleRowMode = dbConfig->get("single_row").toInt();
        isSingleRow = singleRowMode != 0;
        if ( singleRowMode == 2 )
        {
            (*dbConfig)["single_row"] = 0;
        }
    }
    return isSingleRow;
}

winux::SharedPointer<IDbResult> PgsqlConnection::query( winux::String const & sql )
{
    if ( _self->cnn._cnn == nullptr ) throw PgsqlDbError( this->errNo(), "PostgreSQL no valid connection" );
    return winux::MakeShared( new PgsqlResult( winux::MakeShared( new PgsqlStatement( this, sql ) ), __JudgeSingleRowMode(this->_dbConfig) ) );
}

winux::SharedPointer<IDbResult> PgsqlConnection::query( winux::SharedPointer<IDbStatement> stmt )
{
    if ( _self->cnn._cnn == nullptr ) throw PgsqlDbError( this->errNo(), "PostgreSQL no valid connection" );
    return winux::MakeShared( new PgsqlResult( stmt.ensureCast<PgsqlStatement>(), __JudgeSingleRowMode(this->_dbConfig) ) );
}

bool PgsqlConnection::exec( winux::String const & sql )
{
    _self->result = _self->cnn.exec(sql);
    ExecStatusType est = _self->result.status();
    switch ( est )
    {
    case PGRES_COMMAND_OK:
    case PGRES_TUPLES_OK:
    case PGRES_SINGLE_TUPLE:
        this->_affectedRows = winux::Mixed( PQcmdTuples(_self->result._res) );
        return true;
    default:
        return false;
    }
}

bool PgsqlConnection::exec( winux::SharedPointer<IDbStatement> stmt )
{
    PgsqlStatement * pStmt = static_cast<PgsqlStatement *>( stmt.get() );
    return this->exec( pStmt->getSql() );
}

winux::String PgsqlConnection::escape( winux::String const & str, winux::String const & addQuote )
{
    return addQuote + _self->cnn.escapeString(str) + addQuote;
}

winux::String PgsqlConnection::escape( void const * buf, size_t size, winux::String const & addQuote )
{
    return addQuote + _self->cnn.escapeBytea( winux::Buffer( buf, size, true ) ) + addQuote;
}

winux::SharedPointer<IDbResult> PgsqlConnection::listDbs()
{
    return this->query("SELECT datname FROM pg_database WHERE datistemplate = false;");
}

winux::SharedPointer<IDbResult> PgsqlConnection::listFields( winux::String const & tableName )
{
    winux::StringArray arrComp;
    winux::String tableComp, schemaComp;
    int n = winux::StrSplit( tableName, ".", &arrComp );
    int i = n - 1;
    if ( i >= 0 )
    {
        tableComp = arrComp[i];
        --i;
    }
    if ( i >= 0 )
    {
        schemaComp = arrComp[i];
        --i;
    }
    if ( schemaComp.empty() )
    {
        schemaComp = this->_schema;
    }

    return this->query(
        "SELECT column_name FROM information_schema.columns WHERE"
        " table_catalog=" + this->escape(this->_dbName) + 
        " AND table_schema=" + this->escape(schemaComp) + 
        " AND table_name=" + this->escape(tableComp) + 
        " ORDER BY ordinal_position;"
    );
}

winux::SharedPointer<IDbResult> PgsqlConnection::listTables()
{
    return this->query( "SELECT * FROM pg_tables WHERE tableowner=" + this->escape(this->_dbName) + " AND schemaname=" + this->escape(this->_schema) + ";" );
}

winux::String PgsqlConnection::tableDdl( winux::String const & tableName )
{
    winux::StringArray arrComp;
    winux::String tableComp, schemaComp;
    int n = winux::StrSplit( tableName, ".", &arrComp );
    int i = n - 1;
    if ( i >= 0 )
    {
        tableComp = arrComp[i];
        --i;
    }
    if ( i >= 0 )
    {
        schemaComp = arrComp[i];
        --i;
    }
    if ( schemaComp.empty() )
    {
        schemaComp = this->_schema;
    }

    winux::String ddl = "CREATE TABLE " + this->symbolQuotes(schemaComp) + "." + this->symbolQuotes(tableComp) + " (\n";

    // 查询列
    winux::String sqlCols = R"(SELECT
    col.ordinal_position,
    col.column_name,
    att.atttypid,
    col.udt_name,
    typ.typcategory,
    col.character_maximum_length,
    att.attcollation,
    coll.collname,
    att.attnotnull,
    att.atthasdef,
    col.column_default
FROM information_schema.columns col
	LEFT JOIN pg_attribute att ON att.attrelid=(col.table_catalog || '.' || col.table_schema || '.' || col.table_name)::regclass and att.attname=col.column_name
	LEFT JOIN pg_type typ ON typ.oid=att.atttypid
	LEFT JOIN pg_collation coll ON coll.oid=att.attcollation
WHERE table_catalog=? AND table_schema=? AND table_name=?
ORDER BY ordinal_position;)";

    PgsqlResult resCols( winux::MakeShared( new PgsqlStatement( this, sqlCols, { this->_dbName, schemaComp, tableComp } ) ) );
    winux::Mixed row;
    bool nofirst = false;
    while ( resCols.fetchRow(&row) )
    {
        //winux::ColorOutputLine( 14, row.myJson() );
        if ( nofirst ) ddl += ",\n";
        nofirst = true;

        // 列名
        ddl += "  " + this->symbolQuotes(row["column_name"]);

        // 类型
        winux::String typeName, defVal;
        int typeId = row["atttypid"].to<int>();
        typeName = row["udt_name"].to<winux::String>();
        defVal = row["column_default"].to<winux::String>();
        // int8=20 int2=21 int4=23
        std::map< int, winux::String > serialType = {
            { INT8OID, "serial8" },
            { INT4OID, "serial4" },
            { INT2OID, "serial2" }
        };
        switch ( typeId )
        {
        case INT8OID: case INT4OID: case INT2OID:
            if ( defVal.find("nextval(") == 0 )
            {
                defVal = "";
                typeName = serialType[typeId];
            }
            break;
        }
        ddl += " " + typeName;
        // 长度限制
        winux::Mixed & maxlen = row["character_maximum_length"];
        if ( !maxlen.isNull() && maxlen.toInt() > 0 )
        {
            ddl += winux::Format( "(%d)", maxlen.toInt() );
        }

        // 排序规则
        if ( row["attcollation"].toInt() > 0 )
        {
            ddl += " COLLATE " + this->symbolQuotes(row["collname"]);
        }

        // not null
        if ( row["attnotnull"].toBool() )
        {
            ddl += " NOT NULL";
        }

        // 默认值
        if ( !defVal.empty() )
        {
            ddl += " DEFAULT " + defVal;
        }
    }

    // 查询约束
    winux::String sqlCons = R"(SELECT
    con.oid,
    con.conindid,
    con.contype,
    con.conkey,
    ictu.*,
    pg_get_constraintdef(con.oid) as condef
FROM information_schema.constraint_table_usage ictu
LEFT JOIN pg_catalog.pg_constraint con ON con.conname=ictu.constraint_name AND con.conrelid=(ictu.table_catalog || '.' || ictu.table_schema || '.' || ictu.table_name)::regclass
WHERE
    table_catalog=? AND table_schema=? AND table_name=?
ORDER BY con.contype;)";
    PgsqlResult resCons( winux::MakeShared( new PgsqlStatement( this, sqlCons, { this->_dbName, schemaComp, tableComp } ) ) );
    while ( resCons.fetchRow(&row) )
    {
        //winux::ColorOutputLine( 14, row.myJson() );
        if ( nofirst ) ddl += ",\n";
        nofirst = true;
        ddl += "  CONSTRAINT " + this->symbolQuotes(row["constraint_name"]) + " " + row["condef"].toAnsi();
    }

    ddl += "\n);\n";

    // 查询索引
    winux::String sqlIdxes = R"(SELECT
    ns.nspname AS schemaname,
    tc.relname AS tablename,
    ic.relname AS indexname,
    ts.spcname AS tablespace,
    pg_get_indexdef(ic.oid) AS indexdef
FROM pg_index x
    JOIN pg_class tc ON tc.oid = x.indrelid
    JOIN pg_class ic ON ic.oid = x.indexrelid
    LEFT JOIN pg_namespace ns ON ns.oid = tc.relnamespace
    LEFT JOIN pg_tablespace ts ON ts.oid = ic.reltablespace
WHERE
    (tc.relkind = ANY(ARRAY['r'::"char", 'm'::"char", 'p'::"char"]))
    AND (ic.relkind = ANY(ARRAY['i'::"char", 'I'::"char"]))
    AND x.indisprimary=false
    AND x.indrelid=?::regclass
;)";

    PgsqlResult resIdxes( winux::MakeShared( new PgsqlStatement( this, sqlIdxes, this->symbolQuotes( winux::StrJoin( ".", { this->_dbName, schemaComp, tableComp } ) ) ) ) );
    while ( resIdxes.fetchRow(&row) )
    {
        //winux::ColorOutputLine( 15, row.myJson() );
        ddl += row["indexdef"].toAnsi() + ";\n";
    }
    return ddl;
}

winux::String PgsqlConnection::symbolQuotes( winux::String const & str, bool periodAsSeparator )
{
    if ( periodAsSeparator )
    {
        winux::StringArray arr;
        winux::String res;
        winux::StrSplit( str, ".", &arr );
        for ( size_t i = 0; i < arr.size(); ++i )
        {
            if ( i != 0 ) res += ".";
            res += _self->cnn.escapeIdentifier(arr[i]);
        }
        return res;
    }
    else
    {
        return _self->cnn.escapeIdentifier(str);
    }
}

size_t PgsqlConnection::loadSqlFile( winux::IFile * sqlFile, winux::StringArray * arrSql )
{
    return internal::__PgsqlLoadSqlFile( sqlFile, arrSql, this );
}

size_t PgsqlConnection::getPrimaryKey( winux::String const & tableName, winux::StringArray * arrKeyColumn )
{
    winux::StringArray arrComp;
    winux::String tableComp, schemaComp;
    int n = winux::StrSplit( tableName, ".", &arrComp );
    int i = n - 1;
    if ( i >= 0 )
    {
        tableComp = arrComp[i];
        --i;
    }
    if ( i >= 0 )
    {
        schemaComp = arrComp[i];
        --i;
    }
    if ( schemaComp.empty() )
    {
        schemaComp = this->_schema;
    }

    auto rs = this->query(
        "SELECT ikcu.column_name FROM information_schema.key_column_usage ikcu"
        " LEFT JOIN pg_catalog.pg_constraint con ON con.conname=ikcu.constraint_name AND con.conrelid=(ikcu.table_catalog || '.' || ikcu.table_schema || '.' || ikcu.table_name)::regclass"
        " WHERE table_catalog=" + this->escape(this->_dbName) + 
        " AND table_schema=" + this->escape(schemaComp) + 
        " AND table_name=" + this->escape(tableComp) + 
        " AND con.contype='p' ORDER BY ordinal_position;"
    );
    arrKeyColumn->clear();
    winux::MixedArray f;
    while ( rs->fetchArray(&f) )
    {
        arrKeyColumn->push_back(f[0]);
    }
    return arrKeyColumn->size();
}

winux::SharedPointer<IDbStatement> PgsqlConnection::buildStmt( winux::String const & sql, winux::Mixed const & params )
{
    return winux::SharedPointer<IDbStatement>( new PgsqlStatement( this, sql, params ) );
}

PgsqlConnection::operator bool () const
{
    return _self->cnn._cnn != nullptr;
}

PgsqlConnection::operator DbHandle() const
{
    return _self->cnn._cnn;
}

// class PgsqlStatement ---------------------------------------------------------------------

// struct PgsqlResult_Data ------------------------------------------------------------------
struct PgsqlResult_Data
{
    internal::PgResultWrapper result; // 结果集包装

    PgsqlResult_Data() : result( nullptr, false )
    {
    }
};

// class PgsqlResult ------------------------------------------------------------------------
PgsqlResult::PgsqlResult( winux::SharedPointer<PgsqlStatement> stmt, bool singleRow ) : _curRowIndex(-1), _rowsCount(0), _singleRow(false), _hadFetch(true)
{
    this->_stmt = stmt;
    if ( !this->_stmt || !*this->_stmt.get() ) throw PgsqlDbError( 0, "Invalid statement object" );

    this->_singleRow = singleRow;
    if ( this->_singleRow ) // 单行模式
    {
        if ( this->_stmt->getCnn()->_self->cnn.sendQuery( this->_stmt->getSql() ) )
        {
            this->_stmt->getCnn()->_self->cnn.setSingleRowMode();
            this->_hadFetch = false; // 未完成fetch
        }
        else
        {
            throw PgsqlDbError( this->_stmt->getCnn()->errNo(), this->_stmt->getCnn()->error() );
        }
    }
    else // 多行模式
    {
        _self->result = this->_stmt->getCnn()->_self->cnn.exec( this->_stmt->getSql() );
        ExecStatusType est = _self->result.status();
        switch ( est )
        {
        case PGRES_TUPLES_OK:
            // 受影响行数
            this->_stmt->getCnn()->_affectedRows = winux::Mixed( PQcmdTuples(_self->result._res) );

            this->_curRowIndex = 0;
            this->_rowsCount = (size_t)_self->result.ntuples();
            break;
        case PGRES_FATAL_ERROR:
            throw PgsqlDbError( est, _self->result.error() );
            break;
        }
    }
}

PgsqlResult::~PgsqlResult()
{
    this->free();
}

bool PgsqlResult::free()
{
    // 单行模式下，未提取全部数据需要全部提取，否则不能开启新查询
    if ( this->_singleRow && this->_hadFetch == false )
    {
        do
        {
            _self->result = this->_stmt->getCnn()->_self->cnn.getResult();
        }
        while ( _self->result._res != nullptr );
        this->_hadFetch = true;
    }
    _self->result.clear();
    return true;
}

bool PgsqlResult::dataSeek( size_t index )
{
    if ( index >= this->_rowsCount ) return false;
    this->_curRowIndex = index;
    return true;
}

inline static void __FieldValueAssignToMixed( winux::Mixed & m, Oid type, internal::DataFormat df, char const * data, size_t len )
{
    if ( !data ) // is null
    {
        m = winux::mxNull;
        return;
    }

    switch ( type )
    {
    case BOOLOID:
        {
            bool boolVal;
            winux::ParseBool( winux::AnsiString( data, len ), &boolVal );
            m.assign(boolVal);
        }
        break;
    case INT2OID:
        {
            long lVal = 0;
            winux::ParseULong( winux::AnsiString( data, len ), (winux::ulong*)&lVal );
            m.assign((short)lVal);
        }
        break;
    case INT4OID:
        {
            long lVal = 0;
            winux::ParseULong( winux::AnsiString( data, len ), (winux::ulong*)&lVal );
            m.assign(lVal);
        }
        break;
    case OIDOID:
        {
            winux::ulong ulVal = 0;
            winux::ParseULong( winux::AnsiString( data, len ), (winux::ulong*)&ulVal );
            m.assign(ulVal);
        }
        break;
    case INT8OID:
        {
            winux::int64 i64Val = 0;
            winux::ParseUInt64( winux::AnsiString( data, len ), (winux::uint64*)&i64Val );
            m.assign(i64Val);
        }
        break;
    case FLOAT4OID:
        {
            float fltVal = 0;
            winux::ParseFloat( winux::AnsiString( data, len ), &fltVal );
            m.assign(fltVal);
        }
        break;
    case FLOAT8OID:
        {
            double dblVal = 0;
            winux::ParseDouble( winux::AnsiString( data, len ), &dblVal );
            m.assign(dblVal);
        }
        break;
    case BYTEAOID:
        {
            switch ( df )
            {
            case internal::dataTextual:
                m.assign( internal::PgConnWrapper::UnescapeBytea( winux::String( data, len ) ) );
                break;
            case internal::dataBinary:
                m.assign( data, len, false );
                break;
            }
        }
        break;
    case CHAROID:
        m.assign( data, len );
        break;
    case BPCHAROID:
        m = winux::Mixed( winux::StrRTrim( winux::String( data, len ) ) );
        break;
    case NAMEOID:
    case VARCHAROID:
    case TEXTOID:
    default:
        m.assign( data, len );
        break;
    }
}

inline static void __FieldsAssignArray( winux::MixedArray * fields, int type, internal::PgResultWrapper & result, size_t rowIndex )
{
    if ( fields != nullptr )
    {
        int columnCount = result.nfields();
        fields->resize(columnCount);
        for ( int i = 0; i < columnCount; ++i )
        {
            __FieldValueAssignToMixed(
                (*fields)[i],
                result.ftype(i),
                result.fformat(i),
                !result.getisnull( rowIndex, i ) ? result.getvalue( rowIndex, i ) : nullptr,
                (size_t)result.getlength( rowIndex, i )
            );
        }
    }
}

inline static void __FieldsAssignMap( winux::StringMixedMap * fields, int type, internal::PgResultWrapper & result, size_t rowIndex )
{
    if ( fields != nullptr )
    {
        fields->clear();
        int columnCount = result.nfields();
        for ( int i = 0; i < columnCount; ++i )
        {
            __FieldValueAssignToMixed(
                (*fields)[ result.fname(i) ],
                result.ftype(i),
                result.fformat(i),
                !result.getisnull( rowIndex, i ) ? result.getvalue( rowIndex, i ) : nullptr,
                (size_t)result.getlength( rowIndex, i )
            );
        }
    }
}

inline static void __FieldsAssignMixed( winux::Mixed * fields, int type, internal::PgResultWrapper & result, size_t rowIndex )
{
    if ( fields != nullptr )
    {
        if ( type == 0 ) // 映射表
        {
            int columnCount = result.nfields();
            fields->createCollection();
            for ( int i = 0; i < columnCount; ++i )
            {
                __FieldValueAssignToMixed(
                    (*fields)[ result.fname(i) ],
                    result.ftype(i),
                    result.fformat(i),
                    !result.getisnull( rowIndex, i ) ? result.getvalue( rowIndex, i ) : nullptr,
                    (size_t)result.getlength( rowIndex, i )
                );
            }
        }
        else if ( type == 1 ) // 数组
        {
            int columnCount = result.nfields();
            fields->createArray(columnCount);
            for ( int i = 0; i < columnCount; ++i )
            {
                __FieldValueAssignToMixed(
                    (*fields)[i],
                    result.ftype(i),
                    result.fformat(i),
                    !result.getisnull( rowIndex, i ) ? result.getvalue( rowIndex, i ) : nullptr,
                    (size_t)result.getlength( rowIndex, i )
                );
            }
        }
    }
}

template < typename _FieldsType >
inline static bool __Fetch(
    PgsqlResult_Data * resSelf,
    bool singleRow,
    bool * hadFetch,
    size_t * curRowIndex,
    size_t rowsCount,
    PgsqlConnection_Data * cnnSelf,
    size_t * affectedRows,
    _FieldsType * fields,
    int type,
    void (*fx)( _FieldsType * fields, int type, internal::PgResultWrapper & result, size_t rowIndex )
)
{
    if ( singleRow ) // 单行模式
    {
        resSelf->result = cnnSelf->cnn.getResult();
        ExecStatusType est = resSelf->result.status();
        switch ( est )
        {
        case PGRES_SINGLE_TUPLE:
            {
                // 受影响行数
                *affectedRows = winux::Mixed( PQcmdTuples(resSelf->result._res) );
                fx( fields, type, resSelf->result, 0 );
                return true;
            }
            break;
        case PGRES_TUPLES_OK:
            {
                // 受影响行数
                *affectedRows = winux::Mixed( PQcmdTuples(resSelf->result._res) );
                // 继续获取一个PGresult*(nullptr)
                resSelf->result = cnnSelf->cnn.getResult();
                *hadFetch = true;
                return false;
            }
            break;
        }
    }
    else // 多行模式
    {
        ExecStatusType est = resSelf->result.status();
        switch ( est )
        {
        case PGRES_TUPLES_OK:
            {
                if ( *curRowIndex >= rowsCount ) return false;
                fx( fields, type, resSelf->result, *curRowIndex );
                (*curRowIndex)++;
                return true;
            }
            break;
        }
    }
    return false;
}

bool PgsqlResult::fetchRow( winux::Mixed * fields, int type )
{
    auto * cnn = this->_stmt->getCnn();
    return __Fetch( _self.get(), this->_singleRow, &this->_hadFetch, &this->_curRowIndex, this->_rowsCount, cnn->_self.get(), &cnn->_affectedRows, fields, type, __FieldsAssignMixed );
}

bool PgsqlResult::fetchArray( winux::MixedArray * fields )
{
    auto * cnn = this->_stmt->getCnn();
    return __Fetch( _self.get(), this->_singleRow, &this->_hadFetch, &this->_curRowIndex, this->_rowsCount, cnn->_self.get(), &cnn->_affectedRows, fields, 1, __FieldsAssignArray );
}

bool PgsqlResult::fetchMap( winux::StringMixedMap * fields )
{
    auto * cnn = this->_stmt->getCnn();
    return __Fetch( _self.get(), this->_singleRow, &this->_hadFetch, &this->_curRowIndex, this->_rowsCount, cnn->_self.get(), &cnn->_affectedRows, fields, 0, __FieldsAssignMap );
}

winux::String PgsqlResult::fieldName( size_t fieldIndex )
{
    return _self->result.fname((int)fieldIndex);
}

size_t PgsqlResult::fieldsCount()
{
    return _self->result.nfields();
}

size_t PgsqlResult::rowsCount()
{
    return _self->result.ntuples();
}

winux::String PgsqlResult::fieldType( size_t fieldIndex )
{
    return internal::__TypeName( _self->result.ftype(fieldIndex) );
}

bool PgsqlResult::reset()
{
    if ( this->_singleRow ) // 单行模式
    {
        if ( this->_stmt->getCnn()->_self->cnn.sendQuery( this->_stmt->getSql() ) )
        {
            this->_stmt->getCnn()->_self->cnn.setSingleRowMode();
            return true;
        }
        else
        {
            throw PgsqlDbError( this->_stmt->getCnn()->errNo(), this->_stmt->getCnn()->error() );
        }
    }
    else // 多行模式
    {
        return this->dataSeek(0);
    }
}

PgsqlResult::operator bool() const
{
    return _self->result._res != nullptr;
}

// class PgsqlModifier ----------------------------------------------------------------------
PgsqlModifier::PgsqlModifier( PgsqlConnection * cnn, winux::String const & tableName ) : DbModifier( cnn, tableName )
{
    this->_getTableInfo();
}

PgsqlModifier::~PgsqlModifier()
{

}

bool PgsqlModifier::_execInsertInto( winux::SharedPointer<IDbStatement> stmt )
{
    bool r = DbModifier::_execInsertInto(stmt);

    auto * cnn = static_cast<PgsqlConnection *>(this->_cnn);
    cnn->_insertId = 0;
    if ( r )
    {
        winux::StringArray arrComp;
        winux::String tableComp, schemaComp;
        int n = winux::StrSplit( this->_tableName, ".", &arrComp );
        int i = n - 1;
        if ( i >= 0 )
        {
            tableComp = arrComp[i];
            --i;
        }
        if ( i >= 0 )
        {
            schemaComp = arrComp[i];
            --i;
        }
        if ( schemaComp.empty() )
        {
            schemaComp = cnn->_schema;
        }

        winux::String sql = "SELECT pg_sequence_last_value(pg_get_serial_sequence(?, column_name)::regclass) FROM information_schema.columns WHERE table_catalog=? AND table_schema=? AND table_name=? AND column_default LIKE 'nextval%';";
        winux::String fullName = _cnn->symbolQuotes( cnn->_dbName, false ) + "." + _cnn->symbolQuotes( schemaComp, false ) + "." + _cnn->symbolQuotes( tableComp, false );
        PgsqlResult res( _cnn->buildStmt( sql, { fullName, cnn->_dbName, schemaComp, tableComp } ).ensureCast<PgsqlStatement>() );
        winux::MixedArray arr;
        if ( res.fetchArray(&arr) )
        {
            cnn->_insertId = arr[0];
        }
    }
    return r;
}

#endif // HAVE_DB_PGSQL

} // namespace eiendb
