#pragma once


// CTTSInitDlg 对话框

class CTTSInitDlg : public CDialogEx
{
	DECLARE_DYNAMIC(CTTSInitDlg)

public:
	CTTSInitDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~CTTSInitDlg();

// 对话框数据
	enum { IDD = IDD_TTSINIT_DIALOG };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int m_init_result;
	afx_msg void OnBnClickedOk();
	CTTSDemoDlg * mainDlg;
	virtual BOOL OnInitDialog();
	CString m_selectedModule;
};
