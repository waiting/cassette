#ifndef __ELEMPARSER_HPP__
#define __ELEMPARSER_HPP__

namespace eienml
{

/** \brief 元素解析处理器
 *
 *  存在两个接口，一个不考虑块解析V1 parse()，一个考虑块解析V2 parseEx() */
class EIENML_DLL ElemParser
{
public:
    ElemParser( ElemPackage const & package );

    /** \brief V2 解析成元素 */
    Element * parseEx( winux::AnsiString s, Element * container ) const;

    /** \brief V2 解析成文档 */
    Document * parseEx( winux::AnsiString s, DocumentCreator docCreator = NULL ) const;

    /** \brief V2 寻找一对边界符内容，并返回边界符种类，和未经解析的内部数据
     *
     * \param *length 返回搜到的长度，包括边界符
     * \param *internalData 返回内部数据，即去掉边界符的内容
     * \param lDelim,rDelim 返回定界符 */
    int findDelimPair(
        winux::AnsiString const & str,
        int offset,
        int * length,
        winux::AnsiString * internalData,
        winux::AnsiString * lDelim,
        winux::AnsiString * rDelim
    ) const;

    /** \brief V2 搜一个在标签包中存在的标签，并返回标签种类和未经解析的内部数据
     *
     * \param *length 返回搜到的长度，包括边界符
     * \param *data 返回内部数据，即去掉多余的内容，例如'/'
     * \param *isBlock 指示是否为块标签
     * \param *isHead 指示搜到的标签是否是一个开头
     * \param *tagName 当*isBlock==false时，返回元素名 */
    int search( winux::AnsiString const & str, int * length, winux::AnsiString * data, bool * isBlock, bool * isHead, winux::AnsiString * tagName ) const;

    /** \brief V2 读一个元素标签属性数据 */
    void readElement( winux::AnsiString data, winux::AnsiString * tagName, winux::AnsiString * defAttr, Attributes * attrs ) const;

    /** \brief V2 读一个块标签数据 */
    void readBlock( winux::AnsiString const & data, winux::AnsiString * blockTag, winux::AnsiString * blockContent ) const;
private:

public:
    /** \brief V1 解析成元素
     *
     *  搜索一个[...], 把前面的字符当作文本元素存入, 遇到一个[...], 如果是头,则新建元素,解析属性 */
    Element * parse( winux::AnsiString s, Element * container ) const;

    /** \brief V1 解析成文档 */
    Document * parse( winux::AnsiString s, DocumentCreator docCreator = NULL ) const;

    /** \brief V1 搜一个支持的element */
    int searchElement( winux::AnsiString const & str, int * length = NULL ) const;

    /** \brief V1 搜索 [...] 这个串, 返回其开始位置,否则返回-1, length表示搜到的长度 */
    int searchDelim( winux::AnsiString const & str, int offset, int * length = NULL ) const;

    /** \brief V1 判断一个[...] 是头还是尾, data返回去掉界定符后的内容 */
    bool isHeader( winux::AnsiString const & str, winux::AnsiString * data = NULL ) const;
    /* 读取字符到键名,遇到=号键名结束,开始读值
     * 如果第一个字符不是引号,则读到空格为止值结束
     * 如果是引号,则继续读到另一个引号为止值结束
     * 读到\时应对下一字符进行判断,如果是引号,则不结束值,继续读取. */

    /** \brief V1 读取一个[...]内的数据 */
    void read( winux::AnsiString const & str, winux::AnsiString * tagName = NULL, winux::AnsiString * defAttr = NULL, Attributes * attrs = NULL ) const;
private:
    void _key( winux::AnsiString const & str, int len, int start, winux::AnsiString * key, int * pos ) const;
    void _val( winux::AnsiString const & str, int len, int start, winux::AnsiString * val, int * pos ) const;
    ElemPackage const * package;

};

}

#endif // __ELEMPARSER_HPP__
