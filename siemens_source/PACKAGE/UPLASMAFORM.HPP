// Borland C++ Builder
// Copyright (c) 1995, 1999 by Borland International
// All rights reserved

// (DO NOT EDIT: machine generated header) 'UPlasmaForm.pas' rev: 5.00

#ifndef UPlasmaFormHPP
#define UPlasmaFormHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <UPlasmaEd.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
#include <Buttons.hpp>	// Pascal unit
#include <DsgnIntf.hpp>	// Pascal unit
#include <ExtCtrls.hpp>	// Pascal unit
#include <Dialogs.hpp>	// Pascal unit
#include <Forms.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Uplasmaform
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TPlasmaForm;
class PASCALIMPLEMENTATION TPlasmaForm : public Extctrls::TImage 
{
	typedef Extctrls::TImage inherited;
	
private:
	bool FMoveable;
	int FMoveX;
	int FMoveY;
	Uplasmaed::TPlasmaRegion* FRegion;
	void __fastcall RefreshForm(void);
	void __fastcall PictureUpdate(System::TObject* Sender);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, 
		int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	
protected:
	void __fastcall SetRegion(Uplasmaed::TPlasmaRegion* Value);
	Uplasmaed::TPlasmaRegion* __fastcall GetRegion(void);
	virtual void __fastcall SetParent(Controls::TWinControl* Value);
	
public:
	__fastcall virtual TPlasmaForm(Classes::TComponent* Aowner);
	__fastcall virtual ~TPlasmaForm(void);
	virtual void __fastcall Loaded(void);
	void __fastcall LoadMaskFromFile(AnsiString Value);
	void __fastcall LoadMaskFromResource(unsigned Instance, AnsiString ResName);
	
__published:
	__property Uplasmaed::TPlasmaRegion* Mask = {read=GetRegion, write=SetRegion};
	__property bool Moveable = {read=FMoveable, write=FMoveable, nodefault};
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Uplasmaform */
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Uplasmaform;
#endif
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// UPlasmaForm
