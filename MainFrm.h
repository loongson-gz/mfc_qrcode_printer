// MainFrm.h : CMainFrame 僋儔僗愰尵偍傛傃僀儞僞乕僼僃僀僗掕媊
// Date 2006/05/17	Ver. 1.22	Psytec Inc.

#if !defined(AFX_MAINFRM_H__F7C4DD34_15EF_41C5_8907_C79B3F8FF648__INCLUDED_)
#define AFX_MAINFRM_H__F7C4DD34_15EF_41C5_8907_C79B3F8FF648__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "QR_Encode.h"
#include "ImageView.h"
#include "RightView.h"
#include "BottomView.h"
#include <set>

#include "product_client.h"

/////////////////////////////////////////////////////////////////////////////
// CMainFrame 僋儔僗

class CMainFrame : public CFrameWnd
{
public:
	CMainFrame();
protected: 
	DECLARE_DYNAMIC(CMainFrame)

// 傾僩儕價儏乕僩
public:
	// 俻俼僐乕僪昞帵梡
	int m_bDataEncoded;
	int m_nSymbleSize;

	CDC* m_pSymbleDC;
	CBitmap* m_pSymbleBitmap;
	CBitmap* m_pOldBitmap;

	// 暘妱僂傿儞僪僂娭楢
	BOOL m_bInitSplit;

	CSplitterWnd m_wndSplitter1;
	CSplitterWnd m_wndSplitter2;

	CSize m_sizeBottomView;
	CSize m_sizeRightView;

	CBottomView* m_pwndBottomView;
	CRightView*  m_pwndRightView;

	// 僼傽僀儖曐懚僷僗
	CString m_strSavePath;

// 僆儁儗乕僔儑儞
public:
	void ShowImage();
	void ShowBitmapSize();
	void CopyImage(COleDataSource* pOleDataSource = NULL);
	void SaveAsImage();
	void SaveImage();

// 僆乕僶乕儔僀僪
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual void RecalcLayout(BOOL bNotify = TRUE);
	protected:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	//}}AFX_VIRTUAL

// 僀儞僾儕儊儞僥乕僔儑儞
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // 僐儞僩儘乕儖僶乕梡儊儞僶
	CStatusBar m_wndStatusBar;
	CToolBar   m_wndToolBar;

// 惗惉偝傟偨儊僢僙乕僕儅僢僾娭悢
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnGetMinMaxInfo(MINMAXINFO FAR* lpMMI);
	afx_msg void OnUpdateFileSaveAs(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFileSave(CCmdUI* pCmdUI);
	afx_msg void OnFileSave();
	afx_msg void OnFileSaveAs();
	void OnTimer(UINT_PTR nIDEvent);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

public:
		//生成二维码的条码间隔值
	int m_iInterval;

	int SplitString(const CString str, CString split, CStringArray & strArray);
	int SplitString(const CString str, CString split, std::set<CString> &strArray);
	void PrintQRCode();
	CString GenerateQRCodeVal();

	void GetProductRule();
	int InitDB();
	void GetProductionLineSNAndProductionCode(CString productSN);
	void GetErpCodeAndProductKind();
	void GetProductionLineSN(CString sn, int pos);
	CString GetProductionCode(CString cs, int pos, int len);
	void OnBtnprintbmp();
	void SaveToSvr(CString csQrCode, const CStringArray & arrLst);
	bool IsSwitchProdution(const CStringArray & arrLst);

	int m_iSN;						//流水号
	int m_iProductionLineSN;		//检线号
	CString m_csProdutionCode;		//产品条码
	CString m_csDateTime;			//当前日期
	CString m_csQrCode;				//生成二维码数值
	CString m_csErpCode;
	CString m_csProductKind;
	ProductClient *m_pc;

	int m_iProductPos ;		//产品编号起始位置
	int m_iProductCodeLen;	//产品编号长度
	int m_iWorkshopPos ;	//车间编号的起始位置，从尾开始

	bool m_bGetRule;
	TBarcodeRuleLst m_ruleLst;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}

#endif // !defined(AFX_MAINFRM_H__F7C4DD34_15EF_41C5_8907_C79B3F8FF648__INCLUDED_)
