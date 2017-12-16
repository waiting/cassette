/* 元素包 - 表示一套标签语言体系的相关要素 */
#ifndef __ELEMPACKAGE_HPP__
#define __ELEMPACKAGE_HPP__

namespace eienml
{

typedef Element * ( * ElementCreator )( void );
typedef Document * ( * DocumentCreator )( void );
typedef TextNode * ( * TextNodeCreator )( void );
typedef Block * ( * BlockCreator )( void );

typedef std::map< winux::AnsiString, ElementCreator > ElementCreatorTable;
typedef std::map< winux::AnsiString, BlockCreator > BlockCreatorTable;

/** \brief 元素包
 *
 *  描述一套标记语言的相关信息 */
class EIENML_DLL ElemPackage
{
public:
    static const char * ADDSLASHES/* = "\\\"\'" */; ///< 要加反斜杠的字符
    /** \brief 元素包构造函数
     *
     *  调用:setDelims(),setEntitiesTable(),setTextCreator(),setElement(),setBlock()完成元素包的初始化工作 */
    ElemPackage( void );

    /** \brief 对字符进行换码
     *
     *  默认：@(at), left_elem_delim(le), right_elem_delim(re)
     *  left_block_delim(lb), right_block_delim(rb) */
    winux::AnsiString escape( winux::AnsiString const & str ) const;
    /** \brief 进行反换码，与escape()配套 */
    winux::AnsiString unescape( winux::AnsiString const & str ) const;
    /** \brief 特殊字符加上反斜杠 */
    winux::AnsiString addSlashes( winux::AnsiString const & str ) const;
    /** \brief 去掉反斜杠 */
    winux::AnsiString stripSlashes( winux::AnsiString const & str ) const;

    /** \brief 设置实体表,entitiesTable为{文本=>实体}的映射表 */
    void setEntitiesTable(
        winux::AnsiString const & specialChar,
        winux::AnsiString const & specialCharEntity,
        winux::AnsiString const & lElemDelimEntity,
        winux::AnsiString const & rElemDelimEntity,
        winux::AnsiString const & lBlockDelimEntity,
        winux::AnsiString const & rBlockDelimEntity,
        winux::StringStringMap const * entitiesTable = NULL
    );
    /** \brief 设置定界符,调用后应立即调用setEntitiesTable()更新实体 */
    void setDelims( winux::AnsiString const & le, winux::AnsiString const & re, winux::AnsiString const & lb = "", winux::AnsiString const & rb = "" );
    /** \brief 设置文本节点创建器 */
    TextNodeCreator setTextCreator( TextNodeCreator creator );

    /** \brief 设置元素节点创建器 */
    void setElement( winux::AnsiString const & tagName, ElementCreator creator );
    /** \brief 移除元素节点创建器 */
    void delElement( winux::AnsiString const & tagName );
    /** \brief 设置块节点创建器 */
    void setBlock( winux::AnsiString const & blockTag, BlockCreator creator );
    /** \brief 移除块节点创建器 */
    void delBlock( winux::AnsiString const & blockTag );
    /** \brief 通过已指定的元素节点创建器创建元素，如果不存在该创建器，则使用默认Element创建器。 */
    Element * createElement( winux::AnsiString const & tagName ) const;
    /** \brief 通过已指定的块节点创建器创建块，如果不存在该创建器，则使用默认Block创建器。 */
    Block * createBlock( winux::AnsiString const & blockTag ) const;
    /** \brief 通过已指定的文本节点创建器文本节点，如果不存在该创建器，则使用默认TextNode创建器。 */
    TextNode * createTextNode( winux::AnsiString const & str ) const;

    /** \brief 指定的元素创建器是否存在 */
    bool exists( winux::AnsiString const & tagName ) const;
    /** \brief 指定的块创建器是否存在 */
    bool blockExists( winux::AnsiString const & blockTag ) const;
public:
    winux::AnsiString lElemDelim; ///< 左定界符
    winux::AnsiString rElemDelim; ///< 右定界符
    winux::AnsiString lBlockDelim; ///< 左块元素定界符
    winux::AnsiString rBlockDelim; ///< 右块元素定界符
protected:
    TextNodeCreator textCreator; ///< 文本节点创建函数
    ElementCreatorTable elemTable; ///< 元素名=>创建函数映射表
    BlockCreatorTable blockTable; ///< 块名=>创建块的函数映射表
    winux::MultiMatch entities2text; ///< 实体到文本
    winux::MultiMatch text2entities; ///< 文本到实体
};

}


#endif // __ELEMPACKAGE_HPP__
