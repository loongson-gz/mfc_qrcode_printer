#include "stdafx.h"
#include "QR_Image.h"

#include "MainFrm.h"
#include "QR_Encode.h"
#include "ImageView.h"
#include "RightView.h"
#include "BottomView.h"

#include <stdint.h>
#include <iostream>
#include <string>
#include <sstream>
#include "http_client.h"
#include "json/json.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define CX_SPLITTER	7
#define CY_SPLITTER	7
#define CX_MARGIN	2
#define CY_MARGIN	2

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_WM_GETMINMAXINFO()
	ON_WM_TIMER()
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE_AS, OnUpdateFileSaveAs)
	ON_UPDATE_COMMAND_UI(ID_FILE_SAVE, OnUpdateFileSave)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_KANA,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame 

#define QUIT_ID 100

CMainFrame::CMainFrame()
{
	m_bInitSplit = FALSE;

	m_bDataEncoded = FALSE;
	m_pSymbleDC = NULL;

	m_strSavePath.Empty();
	m_strSavePath = _T("d:/1.bmp");

	m_iInterval = 10;
	m_iSN = 1;
	m_iProductionLineSN = 1;
}

CMainFrame::~CMainFrame()
{
	if (m_pSymbleDC != NULL)
	{
		m_pSymbleDC->SelectObject(m_pOldBitmap);

		delete m_pSymbleBitmap;
		delete m_pSymbleDC;
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnCreate

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::PreCreateWindow

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);
	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnSetFocus

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	SetActiveView((CView*)(m_wndSplitter1.GetPane(1, 0)));
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnCmdMsg

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnCreateClient

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext) 
{
	m_wndSplitter1.CreateStatic(this, 2, 1);
	m_wndSplitter1.CreateView(1, 0, RUNTIME_CLASS(CBottomView), CSize(0, 0), pContext);

	m_wndSplitter2.CreateStatic(&m_wndSplitter1, 1, 2);
	m_wndSplitter2.CreateView(0, 0, RUNTIME_CLASS(CImageView), CSize(0, 0), pContext);
	m_wndSplitter2.CreateView(0, 1, RUNTIME_CLASS(CRightView), CSize(0, 0), pContext);

	m_pwndBottomView = ((CBottomView*)m_wndSplitter1.GetPane(1, 0));
	m_pwndRightView  = ((CRightView*)m_wndSplitter2.GetPane(0, 1));

	m_sizeBottomView = ((CFormView*)m_wndSplitter1.GetPane(1, 0))->GetTotalSize();
	m_sizeRightView  = ((CFormView*)m_wndSplitter2.GetPane(0, 1))->GetTotalSize();

	CRect rcClient;
	GetClientRect(rcClient);

	m_wndSplitter1.SetRowInfo(0, (rcClient.Height() - (CY_SPLITTER + CY_MARGIN * 2)) - m_sizeBottomView.cy, 0);
	m_wndSplitter2.SetColumnInfo(0, (rcClient.Width() - (CX_SPLITTER + CX_MARGIN * 2)) - m_sizeRightView.cx, 0);
	//m_wndSplitter1.SetRowInfo(0, (500 - (CY_SPLITTER + CY_MARGIN * 2)) - m_sizeBottomView.cy, 0);
	//m_wndSplitter2.SetColumnInfo(0, (400 - (CX_SPLITTER + CX_MARGIN * 2)) - m_sizeRightView.cx, 0);

	m_bInitSplit = TRUE;

	CString cs;
	cs.Format(_T("万家乐垛码生成器 BUILD:%s %s"), __DATE__, __TIME__);
	SetWindowText(cs);

	return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::RecalcLayout

void CMainFrame::RecalcLayout(BOOL bNotify) 
{
	if (m_bInitSplit)
	{
		CRect rcClient;
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, &rcClient);

		int cy0, cy1, cyMin;

		m_wndSplitter1.GetRowInfo(0, cy0, cyMin);
		m_wndSplitter1.GetRowInfo(1, cy1, cyMin);

		if (cy0 >= 0 && cy1 >= 0)
		{
			if ((cy1 > m_sizeBottomView.cy) && (rcClient.Height() < cy0 + cy1 + (CY_SPLITTER + CY_MARGIN * 2)))
			{
				cy1 = max(m_sizeBottomView.cy, (rcClient.Height() - (CY_SPLITTER + CY_MARGIN * 2)) - cy0);
			}

			cy0 = (rcClient.Height() - (CY_SPLITTER + CY_MARGIN * 2)) - cy1;

			m_wndSplitter1.SetRowInfo(0, cy0, 0);
			m_wndSplitter1.RecalcLayout();
		}

		int cx0, cx1, cxMin;

		m_wndSplitter2.GetColumnInfo(0, cx0, cxMin);
		m_wndSplitter2.GetColumnInfo(1, cx1, cxMin);

		if (cx0 >= 0 && cx1 >= 0)
		{
			if ((cx1 > m_sizeRightView.cx) && (rcClient.Width() < cx0 + cx1 + (CX_SPLITTER + CX_MARGIN * 2)))
			{
				cx1 = max(m_sizeRightView.cx, (rcClient.Width() - (CX_SPLITTER + CX_MARGIN * 2)) - cx0);
			}

			cx0 = (rcClient.Width() - (CX_SPLITTER + CX_MARGIN * 2)) - cx1;

			m_wndSplitter2.SetColumnInfo(0, cx0, 0);
			m_wndSplitter2.RecalcLayout();
		}
	}

	CFrameWnd::RecalcLayout(bNotify);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::OnGetMinMaxInfo

void CMainFrame::OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI) 
{
	static BOOL bRestore = FALSE;

	if (IsWindow(m_wndSplitter1.m_hWnd) && ! bRestore)
	{
		CRect rcWindow;
		GetWindowRect(&rcWindow);

		CRect rcClient;
		RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0, reposQuery, &rcClient);

		lpMMI->ptMinTrackSize.y = m_sizeRightView.cy + m_sizeBottomView.cy + CX_SPLITTER + CX_MARGIN * 2 + (rcWindow.Height() - rcClient.Height());
		lpMMI->ptMinTrackSize.x = m_sizeBottomView.cx + CY_MARGIN * 2 + (rcWindow.Width() - rcClient.Width());
	}

	bRestore = IsIconic();

	CFrameWnd::OnGetMinMaxInfo(lpMMI);
}


