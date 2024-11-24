// 错误Format字符串

// `%s`不是一个变量
#define EXPRERRSTR_NOT_VARIABLE(s) winux::FormatA( "`%s` is not a variable", (s) )
// `%s`计算失败了
#define EXPRERRSTR_EVALUATE_FAILED(s) winux::FormatA( "`%s` evaluate failed", (s) )
// `%s`不是一个可索引操作的对象
#define EXPRERRSTR_NOT_INDEXABLE_OBJECT(s) winux::FormatA( "`%s` is not a indexable object", (s) )
// 未关联变量场景对象
#define EXPRERRSTR_NOT_ASSOC_VARCTX(s) winux::FormatA( "Expression `%s` is not associated a VarContext object", (s) )
// `%s`标识符未定义
#define EXPRERRSTR_IDENTIFIER_NOT_DEFINED(s) winux::FormatA( "The `%s` identifier is not defined", (s) )
// `%s()`函数未定义
#define EXPRERRSTR_FUNCTION_NOT_DEFINED(s) winux::FormatA( "The `%s()` function is not defined", (s) )
// `%s()`参数不足
#define EXPRERRSTR_NOT_ENOUGH_PARAMETERS(s) winux::FormatA( "`%s()` not enough parameters", (s) )

// `%s`操作数出现在错误位置:%d, 不可能连续两个操作数
#define EXPRERRSTR_OPERAND_WRONG_PLACE_IMPOSSIBLE_CONTINUOUS_OPERAND( s, i ) winux::FormatA( "The `%s` operand to be in the wrong place:%d, it's impossible what continuous two operand", (s), (int)(i) )
// `%s`操作数出现在错误位置:%d, 单目左结合算符右边不可能是操作数
#define EXPRERRSTR_OPERAND_WRONG_PLACE_IMPOSSIBLE_OPERAND_ATRIGHTOF_UNARYLEFTASSOC_OPR( s, i ) winux::FormatA( "The `%s` operand to be in the wrong place:%d, it's impossible what an operand at right of unary left-assoc operator", (s), (int)(i) )
// `%s`操作符出现在错误位置:%d
#define EXPRERRSTR_OPERATOR_WRONG_PLACE( s, i ) winux::FormatA( "The `%s` operator to be in the wrong place:%d", (s), (int)(i) )
// 在`%s`中，操作符`%s`出现在错误位置
#define EXPRERRSTR_OPERATOR_WRONG_PLACE_IN( s, s1 ) winux::FormatA( "`%s()`: `%s` appears in the wrong place", (s), (s1) )
// 字符串未关闭, 起始位置:%d
#define EXPRERRSTR_STRING_NOT_CLOSED(i) winux::FormatA( "String is not closed, the starting position:%d", (int)(i) )
// `%s`不是可赋值的表达式
#define EXPRERRSTR_EXPR_NOT_WRITEABLE(s) winux::FormatA( "`%s` is not writeable expression", (s) )
// `%s`不是%s类型的值
#define EXPRERRSTR_ERROR_VALUE_TYPE( s, s1 ) winux::FormatA( "`%s` is not %s type", (s), (s1) )

// 数组越界
#define EXPRERRSTR_OUT_OF_ARRAY_BOUND( s, n, i ) winux::FormatA( "`%s`: Out of array bound: size=%d, index=%d", (s), (int)(n), (int)(i) )
