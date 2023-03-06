//////////////////////////////////////////////////////////////////////
// desc:    控制台相关
// author:  WT
//////////////////////////////////////////////////////////////////////

#if !defined(__WINPLUS_CONSOLE_HPP__)
#define __WINPLUS_CONSOLE_HPP__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace winplus
{

/* 确保文本宽度 */
WINPLUS_FUNC_DECL(String) MakesureTextWidth( String const & text, int text_width, bool left_align = false, int ellipsis = 3 );

/* 计算进度块数目 */
inline int CalcBarBlocks( int text_limit, int screen_width )
{
    return ( screen_width - text_limit - 1 - 1 - 1 ) / 2;
}

/* 输出一个横条，左边带一个文字说明 */
WINPLUS_FUNC_DECL(void) PrintBarEx( String const & text, int text_width, int screen_width, WORD clr, int bar_blocks );

/* 在gui模式下开启控制台窗口 */
WINPLUS_FUNC_DECL(void) Win32GUI_ShowConsole();

} // namespace winplus

#endif // !defined(__WINPLUS_CONSOLE_HPP__)
