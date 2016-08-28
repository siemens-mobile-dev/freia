// Borland C++ Builder
// Copyright (c) 1995, 1999 by Borland International
// All rights reserved

// (DO NOT EDIT: machine generated header) 'UPlasmaEd.pas' rev: 5.00

#ifndef UPlasmaEdHPP
#define UPlasmaEdHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
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

namespace Uplasmaed
{
//-- type declarations -------------------------------------------------------
class DELPHICLASS TPlasmaRegion;
class PASCALIMPLEMENTATION TPlasmaRegion : public Classes::TPersistent 
{
	typedef Classes::TPersistent inherited;
	
private:
	HRGN FRegion;
	Extctrls::TImage* FOwner;
	void __fastcall SetRegion(HRGN Value);
	
protected:
	void __fastcall ReadMask(Classes::TStream* Reader);
	void __fastcall WriteMask(Classes::TStream* Writer);
	virtual void __fastcall DefineProperties(Classes::TFiler* Filer);
	
public:
	void __fastcall SetRegionByData(Windows::PRgnData Value, int Size);
	__fastcall TPlasmaRegion(Extctrls::TImage* AOwner);
	__fastcall virtual ~TPlasmaRegion(void);
	__property Extctrls::TImage* Owner = {read=FOwner};
	__property HRGN Region = {read=FRegion, write=SetRegion, nodefault};
};


class DELPHICLASS TPlasmaMaskProperty;
class PASCALIMPLEMENTATION TPlasmaMaskProperty : public Dsgnintf::TPropertyEditor 
{
	typedef Dsgnintf::TPropertyEditor inherited;
	
private:
	virtual Dsgnintf::TPropertyAttributes __fastcall GetAttributes(void);
	virtual AnsiString __fastcall GetValue(void);
	virtual void __fastcall Edit(void);
protected:
	#pragma option push -w-inl
	/* TPropertyEditor.Create */ inline __fastcall virtual TPlasmaMaskProperty(const Dsgnintf::_di_IFormDesigner 
		ADesigner, int APropCount) : Dsgnintf::TPropertyEditor(ADesigner, APropCount) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TPropertyEditor.Destroy */ inline __fastcall virtual ~TPlasmaMaskProperty(void) { }
	#pragma option pop
	
};


class DELPHICLASS TMaskForm;
class PASCALIMPLEMENTATION TMaskForm : public Forms::TForm 
{
	typedef Forms::TForm inherited;
	
__published:
	Extctrls::TPanel* HPanel;
	Extctrls::TPanel* VPanel;
	Extctrls::TImage* Image;
	Extctrls::TPanel* ColorPanel;
	Stdctrls::TLabel* Label1;
	Buttons::TSpeedButton* BtGo;
	Buttons::TSpeedButton* BtOk;
	Buttons::TSpeedButton* BtCancel;
	Buttons::TSpeedButton* BtSave;
	Dialogs::TSaveDialog* SaveDialog;
	void __fastcall ImageMouseDown(System::TObject* Sender, Controls::TMouseButton Button, Classes::TShiftState 
		Shift, int X, int Y);
	void __fastcall BtOkClick(System::TObject* Sender);
	void __fastcall BtCancelClick(System::TObject* Sender);
	void __fastcall BtGoClick(System::TObject* Sender);
	void __fastcall BtSaveClick(System::TObject* Sender);
	void __fastcall FormCreate(System::TObject* Sender);
	void __fastcall FormDestroy(System::TObject* Sender);
	
private:
	TPlasmaRegion* FRegion;
	void __fastcall SetBitmap(Graphics::TBitmap* Value);
	void __fastcall SetRegion(HRGN Value);
	HRGN __fastcall GetRegion(void);
	
public:
	void __fastcall Empty(void);
	__property HRGN Region = {read=GetRegion, write=SetRegion, nodefault};
	__property Graphics::TBitmap* Bitmap = {write=SetBitmap};
public:
	#pragma option push -w-inl
	/* TCustomForm.Create */ inline __fastcall virtual TMaskForm(Classes::TComponent* AOwner) : Forms::TForm(
		AOwner) { }
	#pragma option pop
	#pragma option push -w-inl
	/* TCustomForm.CreateNew */ inline __fastcall virtual TMaskForm(Classes::TComponent* AOwner, int Dummy
		) : Forms::TForm(AOwner, Dummy) { }
	#pragma option pop
	#pragma option push -w-inl
	/* TCustomForm.Destroy */ inline __fastcall virtual ~TMaskForm(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TMaskForm(HWND ParentWindow) : Forms::TForm(ParentWindow
		) { }
	#pragma option pop
	
};


//-- var, const, procedure ---------------------------------------------------

}	/* namespace Uplasmaed */
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Uplasmaed;
#endif
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// UPlasmaEd
