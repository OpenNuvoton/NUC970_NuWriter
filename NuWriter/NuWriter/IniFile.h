// IniFile.h: interface for the CIniFile class.
// Written by: Adam Clauss
// Email: pandcc3@comwerx.net
// You may use this class/code as you wish in your programs.  Feel free to distribute it, and
// email suggested changes to me.
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_INIFILE_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_)
#define AFX_INIFILE_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "stdafx.h"

#include <iostream>
using namespace std;  //Tells the compiler that your using the standard namespace

class CIniFile  
{
	//all private variables
private:

	//stores pathname of ini file to read/write
	CString path;
	
	//all keys are of this time
	struct key
	{
		//list of values in key
		CArray<CString, CString> values; 

		//corresponding list of value names
		CArray<CString, CString> names;
	};

	//list of keys in ini
	CArray<key, key> keys; 

	//corresponding list of keynames
	CArray<CString, CString> names; 
	
	
	//all private functions
private:
	//overloaded to take CString
	istream & getline( istream & is, CString & str );

	//returns index of specified value, in the specified key, or -1 if not found
	int FindValue(int keynum, CString valuename);

	//returns index of specified key, or -1 if not found
	int FindKey(CString keyname);


	//public variables
public:

	//will contain error info if one occurs
	//ended up not using much, just in ReadFile and GetValue
	CString error;


	//public functions
public:
	//default constructor
	CIniFile();

	//constructor, can specify pathname here instead of using SetPath later
	CIniFile(CString inipath);

	//default destructor
	virtual ~CIniFile();

	//sets path of ini file to read and write from
	void SetPath(CString newpath);
	CString GetPath();
	int FindKeyX(CString keyname);

	//reads ini file specified using CIniFile::SetPath()
	//returns true if successful, false otherwise
	bool ReadFile();

	//writes data stored in class to ini file
	void WriteFile(); 

	//deletes all stored ini data
	void Reset();

	//returns number of keys currently in the ini
	int GetNumKeys();

	//returns number of values stored for specified key
	int GetNumValues(CString keyname);

	//gets value of [keyname] valuename = 
	//overloaded to return CString, int, and double,
	//returns "", or 0 if key/value not found.  Sets error member to show problem
	CString GetValue(CString keyname, CString valuename); 
	int GetValueI(CString keyname, CString valuename); 
	double GetValueF(CString keyname, CString valuename);

	//sets value of [keyname] valuename =.
	//specify the optional paramter as false (0) if you do not want it to create
	//the key if it doesn't exist. Returns true if data entered, false otherwise
	//overloaded to accept CString, int, and double
	bool SetValue(CString key, CString valuename, CString value, bool create = 1);
	bool SetValueI(CString key, CString valuename, int value, bool create = 1);
	bool SetValueF(CString key, CString valuename, double value, bool create = 1);

	//deletes specified value
	//returns true if value existed and deleted, false otherwise
	bool DeleteValue(CString keyname, CString valuename);

	//deletes specified key and all values contained within
	//returns true if key existed and deleted, false otherwise
	bool DeleteKey(CString keyname);

	int GetKeysSize(CString& keyname);
	CString EnumKeysValue(CString& keyname,int valuenum);
};

#endif // !defined(AFX_INIFILE_H__D6BE0D97_13A8_11D4_A5D2_002078B03530__INCLUDED_)