/////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnUpdateFileSaveAs(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bDataEncoded);
}

void CMainFrame::OnUpdateFileSave(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(m_bDataEncoded);
}


/////////////////////////////////////////////////////////////////////////////

void CMainFrame::OnFileSave() 
{
	m_strSavePath.IsEmpty() ? SaveAsImage() : SaveImage();
}

void CMainFrame::OnFileSaveAs() 
{
	SaveAsImage();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::ShowImage    CImageView::OnDraw


void CMainFrame::ShowImage()
{
	int nLevel = m_pwndRightView->m_comboLevel.GetCurSel();
	int nVersion = m_pwndRightView->m_comboVersion.GetCurSel();
	BOOL bAutoExtent = (m_pwndRightView->m_checkAutoExtent.GetCheck() != 0);
	int nMaskingNo = m_pwndRightView->m_comboMaskingNo.GetCurSel() - 1;

	CString strEncodeData;
	m_pwndBottomView->m_editSoureData.GetWindowText(strEncodeData);

	CString csSplit("\r\n");
	CStringArray strArray;
	int ret = SplitString(strEncodeData, csSplit, strArray);
	printf("%d\n", ret);
	
	m_iInterval = m_pwndBottomView->GetInterval();
	if (strArray.GetSize() >= m_iInterval+1)
	{
		GetProductionLineSN(strArray.GetAt(0));
		GetProductionCode(strArray.GetAt(0));
		m_csQrCode = GenerateQRCodeVal();
		m_pwndBottomView->m_editSoureData.SetSel(0, -1);
		m_pwndBottomView->m_editSoureData.Clear();
		//m_pwndBottomView->m_editSoureData.SetWindowText(csQrCode);
		m_pwndRightView->GetDlgItem(IDC_QRCODE)->SetWindowText(m_csQrCode);
	}
	else 
	{
		return ;
	}


	int i, j;
	CQR_Encode* pQR_Encode = new CQR_Encode;
	m_bDataEncoded = pQR_Encode->EncodeData(nLevel, nVersion, bAutoExtent, nMaskingNo, m_csQrCode);
	if (m_bDataEncoded)
	{
		if (m_pSymbleDC != NULL)
		{
			m_pSymbleDC->SelectObject(m_pOldBitmap);

			delete m_pSymbleBitmap;
			delete m_pSymbleDC;
		}

		m_nSymbleSize = pQR_Encode->m_nSymbleSize + (QR_MARGIN * 2);

		m_pSymbleBitmap = new CBitmap;
		m_pSymbleBitmap->CreateBitmap(m_nSymbleSize, m_nSymbleSize, 1, 1, NULL);

		m_pSymbleDC = new CDC;
		m_pSymbleDC->CreateCompatibleDC(NULL);

		m_pOldBitmap = m_pSymbleDC->SelectObject(m_pSymbleBitmap);

		m_pSymbleDC->PatBlt(0, 0, m_nSymbleSize, m_nSymbleSize, WHITENESS);
		for (i = 0; i < pQR_Encode->m_nSymbleSize; ++i)
		{
			for (j = 0; j < pQR_Encode->m_nSymbleSize; ++j)
			{
				if (pQR_Encode->m_byModuleData[i][j])
				{
					m_pSymbleDC->SetPixel(i + QR_MARGIN, j + QR_MARGIN, RGB(0, 0, 0));
				}
			}
		}

		CString strWork;

		if (nVersion != pQR_Encode->m_nVersion)
			strWork.Format("(%d)", pQR_Encode->m_nVersion);
		else
			strWork.Empty();

		m_pwndRightView->m_staticVersion.SetWindowText(strWork);

		if (nMaskingNo == -1)
			strWork.Format("(%d)", pQR_Encode->m_nMaskingNo);
		else
			strWork.Empty();

		m_pwndRightView->m_staticMaskingNo.SetWindowText(strWork);

		m_pwndRightView->m_buttonCopy.EnableWindow();
		m_pwndRightView->m_buttonSave.EnableWindow();
	}
	else
	{

		m_pwndRightView->m_staticVersion.SetWindowText("");
		m_pwndRightView->m_staticMaskingNo.SetWindowText("");

		m_pwndRightView->m_buttonCopy.EnableWindow(FALSE);
		m_pwndRightView->m_buttonSave.EnableWindow(FALSE);
	}

	delete pQR_Encode;

	ShowBitmapSize();

	m_wndSplitter1.GetPane(0, 0)->InvalidateRect(NULL, FALSE);

	SaveToSvr(m_csQrCode, strArray);

	SaveImage();
	OnBtnprintbmp();
	//PrintQRCode();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::ShowBitmapSize


void CMainFrame::ShowBitmapSize()
{
	CString strWork;
	strWork.Empty();

	if (m_bDataEncoded)
	{
		int nModuleSize = m_pwndRightView->GetDlgItemInt(IDC_EDITMODULESIZE);

		if (nModuleSize >= 1 && nModuleSize <= 20) 
		{
			int nSize = m_nSymbleSize * nModuleSize;
			strWork.Format("%d  %d", nSize, nSize);
		}
	}

	m_pwndRightView->m_staticBmpSize.SetWindowText(strWork);
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::CopyImage

void CMainFrame::CopyImage(COleDataSource* pOleDataSource)
{
	int nPixelSize = m_nSymbleSize * m_pwndRightView->GetDlgItemInt(IDC_EDITMODULESIZE);

	int nBmpSize = ((nPixelSize + 31) / 32) * 32 / 8;
	nBmpSize *= nPixelSize;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE, sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + nBmpSize);

	if (hGlobal == NULL)
	{
		MessageBox("复制图像", NULL, MB_ICONEXCLAMATION);
		return;
	}

	LPBYTE lpbyBits = (LPBYTE)GlobalLock(hGlobal);

	LPBITMAPINFO pbmi = (LPBITMAPINFO)lpbyBits;
	LPVOID       pbdt = lpbyBits + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;

	// LPBITMAPINFO->BITMAPINFOHEADER
	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= nPixelSize;
	pbmi->bmiHeader.biHeight		= nPixelSize;
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biBitCount		= 1;
	pbmi->bmiHeader.biCompression	= BI_RGB;
	pbmi->bmiHeader.biSizeImage		= nBmpSize;
	pbmi->bmiHeader.biXPelsPerMeter = 3780;
	pbmi->bmiHeader.biYPelsPerMeter = 3780;
	pbmi->bmiHeader.biClrUsed		= 0;
	pbmi->bmiHeader.biClrImportant	= 0;

	CDC* pWorkDC = new CDC;
	pWorkDC->CreateCompatibleDC(NULL);

	CBitmap* pWorkBitmap = new CBitmap;
	pWorkBitmap->CreateBitmap(nPixelSize, nPixelSize, 1, 1, NULL);

	CBitmap* pOldBitmap = pWorkDC->SelectObject(pWorkBitmap);
	pWorkDC->StretchBlt(0, 0, nPixelSize, nPixelSize, m_pSymbleDC, 0, 0, m_nSymbleSize, m_nSymbleSize, SRCCOPY);

	GetDIBits(pWorkDC->m_hDC, (HBITMAP)*pWorkBitmap, 0, nPixelSize, pbdt, pbmi, DIB_RGB_COLORS);

	GlobalUnlock(hGlobal);

	pWorkDC->SelectObject(pOldBitmap);
	delete pWorkBitmap;
	delete pWorkDC;

	if (pOleDataSource == NULL)
	{
		OpenClipboard();
		EmptyClipboard();
		SetClipboardData(CF_DIB, hGlobal);
		CloseClipboard();
	}
	else
	{
		pOleDataSource->CacheGlobalData(CF_DIB, hGlobal);
		pOleDataSource->DoDragDrop(DROPEFFECT_COPY);
	}
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::SaveAsImage


void CMainFrame::SaveAsImage()
{
	// CFileDialog 
	CFileDialog* pFileDialog = new CFileDialog(FALSE, "bmp", m_strSavePath, OFN_OVERWRITEPROMPT | OFN_HIDEREADONLY | OFN_PATHMUSTEXIST,
											   "保存文件(*.bmp)|*.bmp|所有文件 (*.*)|*.*||", this);

	pFileDialog->m_ofn.lpstrTitle = "QR Code 保存?";

	CString strFileName;

	//乽僼傽僀儖曐懚乿僟僀傾儘僌昞帵
	if (pFileDialog->DoModal() == IDOK)
		strFileName = pFileDialog->GetPathName();

	delete 	pFileDialog;

	if (strFileName.IsEmpty())
		return;

	m_strSavePath = strFileName;

	SaveImage();
}


/////////////////////////////////////////////////////////////////////////////
// CMainFrame::SaveImage
// 梡  搑丗俻俼僐乕僪僀儊乕僕傪價僢僩儅僢僾僼傽僀儖偵曐懚

void CMainFrame::SaveImage()
{
	int nPixelSize = m_nSymbleSize * m_pwndRightView->GetDlgItemInt(IDC_EDITMODULESIZE);

	// 價僢僩僨乕僞僒僀僘庢摼
	int nBmpSize = ((nPixelSize + 31) / 32) * 32 / 8;
	nBmpSize *= nPixelSize;

	// 儊儌儕乕儅僢僾僪僼傽僀儖嶌惉
	HANDLE hFile = CreateFile(m_strSavePath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
							  CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		m_strSavePath.Empty();
		MessageBox("是否保存文件","QR Code保存?", MB_ICONSTOP);
		return;
	}

	HANDLE hFileMapping = CreateFileMapping(hFile, NULL, PAGE_READWRITE | SEC_COMMIT, 0, 
						  sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + nBmpSize, NULL);
	LPBYTE lpbyMapView = (LPBYTE)MapViewOfFile(hFileMapping, FILE_MAP_WRITE, 0, 0, 0);

	ZeroMemory(lpbyMapView, sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + nBmpSize);

	// 峔憿懱傾僪儗僗妱晅
	LPBITMAPFILEHEADER pbfh = (LPBITMAPFILEHEADER)lpbyMapView;
	LPBITMAPINFO       pbmi = (LPBITMAPINFO)(lpbyMapView + sizeof(BITMAPFILEHEADER));
	LPVOID             pbdt = lpbyMapView + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;

	// BITMAPFILEHEADER
	pbfh->bfType      = (WORD) (('M' << 8) | 'B'); // "BM"
	pbfh->bfSize      = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2 + nBmpSize;
	pbfh->bfReserved1 = 0;
	pbfh->bfReserved2 = 0;
	pbfh->bfOffBits   = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + sizeof(RGBQUAD) * 2;

	// LPBITMAPINFO->BITMAPINFOHEADER
	pbmi->bmiHeader.biSize			= sizeof(BITMAPINFOHEADER);
	pbmi->bmiHeader.biWidth			= nPixelSize;
	pbmi->bmiHeader.biHeight		= nPixelSize;
	pbmi->bmiHeader.biPlanes		= 1;
	pbmi->bmiHeader.biBitCount		= 1;
	pbmi->bmiHeader.biCompression	= BI_RGB;
	pbmi->bmiHeader.biSizeImage		= nBmpSize;
	pbmi->bmiHeader.biXPelsPerMeter = 3780;
	pbmi->bmiHeader.biYPelsPerMeter = 3780;
	pbmi->bmiHeader.biClrUsed		= 0;
	pbmi->bmiHeader.biClrImportant	= 0;

	CDC* pWorkDC = new CDC;
	pWorkDC->CreateCompatibleDC(NULL);

	CBitmap* pWorkBitmap = new CBitmap;
	pWorkBitmap->CreateBitmap(nPixelSize, nPixelSize, 1, 1, NULL);

	CBitmap* pOldBitmap = pWorkDC->SelectObject(pWorkBitmap);
	pWorkDC->StretchBlt(0, 0, nPixelSize, nPixelSize, m_pSymbleDC, 0, 0, m_nSymbleSize, m_nSymbleSize, SRCCOPY);

	GetDIBits(pWorkDC->m_hDC, (HBITMAP)*pWorkBitmap, 0, nPixelSize, pbdt, pbmi, DIB_RGB_COLORS);

	pWorkDC->SelectObject(pOldBitmap);
	delete pWorkBitmap;
	delete pWorkDC;

	UnmapViewOfFile(lpbyMapView);
	CloseHandle(hFileMapping);
	CloseHandle(hFile);
}

void CMainFrame::PrintQRCode()
{
	// TODO: 在此添加控件通知处理程序代码
	DWORD dwflags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION;
	CPrintDialog dlg(FALSE, dwflags, NULL);
	//CPrintDialog dlg(FALSE);  
	//dlg.GetDefaults();  
	if (dlg.DoModal() == IDOK)
	{
		HDC hdcPrinter = dlg.GetPrinterDC();
		if (hdcPrinter == NULL)
		{
			MessageBox(_T("没有打印设备!"));
		}
		else
		{
			CDC dcPrinter;
			dcPrinter.Attach(hdcPrinter);

			DOCINFO docinfo;
			memset(&docinfo, 0, sizeof(docinfo));
			docinfo.cbSize = sizeof(docinfo);
			docinfo.lpszDocName = _T("打印");

			if (dcPrinter.StartDoc(&docinfo) < 0)
			{
				MessageBox(_T("设备没有初始化!"));
			}
			else
			{
				if (dcPrinter.StartPage() < 0)
				{
					MessageBox(_T("没有起始页"));
					dcPrinter.AbortDoc();
				}
				else
				{
					CFont TitleFont, DetailFont, *oldfont;
					TitleFont.CreateFont(-MulDiv(14, dcPrinter.GetDeviceCaps(LOGPIXELSY), 72),
						0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET,
						OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
						VARIABLE_PITCH | FF_SWISS, _T("黑体"));
					//细节字体
					DetailFont.CreateFont(-MulDiv(10, dcPrinter.GetDeviceCaps(LOGPIXELSY), 72),
						0, 0, 0, FW_NORMAL, 0, 0, 0, GB2312_CHARSET,
						OUT_STROKE_PRECIS, CLIP_STROKE_PRECIS, DRAFT_QUALITY,
						VARIABLE_PITCH | FF_SWISS, _T("宋体"));

					//CGdiObject* pOldFont = dcPrinter.SelectStockObject(SYSTEM_FONT); 
					int width = GetDeviceCaps(dcPrinter, HORZSIZE);
					int height = GetDeviceCaps(dcPrinter, VERTSIZE);
					int xP = GetDeviceCaps(dcPrinter, LOGPIXELSX);	//x方向每英寸像素点数
					int yP = GetDeviceCaps(dcPrinter, LOGPIXELSY);	//y方向每英寸像素点数

					DOUBLE xPix = (DOUBLE)xP * 10 / 254;	//每 mm 宽度的像素
					DOUBLE yPix = (DOUBLE)yP * 10 / 254;	//每 mm 高度的像素	
					DOUBLE fAdd = 7 * yPix;		//每格递增量
					DOUBLE nTop = 10 * yPix;		//第一页最上线
					DOUBLE nLeft = 20 * xPix;			//最左线
					DOUBLE nRight = xPix*(width - 20);	//最右线
					oldfont = dcPrinter.SelectObject(&TitleFont);
					CRect rc, rc1, rc2, rc3, rc4, rc5, rc6, rc7, rc8, rc9;
					CRect rc10, rc11, rc12, rc13, rc14, rc15, rc16;
					rc.SetRect(nLeft, nTop, nRight, nTop + fAdd);
					dcPrinter.DrawText(_T("产品编码列表"), &rc, DT_CENTER | DT_VCENTER | DT_WORDBREAK);

					dcPrinter.EndPage();
					dcPrinter.EndDoc();
					TitleFont.DeleteObject();
					DetailFont.DeleteObject();
					DeleteDC(dcPrinter.Detach());
				}
			}
		}
	}

}

#if 0
void CMainFrame::OnBtnprintbmp()
{
	// TODO: Add your control notification handler code here
	CString filename = m_strSavePath;
	//CFileDialog fileDlg(TRUE);
	//fileDlg.m_ofn.lpstrTitle = "打开对话框";
	//fileDlg.m_ofn.lpstrFilter = "Bmp Files(*.bmp)\0*.bmp\0All Files(*.*)\0*.*\0\0";

	//if (IDOK == fileDlg.DoModal())
	//{
	//	filename = fileDlg.GetPathName();
	//}


	CPrintDialog printDlg(FALSE);
	printDlg.GetDefaults();
	// Or get from user: 
	//if (printDlg.DoModal() == IDCANCEL)   
	//	return; 

	//DWORD dwflags = PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION;
	//CPrintDialog printDlg(FALSE, dwflags, NULL);
	//if (printDlg.DoModal() == IDOK)
	{
		//HDC hdcPrinter = printDlg.GetPrinterDC();
		//if (hdcPrinter == NULL)
		//{
		//	MessageBox(_T("没有打印设备!"));
		//}
		//else 
		{
			CDC dc;
			if (!dc.Attach(printDlg.GetPrinterDC()))
			{
				AfxMessageBox(_T("No printer found!")); return;
			}

			dc.m_bPrinting = TRUE;
			DOCINFO di;
			// Initialise print document details 
			::ZeroMemory(&di, sizeof(DOCINFO));
			di.cbSize = sizeof(DOCINFO);
			di.lpszDocName = filename;
			BOOL bPrintingOK = dc.StartDoc(&di); // Begin a new print job 
			// Get the printing extents 
			// and store in the m_rectDraw field of a 
			// CPrintInfo object 
			CPrintInfo Info;
			Info.SetMaxPage(1); // just one page 
			int maxw = dc.GetDeviceCaps(HORZRES);
			int maxh = dc.GetDeviceCaps(VERTRES);
			int xP = GetDeviceCaps(dc, LOGPIXELSX);	//x方向每英寸像素点数
			int yP = GetDeviceCaps(dc, LOGPIXELSY);	//y方向每英寸像素点数

			Info.m_rectDraw.SetRect(0, 0, maxw, maxh);
			//for (UINT page = Info.GetMinPage(); page <=
			//	Info.GetMaxPage() && bPrintingOK; page++)
			{
				dc.StartPage();    // begin new page 
				Info.m_nCurPage = 1;//page;
				CBitmap bitmap;
				// LoadImage does the trick here, it creates a DIB section 
				// You can also use a resource here 
				// by using MAKEINTRESOURCE() ... etc. 

				if (!bitmap.Attach(LoadImage(AfxGetInstanceHandle(), filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)))
				{
					AfxMessageBox(_T("Error loading bitmap!")); return;
				}
				BITMAP bm;
				bitmap.GetBitmap(&bm);
				int w = bm.bmWidth;
				int h = bm.bmHeight;
				// create memory device context 
				CDC memDC;
				memDC.CreateCompatibleDC(&dc);
				CBitmap *pBmp = memDC.SelectObject(&bitmap);
				memDC.SetMapMode(dc.GetMapMode());
				dc.SetStretchBltMode(HALFTONE);
				// now stretchblt to maximum width on page 
				dc.StretchBlt(0, 0, maxw, maxh, &memDC, 0, 0, w, h, SRCCOPY);
				//dc.StretchBlt(w*1.6, 0, w*2, h*2, &memDC, 0, 0, w, h, SRCCOPY);

				// clean up 
				memDC.SelectObject(pBmp);

				DOUBLE xPix = (DOUBLE)xP*10/254;	//每 mm 宽度的像素
				DOUBLE yPix = (DOUBLE)yP*10/254;	//每 mm 高度的像素	
				DOUBLE fAdd = 7*yPix;		//每格递增量
				DOUBLE nTop = 10*yPix;		//第一页最上线
				DOUBLE nLeft = 20*xPix;			//最左线
				DOUBLE nRight = xPix*(maxw-20);	//最右线

				dc.MoveTo(0, 0);
				dc.LineTo(0, maxh);
				CRect rc,rc1;
				rc.SetRect(5, 5, nRight, h-5);
				dc.DrawText(m_csQrCode, &rc, DT_CENTER | DT_VCENTER |DT_WORDBREAK );

				bPrintingOK = (dc.EndPage() > 0);   // end page 
			}
			if (bPrintingOK)
				dc.EndDoc(); // end a print job 
			else 
				dc.AbortDoc();           // abort job. 
			//  } 

		}
	}

	
}
#endif

void CMainFrame::OnBtnprintbmp()
{
	// TODO: Add your control notification handler code here
	CString filename = m_strSavePath;

	CPrintDialog printDlg(FALSE);
	printDlg.GetDefaults();
	
	CDC dc;
	if (!dc.Attach(printDlg.GetPrinterDC()))
	{
		AfxMessageBox(_T("No printer found!")); return;
	}

	dc.m_bPrinting = TRUE;
	DOCINFO di;
	// Initialise print document details 
	::ZeroMemory(&di, sizeof(DOCINFO));
	di.cbSize = sizeof(DOCINFO);
	di.lpszDocName = filename;
	BOOL bPrintingOK = dc.StartDoc(&di); // Begin a new print job 

	CPrintInfo Info;
	Info.SetMaxPage(1); // just one page 

	CFont TitleFont, DetailFont, *oldfont;
	TitleFont.CreateFont(-MulDiv(12,dc.GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("黑体"));
	//细节字体
	DetailFont.CreateFont(-MulDiv(10,dc.GetDeviceCaps(LOGPIXELSY),72),
		0,0,0,FW_NORMAL,0,0,0,GB2312_CHARSET,
		OUT_STROKE_PRECIS,CLIP_STROKE_PRECIS,DRAFT_QUALITY,
		VARIABLE_PITCH|FF_SWISS,_T("宋体"));
	//CGdiObject* pOldFont = dcPrinter.SelectStockObject(SYSTEM_FONT); 
	int width  = GetDeviceCaps(dc,HORZSIZE);
	int height  = GetDeviceCaps(dc,VERTSIZE);
	int xP = GetDeviceCaps(dc, LOGPIXELSX);	//x方向每英寸像素点数
	int yP = GetDeviceCaps(dc, LOGPIXELSY);	//y方向每英寸像素点数

	DOUBLE xPix = (DOUBLE)xP*10/254;	//每 mm 宽度的像素
	DOUBLE yPix = (DOUBLE)yP*10/254;	//每 mm 高度的像素	
	DOUBLE fAdd = 4*yPix;		//每格递增量
	DOUBLE nTop = 1*yPix;		//第一页最上线
	DOUBLE nLeft = 3*xPix;			//最左线
	DOUBLE nRight = xPix*(width-1);	//最右线
	DOUBLE nSplitLeft = nLeft+nRight*7/16;		//中线

	oldfont = dc.SelectObject(&TitleFont);
	CRect rc,rc1,rc2,rc3,rc4,rc5,rc6,rc7,rc8,rc9;
	CRect rc10,rc11,rc12,rc13,rc14,rc15,rc16;

	//顶线
	rc.SetRect(nLeft,nTop,nRight,nTop);
	dc.MoveTo(rc.left,rc.bottom);
	dc.LineTo(rc.right,rc.bottom);

	rc2.SetRect(nSplitLeft, rc.bottom+xPix, nRight, rc.bottom+2*fAdd);
	dc.DrawText(_T("型号:JSQ24-12HA5"), &rc2, DT_LEFT | DT_VCENTER |DT_WORDBREAK );
	//底线
	dc.MoveTo(nSplitLeft, rc2.bottom - xPix);
	dc.LineTo(rc2.right,rc2.bottom - xPix);

	rc4.SetRect(nSplitLeft, rc2.bottom+xPix, nRight,rc2.bottom+2*fAdd);
	CString csCount;
	csCount.Format(_T("数量:%d"), m_iInterval);
	dc.DrawText(csCount, &rc4, DT_LEFT | DT_VCENTER |DT_WORDBREAK );
	//底线
	dc.MoveTo(nSplitLeft,rc4.bottom - xPix);
	dc.LineTo(rc4.right,rc4.bottom - xPix);

	rc6.SetRect(nSplitLeft, rc4.bottom+xPix, nRight,rc4.bottom+2*fAdd);
	CString csDate;
	csDate.Format(_T("日期:%s"), m_csDateTime);
	dc.DrawText(csDate, &rc6, DT_LEFT | DT_VCENTER |DT_WORDBREAK );
	//底线
	dc.MoveTo(nSplitLeft,rc6.bottom - xPix);
	dc.LineTo(rc6.right,rc6.bottom - xPix);

	rc8.SetRect(nSplitLeft, rc6.bottom+xPix, nRight,rc6.bottom+fAdd+yPix);
	dc.DrawText(_T("ERP:C8901692"), &rc8, DT_LEFT | DT_VCENTER |DT_WORDBREAK );

	//底线
	dc.MoveTo(nLeft,rc8.bottom);
	dc.LineTo(rc8.right,rc8.bottom);

	//左线
	dc.MoveTo(nLeft, nTop);
	dc.LineTo(nLeft, rc8.bottom);

	//中分线
	dc.MoveTo(nSplitLeft , nTop);
	dc.LineTo(nSplitLeft , rc8.bottom);

	//右线
	dc.MoveTo(nRight, nTop);
	dc.LineTo(nRight, rc8.bottom);

	//CString filename(_T("d:/1.bmp"));
	CBitmap bitmap;
	if (!bitmap.Attach(LoadImage(AfxGetInstanceHandle(), filename, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE)))
	{
		AfxMessageBox(_T("Error loading bitmap!")); return;
	}
	BITMAP bm;
	bitmap.GetBitmap(&bm);
	int w = bm.bmWidth;
	int h = bm.bmHeight;
	// create memory device context 
	CDC memDC;
	memDC.CreateCompatibleDC(&dc);
	CBitmap *pBmp = memDC.SelectObject(&bitmap);
	memDC.SetMapMode(dc.GetMapMode());
	dc.SetStretchBltMode(HALFTONE);
	// now stretchblt to maximum width on page 
	dc.StretchBlt(rc.left + 5*xPix, rc.top + yPix, w, h, &memDC, 0, 0, w, h, SRCCOPY);
	//dc.StretchBlt(w*1.6, 0, w*2, h*2, &memDC, 0, 0, w, h, SRCCOPY);

	// clean up 
	memDC.SelectObject(pBmp);

	rc9.SetRect(rc.left + xPix, rc.top + yPix + h + fAdd, nLeft+nRight/2, rc6.bottom+fAdd);
	dc.DrawText(m_csQrCode, &rc9, DT_LEFT | DT_VCENTER |DT_WORDBREAK );

	bPrintingOK = (dc.EndPage() > 0);   // end page  
	if (bPrintingOK)
		dc.EndDoc(); // end a print job 
	else 
		dc.AbortDoc();           // abort job. 
	TitleFont.DeleteObject();
	DetailFont.DeleteObject();

}


int CMainFrame::SplitString(const CString str, CString split, CStringArray &strArray)
{
	strArray.RemoveAll();
	CString strTemp = str;
	int iIndex = 0;
	while (1)
	{
		iIndex = strTemp.Find(split);
		if (iIndex >= 0)
		{
			strArray.Add(strTemp.Left(iIndex));
			strTemp = strTemp.Right(strTemp.GetLength() - iIndex - split.GetLength());
		}
		else
		{
			break;
		}
	}
	strArray.Add(strTemp);

	return strArray.GetSize();
}

CString CMainFrame::GenerateQRCodeVal()
{
	SYSTEMTIME t;
	GetSystemTime(&t);

	m_csDateTime.Format(_T("%02d%02d%02d"), t.wYear-2000, t.wMonth, t.wDay);

	CString cs;
	cs.Format(_T("ZJ%s%02d%03d"), m_csDateTime, m_iProductionLineSN, m_iSN++);
	return cs;
}


static inline std::string HttpUrl(const std::string& host, int16_t port, const std::string& append) {
	std::stringstream ss;
	ss << "http://";
	ss << (host.empty() ? "127.0.0.1" : host);
	ss << ":" << port;
	ss << append;
	return ss.str();
}

static inline int32_t HttpPost(const std::string& url, const Json::Value& request, Json::Value& response) 
{
	std::string tmp;
	Json::FastWriter writer;
	std::string strReq = writer.write(request);
	//std::string strReq = request.empty() ? std::string():request.toStyledString();
	HttpClient http;
	int ret = http.Post(url, strReq, tmp);
	if (ret < 0)
	{
		return ret;
	}
	Json::Reader reader;
	if (!reader.parse(tmp, response, false))
	{
		std::cout << __FUNCTION__ << " json parse failed !" << std::endl;
		return -1;
	}
	return 0;
}

//192.168.103.11:8070
void CMainFrame::SaveToSvr(CString csQrCode, const CStringArray &arrLst)
{
	Json::Value jsonRoot;
	Json::Value jsonItem;
	jsonItem["buttress_code"] = csQrCode.GetBuffer(0);

	//Json::Value jsonBarCode;
	for (int i = 0; i<arrLst.GetSize(); i++)
	{
		CString cs = arrLst.GetAt(i);
		jsonItem["bar_code"].append(cs.GetBuffer(0));
	}

	jsonRoot.append(jsonItem);
	//jsonRoot.append(jsonBarCode);

	Json::FastWriter writer;
	std::string str = writer.write(jsonItem);

	//std::string str = jsonRoot.toStyledString();
	//OutputDebugString(str);


	//std::string url = HttpUrl("192.168.1.101", 8080, "/WJLPdaServer/gongdanguanli/insertChengPinShangXian");
	std::string url = HttpUrl("192.168.103.11", 8070, "/WJLPdaServer/gongdanguanli/insertChengPinShangXian");

	Json::Value response;
	int ret = HttpPost(url, jsonItem, response);
	if (ret !=0)
	{
		SetTimer(QUIT_ID,1000,NULL);
		AfxMessageBox(_T("入库失败"));
		return;
	}

	bool status = response["status"].asBool();
	if (!status)
	{
		SetTimer(QUIT_ID,1000,NULL);
		CString message = response["message"].asCString();
		AfxMessageBox(message);
	}

}

void CMainFrame::GetProductionLineSN(CString sn)
{
	int pos = sn.GetLength() - 6;
	CString cs = sn.Mid(pos, 2);
	m_iProductionLineSN = atoi(cs.GetBuffer(0));
}

void CMainFrame::GetProductionCode(CString cs)
{
	int pos = 10;
	m_csProdutionCode = cs.Mid(pos, 5);
}

void CMainFrame::OnTimer(UINT_PTR nIDEvent)
{
	if (nIDEvent == QUIT_ID)
	{
		KillTimer(nIDEvent);
		keybd_event(VK_RETURN,0,0,0);
		keybd_event(VK_RETURN,0,KEYEVENTF_KEYUP,0);//模拟"回车"命令
	}
}
