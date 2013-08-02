// 支持alpha通道的ImageList
// 支持PNG,JPG,GIF,BMP,ICO等等图片格式
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IMAGELIST_H__C284F61F_22A0_4699_9D90_5AB1C75E8FF5__INCLUDED_)
#define AFX_IMAGELIST_H__C284F61F_22A0_4699_9D90_5AB1C75E8FF5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class ImageList : public CImageList
{
public:
	ImageList();
	virtual ~ImageList();
	// 需要GDI+支持,可载入多种图片格式,支持Alpha通道
	// ImageList创建时必须指定ILC_COLOR32标志
	// 当ImageList为ILC_COLOR32时,ImageList::Draw()无法正常画出图标
	// 但关联的控件可以正确显示图标
	int AddAlphaImage( Gdiplus::Bitmap & bm );
};

#endif // !defined(AFX_IMAGELIST_H__C284F61F_22A0_4699_9D90_5AB1C75E8FF5__INCLUDED_)
