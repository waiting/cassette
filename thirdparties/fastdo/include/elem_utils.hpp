#ifndef __EIEN__EIENML__ELEMENTUTILS_HPP__
#define __EIEN__EIENML__ELEMENTUTILS_HPP__

/** \brief 标签式语言支持库
 *
 *  这是能够创建一门新标签式语言的库，几乎所有的类都需要你派生出新类并自定义它们 */
namespace eienml
{

/** \brief 节点类型 */
enum NodeType
{
    NodeType_Text,  ///< 文本
    NodeType_Block, ///< 块
    NodeType_Elem,  ///< 元素
    NodeType_Doc,   ///< 文档
};

/** \brief 元素处理种类 */
enum ProcType
{
    ProcType_Raw,     ///< 不处理，原始内容
    ProcType_Simple,  ///< 简单处理，输出符合规则的内容
    ProcType_Tree,    ///< 展现树形结构
    ProcType_Strip,   ///< 去掉元素tag，输出非元素部分
    ProcType_Replace, ///< 替换处理，输出转换后的内容
};
// 相关宏 ----------------------------------------------------
/* 给指针指的空间赋值 */
#define AssignToPointer( ptr, val ) if ( ptr ) *( ptr ) = ( val )

#define DEFINE_NEWDOC_FUNC( cls ) DEFINE_NEWOBJ_FUNC( cls, eienml::Document )
#define DEFINE_NEWBLOCK_FUNC( cls ) DEFINE_NEWOBJ_FUNC( cls, eienml::Block )
#define DEFINE_NEWELEM_FUNC( cls ) DEFINE_NEWOBJ_FUNC( cls, eienml::Element )
#define DEFINE_NEWTEXT_FUNC( cls ) DEFINE_NEWOBJ_FUNC( cls, eienml::TextNode )
#define DEFINE_NEWNODE_FUNC( cls ) DEFINE_NEWOBJ_FUNC( cls, eienml::Node )
#define DEFINE_NEWOBJ_FUNC( cls, ret )\
inline static ret * NewInstance( void )\
{\
    return new cls();\
}
// 指针转化
#define ClassObj( cls, p ) ( static_cast<cls *>( p ) )
#define TextNodeObj( p ) ClassObj( eienml::TextNode, ( p ) )
#define BlockObj( p ) ClassObj( eienml::Block, ( p ) )
#define ElementObj( p ) ClassObj( eienml::Element, ( p ) )
#define DocumentObj( p ) ClassObj( eienml::Document, ( p ) )
// 相关类 ----------------------------------------------------
class ElemPackage;
class Element;
class Document;

/** \brief 节点基类 */
class EIENML_DLL Node
{
public:
    DEFINE_NEWNODE_FUNC(Node)
    void delInstance( void );
    virtual winux::AnsiString asStr( int procType = ProcType_Raw );
    virtual void doWork( void );
    // 属性
    ElemPackage const * getPackage( void ) const;
    void setPackage( ElemPackage const * package );
    Element * getParent( void ) const;
    void setParent( Element * element );
    Document * getDocument( void ) const;
    NodeType getType( void ) const;
protected:
    Node( void );
    virtual ~Node( void );
    ElemPackage const * package; // 指向元素包
    Element * parent; // 父节点
    NodeType type;
};

/** \brief 文本节点类 */
class EIENML_DLL TextNode : public Node
{
public:
    DEFINE_NEWTEXT_FUNC(TextNode)
    virtual winux::AnsiString asStr( int procType = ProcType_Raw );
    virtual void doWork( void );
    winux::AnsiString getValue( void ) const;
    void setValue( winux::AnsiString const & value );
protected:
    TextNode( void );
    virtual ~TextNode( void );
    winux::AnsiString value;
public:
    winux::AnsiString rawTextNode;
};

/** \brief 块节点类 */
class EIENML_DLL Block : public Node
{
public:
    DEFINE_NEWBLOCK_FUNC(Block)
    virtual winux::AnsiString asStr( int procType = ProcType_Raw );
protected:
    Block( void );
    ~Block( void );
public:
    winux::AnsiString blockTag; ///< 块元素标记
    winux::AnsiString blockContent; ///< 块元素内部包含的内容
    winux::AnsiString rawBlock; ///< 块元素(原始)文本
};

typedef std::map<winux::AnsiString, winux::AnsiString> Attributes;
typedef std::vector<Node *> NodeArray;
typedef std::vector<Block *> BlockArray;
typedef std::vector<Element *> ElementArray;

/** \brief 元素节点类 */
class EIENML_DLL Element : public Node
{
public:
    DEFINE_NEWELEM_FUNC(Element)
    virtual winux::AnsiString childAsStr( int procType = ProcType_Raw );
    virtual void doChildWork( void );
    /** \brief 向最后一个文本节点加字符串,如果没有文本节点,则添加文本节点.
     *
     *  如果成功有TextNode新添加进node tree中,则返回true.
     *  返回false则表示文本添加进了最后的文本节点,p将返回文本被加入到的那个TextNode上 */
    bool addTextToLast( winux::AnsiString const & str, TextNode * * p = NULL );
    bool addChild( Node * element );
    bool hasChildren( void );
    void getElements( ElementArray * arr, winux::AnsiString const & tagName );
    void getBlocks( BlockArray * arr, winux::AnsiString const & blockTag );
    virtual winux::AnsiString asStr( int procType = ProcType_Raw );
    virtual void doWork( void );
    bool hasAttr( winux::AnsiString const & name ) const;
    winux::AnsiString getAttr( winux::AnsiString const & name ) const;
    void setAttr( winux::AnsiString const & name, winux::AnsiString const & value );
    void delAttr( winux::AnsiString const & name );

    NodeArray & getChildren( void );
    Attributes & getAttributes( void );
    void setAttributes( Attributes const & attrs );
    bool isOdd( void ) const;
protected:
    Element( void );
    virtual ~Element( void );
    virtual winux::AnsiString replaceProc( void );
    virtual winux::AnsiString treeProc( void );

    Attributes attrs; ///< 属性列表
    bool odd;   ///< 是否为单独元素（没有`尾`）
    NodeArray children; ///< 子节点列表
public:
    winux::AnsiString raw1; ///< 未解析的原始`头`文本
    winux::AnsiString raw2; ///< 未解析的原始`尾`文本
    winux::AnsiString tagName; ///< Tag名称
    winux::AnsiString defAttr; ///< 默认属性
};

/** \brief 单独元素类（没有`尾`） */
class EIENML_DLL OddElement : public Element
{
public:
    DEFINE_NEWELEM_FUNC(OddElement)
    OddElement( void )
    {
        this->odd = true;
    }
};

/** \brief 文档类 */
class EIENML_DLL Document : public Element
{
public:
    DEFINE_NEWDOC_FUNC(Document)
    virtual winux::AnsiString asStr( int procType = ProcType_Raw );
    virtual void doWork( void );
protected:
    Document( void );
    virtual ~Document( void );
};

}


#endif // __EIEN__EIENML__ELEMENTUTILS_HPP__
