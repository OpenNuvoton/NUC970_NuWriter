#ifndef __ENUMSER_H__
#define __ENUMSER_H__


///////////////////////// Includes ////////////////////////////////////////////                      

#ifndef __AFXTEMPL_H__
#pragma message("To avoid this message put afxtempl.h in your PCH")                                                                                
#include <afxtempl.h>
#endif



///////////////////////// Classes /////////////////////////////////////////////


class CEnumerateSerial
{
public:
#ifndef NO_ENUMSERIAL_USING_CREATEFILE
  static BOOL UsingCreateFile(CUIntArray& ports);
#endif

#ifndef NO_ENUMSERIAL_USING_QUERYDOSDEVICE
  static BOOL UsingQueryDosDevice(CUIntArray& ports);
#endif

#ifndef NO_ENUMSERIAL_USING_GETDEFAULTCOMMCONFIG
  static BOOL UsingGetDefaultCommConfig(CUIntArray& ports);
#endif

#ifndef NO_ENUMSERIAL_USING_SETUPAPI
  static BOOL UsingSetupAPI(CUIntArray& ports, CStringArray& sFriendlyNames);
#endif

#ifndef NO_ENUMSERIAL_USING_ENUMPORTS
  static BOOL UsingEnumPorts(CUIntArray& ports);
#endif

#ifndef NO_ENUMSERIAL_USING_WMI
  static BOOL UsingWMI(CUIntArray& ports);
#endif

protected:
  static BOOL IsNumeric(LPCTSTR pszString, BOOL bIgnoreColon);
};

#endif //__ENUMSER_H__