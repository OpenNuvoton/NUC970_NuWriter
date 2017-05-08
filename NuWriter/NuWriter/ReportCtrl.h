#ifndef __REPORTCTRL_H__
#define __REPORTCTRL_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// The callback function which is optionally defined by you, the programmer.
// A very common useage of this function is to free item data if they are heap
// pointers, before each list item is deleted. The item data will be passed in as
// the only parameter.
// You may return FALSE if you want to abort the deletion of current item, if you
// want to proceed, you must return TRUE.
typedef BOOL (CALLBACK *ITEMDATAPROC)(DWORD, LPARAM);

class ItemData
{
public:
	ItemData() : dwData(0) {} 
	CStringArray arrStr; // item texts
	DWORD dwData; // the actual item data
};
///////////////////////////////////////////////////////////////////////////
// The CReportCtrl class definition
///////////////////////////////////////////////////////////////////////////
class CReportCtrl : public CListCtrl
{
// Construction
public:
	CReportCtrl();
	virtual ~CReportCtrl();

	///////////////////////////////////////////////////////////////////////
	//		Column attributes & operations
	///////////////////////////////////////////////////////////////////////

	int GetColumnCount() const; // Get the column count.
	BOOL SetHeadings(UINT uiStringID); // Set columns and their formats.
	BOOL SetHeadings(const CString& strHeadings); // Set columns and their formats.
	int InsertColumn(int nCol, LPCTSTR lpszColumnHeading, int nFormat = LVCFMT_LEFT, int nWidth = -1, int nSubItem = -1);
	int InsertColumn(int nCol, const LVCOLUMN* pColumn);
	BOOL DeleteColumn(int nCol);
	BOOL DeleteAllColumns();
	
	///////////////////////////////////////////////////////////////////////
	//		Item attributes & operations
	///////////////////////////////////////////////////////////////////////
	
	// List style operation
	void SetGridLines(BOOL bSet = TRUE); // Show grid lines.
	void SetFullRowSelect(BOOL bSet = TRUE); // Use full row selection style.
	void SetCheckboxes(BOOL bSet = TRUE); // Show checkboxes.
		
	// Selection related
	BOOL IsItemSelected(int nItem) const; // Is item selected?
	int GetSelectedItemCount() const; // How many items are selected?
	int GetUnselectedItemCount() const; // How many items are not selected?
	int GetFirstSelectedItem(int nStartAfter = -1) const; // Index of the first selected item.
	ItemData* GetFirstSelectedItemData(int nStartAfter = -1) const;
	int GetFirstUnselectedItem(int nStartAfter = -1) const; // Index of the first unselected item.
	BOOL SelectItem(int nItem, BOOL bSelectAdjacentIfNotAvailable = FALSE); // Select an item
	BOOL UnSelectItem(int nItem); // Unselect an item.	
	BOOL SelectAllItems(); // Select all items.
	BOOL UnSelectAllItems(); // Unselect all items.
	BOOL InvertSelect(); // Unselect all selected items, and select those were not.	

	// Checkbox related
	BOOL IsItemChecked(int nItem) const; // Is item checked?
	int GetCheckedItemCount() const; // How many items are checked?
	int GetUncheckedItemCount() const; // How many items are not checked?
	int GetFirstCheckedItem(int nStartAfter = -1) const; // Index of the first checked item.
	int GetFirstUncheckedItem(int nStartAfter = -1) const; // Index of the first unchecked item.	
	BOOL CheckItem(int nItem); // Check an item.
	BOOL UnCheckItem(int nItem); // Uncheck an item.
	void CheckAllItems(); // Check all items.
	void UnCheckAllItems(); // Uncheck all items.
	void InvertCheck(); // Uncheck all checked items, and check those were not.

	// Item Insertion	
	int InsertItem(UINT nMask, int nItem, LPCTSTR lpszItem, UINT nState, UINT nStateMask, int nImage, LPARAM lParam);
	int InsertItem(int nItem, LPCTSTR lpszItem, int nImage);
	int InsertItem(const LVITEM* pItem);
	int InsertItem(int nIndex, LPCTSTR pszText, ...);	

