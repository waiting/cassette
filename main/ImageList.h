// ֧��alphaͨ����ImageList
// ֧��PNG,JPG,GIF,BMP,ICO�ȵ�ͼƬ��ʽ
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
	// ��ҪGDI+֧��,���������ͼƬ��ʽ,֧��Alphaͨ��
	// ImageList����ʱ����ָ��ILC_COLOR32��־
	// ��ImageListΪILC_COLOR32ʱ,ImageList::Draw()�޷���������ͼ��
	// �������Ŀؼ�������ȷ��ʾͼ��
	int AddAlphaImage( Gdiplus::Bitmap & bm );
};

#endif // !defined(AFX_IMAGELIST_H__C284F61F_22A0_4699_9D90_5AB1C75E8FF5__INCLUDED_)
