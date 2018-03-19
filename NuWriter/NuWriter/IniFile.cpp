// IniFile.cpp: implementation of the CIniFile class.
// Written by: Adam Clauss
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IniFile.h"

#include <fstream>
using namespace std;  //Tells the compiler that your using the standard namespace


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

/////////////////////////////////////////////////////////////////////
// Construction/Destruction
/////////////////////////////////////////////////////////////////////
#define BUFFER_SIZE 256
//default constructor
CIniFile::CIniFile()
{
    setlocale(LC_ALL, "");
}

//constructor, can specify pathname here instead of using SetPath later
CIniFile::CIniFile(CString inipath)
{
    path = inipath;
}

//default destructor
CIniFile::~CIniFile()
{

}

/////////////////////////////////////////////////////////////////////
// Public Functions
/////////////////////////////////////////////////////////////////////

//sets path of ini file to read and write from
void CIniFile::SetPath(CString newpath)
{
    path = newpath;
}

CString CIniFile::GetPath()
{
    return path;
}
//reads ini file specified using CIniFile::SetPath()
//returns true if successful, false otherwise
bool CIniFile::ReadFile()
{
    CFile file;
    CFileStatus status;
    if (!file.GetStatus(path,status))
        return 0;
    ifstream inifile;
    CString readinfo;
    inifile.open(path);
    int curkey = -1, curval = -1;
    if (inifile.fail()) {
        error = "Unable to open ini file.";
        return 0;
    }
    CString keyname, valuename, value;
    CString temp;
    while (getline(inifile,readinfo)) {
        if (readinfo != "") {
            if (readinfo[0] == '[' && readinfo[readinfo.GetLength()-1] == ']') { //if a section heading
                keyname = readinfo;
                keyname.TrimLeft('[');
                keyname.TrimRight(']');
            } else { //if a value
                valuename = readinfo.Left(readinfo.Find(_T("=")));
                value = readinfo.Right(readinfo.GetLength()-valuename.GetLength()-1);
                SetValue(keyname,valuename,value);
            }
        }
    }
    inifile.close();
    return 1;
}

//writes data stored in class to ini file
void CIniFile::WriteFile()
{
    ofstream inifile;
    CString zTmp;
    inifile.open(path);
    char buffer1[BUFFER_SIZE];
    char buffer2[BUFFER_SIZE];
    for (int keynum = 0; keynum <= names.GetUpperBound(); keynum++) {
        if (keys[keynum].names.GetSize() != 0) {

            wcstombs ( buffer1,(wchar_t *) names[keynum].GetBuffer(), sizeof(buffer1) );
            inifile << '[' << buffer1 << ']' << endl;
            zTmp=names[keynum];
            for (int valuenum = 0; valuenum <= keys[keynum].names.GetUpperBound(); valuenum++) {
                wcstombs ( buffer1,(wchar_t *) keys[keynum].names[valuenum].GetBuffer(), sizeof(buffer1) );
                wcstombs ( buffer2,(wchar_t *) keys[keynum].values[valuenum].GetBuffer(), sizeof(buffer2) );
                inifile << buffer1 << "=" << buffer2;
                if (valuenum != keys[keynum].names.GetUpperBound())
                    inifile << endl;
                else if (keynum < names.GetSize())
                    inifile << endl;
            }
            if (keynum < names.GetSize())
                inifile << endl;
        }
    }
    inifile.close();

#if 0
    ofstream inifile;
    inifile.open(path);
    for (int keynum = 0; keynum <= names.GetUpperBound(); keynum++) {
        if (keys[keynum].names.GetSize() != 0) {
            inifile << '[' << names[keynum] << ']' << endl;
            for (int valuenum = 0; valuenum <= keys[keynum].names.GetUpperBound(); valuenum++) {
                inifile << keys[keynum].names[valuenum] << "=" << keys[keynum].values[valuenum];
                if (valuenum != keys[keynum].names.GetUpperBound())
                    inifile << endl;
                else if (keynum < names.GetSize())
                    inifile << endl;
            }
            if (keynum < names.GetSize())
                inifile << endl;
        }
    }
    inifile.close();
#endif
}

//deletes all stored ini data
void CIniFile::Reset()
{
    keys.SetSize(0);
    names.SetSize(0);
}

//returns number of keys currently in the ini
int CIniFile::GetNumKeys()
{
    return keys.GetSize();
}

int CIniFile::GetKeysSize(CString& keyname)
{
    int keynum = FindKey(keyname);
    return keys[keynum].names.GetSize();
}

CString CIniFile::EnumKeysValue(CString& keyname,int valuenum)
{
    int keynum = FindKey(keyname);
    return keys[keynum].names[valuenum];
}

//returns number of values stored for specified key, or -1 if key found
int CIniFile::GetNumValues(CString keyname)
{
    int keynum = FindKey(keyname);
    if (keynum == -1)
        return -1;
    else
        return keys[keynum].names.GetSize();
}