	// Item Deletion
	// Note: if the item data are heap pointers, make sure to free them before calling
	//       any item deletion methods, or define and pass in callback function
	//		 "ITEMDATAPROC" and do the memory cleanup within.
	BOOL DeleteItem(int iItem, BOOL bSelectNextItem = FALSE, ITEMDATAPROC lpFunc = NULL, LPARAM lParam = NULL); // Delete an item.
	int DeleteAllItems(ITEMDATAPROC lpFunc = NULL, LPARAM lParam = NULL); // Delete all items.
	int DeleteAllSelectedItems(ITEMDATAPROC lpFunc = NULL, LPARAM lParam = NULL); // Delete all selected items.
	int DeleteAllUnselectedItems(ITEMDATAPROC lpFunc = NULL, LPARAM lParam = NULL); // Delete all unselected items.	
	int DeleteAllCheckedItems(ITEMDATAPROC lpFunc = NULL, LPARAM lParam = NULL); // Delete all checked items.
	int DeleteAllUncheckedItems(ITEMDATAPROC lpFunc = NULL, LPARAM lParam = NULL); // Delete all unchecked items.
	
	// Item position related	
	BOOL SwapItems(int nItem1, int nItem2);	// Swap two items in the list, including texts and item data.
	BOOL MoveUp(int nItem, int nCount = 1); // Move an item up by "nCount" positions.
	BOOL MoveDown(int nItem, int nCount = 1); // Move an item down by "nCount" positions.
	BOOL MoveToTop(int nItem); // Move an item up to the top.
	BOOL MoveToBottom(int nItem); // Move an item down to the bottom.

	// Convenient versions of "CListCtrl::SetItemText"
	BOOL SetItemText(int nItem, int nSubItem, LPCTSTR lpszText); // String.
	BOOL SetItemText(int nItem, int nSubItem, DOUBLE val, int nPrecision = -1); // Double.
	BOOL SetItemText(int nItem, int nSubItem, FLOAT val, int nPrecision = -1); // Float.
	BOOL SetItemText(int nItem, int nSubItem, ULONG val); // Unsigned long.
	BOOL SetItemText(int nItem, int nSubItem, LONG val); // Long.
	BOOL SetItemText(int nItem, int nSubItem, UINT val); // Unsigned int.
	BOOL SetItemText(int nItem, int nSubItem, INT val); // Int.
	BOOL SetItemText(int nItem, int nSubItem, USHORT val); // Unsigned short.
	BOOL SetItemText(int nItem, int nSubItem, SHORT val); // Short.
	BOOL SetItemText(int nItem, int nSubItem, BYTE val); // Byte.
	BOOL SetItemText(int nItem, int nSubItem, TCHAR val); // TCHAR.

	// Sorting
	void Sort(int iColumn, BOOL bAscending); // Sort a specified column.
	void Sort(); // Re-sort previously sorted column, after item insertion or modification.

	// Item data operation
	BOOL SetItemData(int nItem, DWORD dwData); // Assign the 32-bit application defined data.
	DWORD GetItemData(int nItem) const;	// Retrieve the 32-bit application defined data.
	
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CReportCtrl)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

protected:
	
	// Protected member methods
	BOOL _IsValidIndex(int nIndex) const;
	BOOL _ResetTextArray(int nItem); 
	BOOL _AssignNewItemData(int iItem, LPCTSTR lpItem);	
	void _FreeItemMemory(const int iItem);
	BOOL _AssignNewItemData(int iItem, const CString* pTexts, int nSize);

	// Static methods used for sorting and comparison
	static int CALLBACK _CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamData);
	static int _StringCompare(const CString& s1, const CString& s2);
	static BOOL _IsNumber(LPCTSTR pszText);
	static int _NumberCompare(LPCTSTR pszNumber1, LPCTSTR pszNumber2);
	static BOOL _IsDate(LPCTSTR pszText);
	static int _DateCompare(const CString& strDate1, const CString& strDate2);


	////////////////////////////////////////////////////////////////////////////////
	// The header control only used by CReportCtrl
	////////////////////////////////////////////////////////////////////////////////
	class CReportHeaderCtrl : public CHeaderCtrl			
	{
	public:		
		CReportHeaderCtrl();
		virtual ~CReportHeaderCtrl() {};	
		void SetSortedColumn(int nCol);
		void SetSortAscending(BOOL bAscending);
		BOOL IsSortAscending() const;
		int GetSortedColumn() const;
		void UpdateSortArrow();

	protected:
		void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
		int m_iSortColumn;
		BOOL m_bSortAscending;
	};

	// Member data
	CReportHeaderCtrl m_wndHeader;

protected:
	// Wizzard generated stuff

	//{{AFX_MSG(CReportCtrl)
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnDestroy();
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

#endif // __REPORTCTRL_H__