//gets value of [keyname] valuename =
//overloaded to return CString, int, and double
CString CIniFile::GetValue(CString keyname, CString valuename)
{
    int keynum = FindKey(keyname), valuenum = FindValue(keynum,valuename);

    if (keynum == -1) {
        error = "Unable to locate specified key.";
        return _T("");
    }

    if (valuenum == -1) {
        error = "Unable to locate specified value.";
        return _T("");
    }
    return keys[keynum].values[valuenum];
}

//gets value of [keyname] valuename =
//overloaded to return CString, int, and double
int CIniFile::GetValueI(CString keyname, CString valuename)
{
    char buffer[BUFFER_SIZE];
    wcstombs ( buffer,(wchar_t *)GetValue(keyname,valuename).GetBuffer(), sizeof(buffer) );
    return atoi(buffer);
}

//gets value of [keyname] valuename =
//overloaded to return CString, int, and double
double CIniFile::GetValueF(CString keyname, CString valuename)
{
    char buffer[BUFFER_SIZE];
    wcstombs ( buffer,(wchar_t *)GetValue(keyname, valuename).GetBuffer(), sizeof(buffer) );
    return atof(buffer);
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CIniFile::SetValue(CString keyname, CString valuename, CString value, bool create)
{
    int keynum = FindKey(keyname), valuenum = 0;
    CString zTmp;
    //find key
    if (keynum == -1) { //if key doesn't exist
        if (!create) //and user does not want to create it,
            return 0; //stop entering this key
        names.SetSize(names.GetSize()+1);
        keys.SetSize(keys.GetSize()+1);
        keynum = names.GetSize()-1;
        names[keynum] = keyname;
        zTmp=names[keynum];
    }

    //find value
    valuenum = FindValue(keynum,valuename);
    if (valuenum == -1) {
        if (!create)
            return 0;
        keys[keynum].names.SetSize(keys[keynum].names.GetSize()+1);
        keys[keynum].values.SetSize(keys[keynum].names.GetSize()+1);
        valuenum = keys[keynum].names.GetSize()-1;
        keys[keynum].names[valuenum] = valuename;
    }
    keys[keynum].values[valuenum] = value;
    return 1;
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CIniFile::SetValueI(CString keyname, CString valuename, int value, bool create)
{
    CString temp;
    temp.Format(_T("%d"),value);
    return SetValue(keyname, valuename, temp, create);
}

//sets value of [keyname] valuename =.
//specify the optional paramter as false (0) if you do not want it to create
//the key if it doesn't exist. Returns true if data entered, false otherwise
//overloaded to accept CString, int, and double
bool CIniFile::SetValueF(CString keyname, CString valuename, double value, bool create)
{
    CString temp;
    temp.Format(_T("%e"),value);
    return SetValue(keyname, valuename, temp, create);
}

//deletes specified value
//returns true if value existed and deleted, false otherwise
bool CIniFile::DeleteValue(CString keyname, CString valuename)
{
    int keynum = FindKey(keyname), valuenum = FindValue(keynum,valuename);
    if (keynum == -1 || valuenum == -1)
        return 0;

    keys[keynum].names.RemoveAt(valuenum);
    keys[keynum].values.RemoveAt(valuenum);
    return 1;
}

//deletes specified key and all values contained within
//returns true if key existed and deleted, false otherwise
bool CIniFile::DeleteKey(CString keyname)
{
    int keynum = FindKey(keyname);
    if (keynum == -1)
        return 0;
    keys.RemoveAt(keynum);
    names.RemoveAt(keynum);
    return 1;
}

/////////////////////////////////////////////////////////////////////
// Private Functions
/////////////////////////////////////////////////////////////////////

//returns index of specified key, or -1 if not found
int CIniFile::FindKey(CString keyname)
{
    int keynum = 0;
    while ( keynum < keys.GetSize() && names[keynum] != keyname)
        keynum++;
    if (keynum == keys.GetSize())
        return -1;
    return keynum;
}

int CIniFile::FindKeyX(CString keyname)
{
    int keynum = 0,index=0;
    char buf1[BUFFER_SIZE],buf2[BUFFER_SIZE],buf3[BUFFER_SIZE];
    while ( keynum < keys.GetSize()) {
        wcstombs ( buf1,(wchar_t *)keyname.GetBuffer(), sizeof(buf1) );
        wcstombs ( buf2,(wchar_t *)(names[keynum]).GetBuffer(), sizeof(buf2) );
        if(!strncmp(buf1,buf2,5)) {
            wcstombs ( buf3,(wchar_t *) (names[keynum].Mid(5)).GetBuffer(), sizeof(buf3) );
            index|=1<<atoi(buf3);
        }
        keynum++;
    }

    return index;
}

//returns index of specified value, in the specified key, or -1 if not found
int CIniFile::FindValue(int keynum, CString valuename)
{
    if (keynum == -1)
        return -1;
    int valuenum = 0;
    while (valuenum < keys[keynum].names.GetSize() && keys[keynum].names[valuenum] != valuename)
        valuenum++;
    if (valuenum == keys[keynum].names.GetSize())
        return -1;
    return valuenum;
}

//overloaded from original getline to take CString
istream & CIniFile:: getline(istream & is, CString & str)
{
    char buf[2048];
    is.getline(buf,2048);
    str = buf;
    return is;
}
