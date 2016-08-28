// Borland C++ Builder
// Copyright (c) 1995, 1999 by Borland International
// All rights reserved

// (DO NOT EDIT: machine generated header) 'TransparentComponents.pas' rev: 5.00

#ifndef TransparentComponentsHPP
#define TransparentComponentsHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Menus.hpp>	// Pascal unit
#include <ComCtrls.hpp>	// Pascal unit
#include <StdCtrls.hpp>	// Pascal unit
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

namespace Transparentcomponents
{
//-- type declarations -------------------------------------------------------
struct TCMMouseEnter
{
	unsigned Msg;
	int Unused;
	Controls::TControl* Sender;
	int Result;
} ;

typedef TCMMouseEnter  TCMMouseLeave;

typedef void __fastcall (__closure *TMouseEnter)(Controls::TControl* Sender, TCMMouseEnter &Message)
	;

typedef void __fastcall (__closure *TMouseLeave)(Controls::TControl* Sender, TCMMouseEnter &Message)
	;

#pragma option push -b-
enum TTransparentFrameStyle { tfsNone, tfsSingle, tfsLowered, tfsRaised };
#pragma option pop

class DELPHICLASS TCtrl;
class PASCALIMPLEMENTATION TCtrl : public Controls::TWinControl 
{
	typedef Controls::TWinControl inherited;
	
public:
	#pragma option push -w-inl
	/* TWinControl.Create */ inline __fastcall virtual TCtrl(Classes::TComponent* AOwner) : Controls::TWinControl(
		AOwner) { }
	#pragma option pop
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TCtrl(HWND ParentWindow) : Controls::TWinControl(
		ParentWindow) { }
	#pragma option pop
	#pragma option push -w-inl
	/* TWinControl.Destroy */ inline __fastcall virtual ~TCtrl(void) { }
	#pragma option pop
	
};


class DELPHICLASS TTransparentEdit;
class PASCALIMPLEMENTATION TTransparentEdit : public Stdctrls::TEdit 
{
	typedef Stdctrls::TEdit inherited;
	
private:
	Classes::TAlignment FAlignText;
	bool FTransparent;
	bool FPainting;
	void __fastcall SetAlignText(Classes::TAlignment Value);
	void __fastcall SetTransparent(bool Value);
	HIDESBASE MESSAGE void __fastcall WMPaint(Messages::TWMPaint &Message);
	HIDESBASE MESSAGE void __fastcall WMNCPaint(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMEraseBkGnd(Messages::TWMEraseBkgnd &Message);
	MESSAGE void __fastcall CNCtlColorEdit(Messages::TWMCtlColor &Message);
	MESSAGE void __fastcall CNCtlColorStatic(Messages::TWMCtlColor &Message);
	HIDESBASE MESSAGE void __fastcall CMParentColorChanged(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMSize(Messages::TWMSize &Message);
	HIDESBASE MESSAGE void __fastcall WMMove(Messages::TWMMove &Message);
	
protected:
	void __fastcall RepaintWindow(void);
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
	DYNAMIC void __fastcall Change(void);
	virtual void __fastcall SetParent(Controls::TWinControl* AParent);
	
public:
	__fastcall virtual TTransparentEdit(Classes::TComponent* AOwner);
	__fastcall virtual ~TTransparentEdit(void);
	
__published:
	__property Classes::TAlignment AlignText = {read=FAlignText, write=SetAlignText, default=0};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, default=0};
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TTransparentEdit(HWND ParentWindow) : Stdctrls::TEdit(
		ParentWindow) { }
	#pragma option pop
	
};


class DELPHICLASS TTransparentMemo;
class PASCALIMPLEMENTATION TTransparentMemo : public Stdctrls::TMemo 
{
	typedef Stdctrls::TMemo inherited;
	
private:
	Classes::TAlignment FAlignText;
	bool FTransparent;
	bool FPainting;
	void __fastcall SetAlignText(Classes::TAlignment Value);
	void __fastcall SetTransparent(bool Value);
	HIDESBASE MESSAGE void __fastcall WMPaint(Messages::TWMPaint &Message);
	HIDESBASE MESSAGE void __fastcall WMNCPaint(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMEraseBkGnd(Messages::TWMEraseBkgnd &Message);
	MESSAGE void __fastcall CNCtlColorEdit(Messages::TWMCtlColor &Message);
	MESSAGE void __fastcall CNCtlColorStatic(Messages::TWMCtlColor &Message);
	HIDESBASE MESSAGE void __fastcall CMParentColorChanged(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMSize(Messages::TWMSize &Message);
	HIDESBASE MESSAGE void __fastcall WMMove(Messages::TWMMove &Message);
	
protected:
	void __fastcall RepaintWindow(void);
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
	DYNAMIC void __fastcall Change(void);
	virtual void __fastcall SetParent(Controls::TWinControl* AParent);
	
public:
	__fastcall virtual TTransparentMemo(Classes::TComponent* AOwner);
	__fastcall virtual ~TTransparentMemo(void);
	
__published:
	__property Classes::TAlignment AlignText = {read=FAlignText, write=SetAlignText, default=0};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, default=0};
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TTransparentMemo(HWND ParentWindow) : Stdctrls::TMemo(
		ParentWindow) { }
	#pragma option pop
	
};


class DELPHICLASS TTransparentRadioGroup;
class PASCALIMPLEMENTATION TTransparentRadioGroup : public Controls::TGraphicControl 
{
	typedef Controls::TGraphicControl inherited;
	
private:
	bool FTransparent;
	Graphics::TBitmap* FCheckedBmp;
	Graphics::TBitmap* FUnCheckedBmp;
	Classes::TStringList* FItems;
	int FSpace;
	int FItemIndex;
	TTransparentFrameStyle FFrame;
	Classes::TAlignment FCaptionAlignment;
	bool FCaptionTransparent;
	TMouseEnter FMouseEnter;
	TMouseLeave FMouseLeave;
	Classes::TNotifyEvent FOnChange;
	Classes::TNotifyEvent FOnPaint;
	void __fastcall SetCheckedBmp(Graphics::TBitmap* Value);
	void __fastcall SetUnCheckedBmp(Graphics::TBitmap* Value);
	void __fastcall SetTransparent(bool Value);
	void __fastcall SetFrame(TTransparentFrameStyle Value);
	void __fastcall SetCaptionAlignment(Classes::TAlignment Value);
	void __fastcall SetCaptionTransparent(bool Value);
	void __fastcall OwnMouseEnter(void);
	void __fastcall OwnMouseLeave(void);
	void __fastcall DrawFrame(void);
	void __fastcall DrawCaption(void);
	
protected:
	virtual void __fastcall Paint(void);
	HIDESBASE MESSAGE void __fastcall CMMouseEnter(TCMMouseEnter &Message);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(TCMMouseEnter &Message);
	MESSAGE void __fastcall OwnMouseDown(Messages::TMessage &Message);
	DYNAMIC void __fastcall Click(void);
	void __fastcall SetItems(Classes::TStringList* Value);
	void __fastcall SetSpace(int Value);
	void __fastcall SetItemIndex(int Value);
	MESSAGE void __fastcall wmerbkgnd(Messages::TMessage &Msg);
	
public:
	__fastcall virtual TTransparentRadioGroup(Classes::TComponent* AOwner);
	__fastcall virtual ~TTransparentRadioGroup(void);
	
__published:
	int __fastcall ESGetItemIndex(int x, int y);
	__property Graphics::TBitmap* CheckedBmp = {read=FCheckedBmp, write=SetCheckedBmp};
	__property Graphics::TBitmap* UnCheckedBmp = {read=FUnCheckedBmp, write=SetUnCheckedBmp};
	__property Classes::TStringList* Items = {read=FItems, write=SetItems};
	__property int Space = {read=FSpace, write=SetSpace, nodefault};
	__property int ItemIndex = {read=FItemIndex, write=SetItemIndex, nodefault};
	__property Classes::TNotifyEvent OnChange = {read=FOnChange, write=FOnChange};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, nodefault};
	__property Classes::TNotifyEvent OnPaint = {read=FOnPaint, write=FOnPaint};
	__property TTransparentFrameStyle Frame = {read=FFrame, write=SetFrame, nodefault};
	__property Classes::TAlignment CaptionAlign = {read=FCaptionAlignment, write=SetCaptionAlignment, default=0
		};
	__property bool CaptionTransparent = {read=FCaptionTransparent, write=SetCaptionTransparent, nodefault
		};
	__property Caption ;
	__property Color ;
	__property DragCursor ;
	__property DragMode ;
	__property Enabled ;
	__property Font ;
	__property ParentColor ;
	__property ParentFont ;
	__property ParentShowHint ;
	__property PopupMenu ;
	__property ShowHint ;
	__property Visible ;
	__property OnClick ;
	__property OnDragDrop ;
	__property OnDragOver ;
	__property OnEndDrag ;
	__property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
	__property OnStartDrag ;
	__property TMouseEnter OnMouseEnter = {read=FMouseEnter, write=FMouseEnter};
	__property TMouseLeave OnMouseLeave = {read=FMouseLeave, write=FMouseLeave};
};


#pragma option push -b-
enum TCheckControlType { iacctCheck, iacctSelect };
#pragma option pop

#pragma option push -b-
enum TCheckControlStyle { iaccsStyle1, iaccsBitmap, iaccsStyle2, iaccsStyle1Small, iaccsStyle3 };
#pragma option pop

#pragma option push -b-
enum TCheckControlTextHAlign { iaccthaLeft, iaccthaRight, iaccthaCenter };
#pragma option pop

#pragma option push -b-
enum TCheckControlTextVAlign { iacctvaTop, iacctvaBottom, iacctvaCenter };
#pragma option pop

#pragma option push -b-
enum TCheckControlText3DStyle { iscct3dsSimple, iscct3dsUp, iscct3dsDown };
#pragma option pop

typedef void __fastcall (__closure *TCheckControlChangeStateProcedure)(Classes::TPersistent* ASender
	, bool ANewStateSelected);

class DELPHICLASS TCheckControl;
class PASCALIMPLEMENTATION TCheckControl : public Controls::TCustomControl 
{
	typedef Controls::TCustomControl inherited;
	
private:
	int FTextAngle;
	TCheckControlType FControlType;
	Classes::TStringList* FTextLines;
	unsigned FGroupIndex;
	bool FChecked;
	Graphics::TColor FCheckFieldColor1;
	Graphics::TColor FCheckFieldColor2;
	TCheckControlStyle FStyle;
	TCheckControlTextHAlign FTextAlign;
	TCheckControlTextHAlign FTextFieldHorizontalAlign;
	TCheckControlTextVAlign FTextFieldVerticalAlign;
	Controls::TImageList* FImageList;
	Byte FGapTextToImage;
	bool FTransparent;
	bool FShowFocus;
	TCheckControlText3DStyle FText3DStyle;
	TCheckControlChangeStateProcedure FOnChangeState;
	bool __fastcall GetAutoSize(void);
	Graphics::TColor __fastcall GetColor(void);
	Graphics::TFont* __fastcall GetFont(void);
	int __fastcall GetHeight(void);
	HIDESBASE AnsiString __fastcall GetText(void);
	int __fastcall GetWidth(void);
	HIDESBASE void __fastcall SetAutoSize(bool Value);
	HIDESBASE void __fastcall SetColor(Graphics::TColor Value);
	HIDESBASE void __fastcall SetFont(Graphics::TFont* Value);
	HIDESBASE void __fastcall SetHeight(int Value);
	HIDESBASE void __fastcall SetText(AnsiString Value);
	HIDESBASE void __fastcall SetWidth(int Value);
	HIDESBASE MESSAGE void __fastcall WMKeyUp(Messages::TWMKey &Message);
	HIDESBASE MESSAGE void __fastcall WMLButtonDown(Messages::TWMMouse &Message);
	HIDESBASE MESSAGE void __fastcall WMLButtonDblClk(Messages::TWMMouse &Message);
	HIDESBASE MESSAGE void __fastcall WMSIZE(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMSETFOCUS(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall WMKILLFOCUS(Messages::TMessage &Message);
	void __fastcall SetControlType(TCheckControlType Value);
	void __fastcall SetTextAngle(int Value);
	void __fastcall SetTextLines(Classes::TStringList* Value);
	void __fastcall FontIsChange(System::TObject* Sender);
	void __fastcall SetGroupIndex(unsigned Value);
	void __fastcall SetChecked(bool Value);
	void __fastcall SetCheckFieldColor1(Graphics::TColor Value);
	void __fastcall SetCheckFieldColor2(Graphics::TColor Value);
	void __fastcall SetStyle(TCheckControlStyle Value);
	void __fastcall SetTextAlign(TCheckControlTextHAlign Value);
	void __fastcall SetTextFieldHorizontalAlign(TCheckControlTextHAlign Value);
	void __fastcall SetTextFieldVerticalAlign(TCheckControlTextVAlign Value);
	tagSIZE __fastcall GetBMPSize(void);
	tagSIZE __fastcall GetTextSize(void);
	tagSIZE __fastcall GetFullSize(void);
	tagSIZE __fastcall GetBasis(void);
	tagSIZE __fastcall GetOffset(void);
	tagSIZE __fastcall GetCurrOffset(int i);
	tagSIZE __fastcall GetBMPBasis(void);
	void __fastcall RotateFont(Graphics::TFont* AFont, int ADegDeci);
	bool __fastcall GetEnabled1(void);
	void __fastcall SetEnabled1(bool Value);
	void __fastcall DrawCheckBMP(void);
	void __fastcall DrawFocus(void);
	void __fastcall SetGapTextToImage(Byte Value);
	void __fastcall SetTransparent(bool Value);
	void __fastcall SetProperlyRGN(void);
	void __fastcall SetShowFocus(bool Value);
	void __fastcall SetText3DStyle(TCheckControlText3DStyle Value);
	bool __fastcall LessColorDistanceThen(Graphics::TColor C1, Graphics::TColor C2, int ADistance);
	void __fastcall SetOnChangeState(TCheckControlChangeStateProcedure Value);
	bool __fastcall GetVisible(void);
	HIDESBASE void __fastcall SetVisible(bool Value);
	
protected:
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall AdjustSize(void);
	virtual void __fastcall CreateHandle(void);
	
public:
	__fastcall virtual TCheckControl(Classes::TComponent* AOwner);
	__fastcall virtual ~TCheckControl(void);
	virtual void __fastcall SetBounds(int ALeft, int ATop, int AWidth, int AHeight);
	virtual void __fastcall Assign(Classes::TPersistent* Source);
	
__published:
	__property Graphics::TColor Color = {read=GetColor, write=SetColor, nodefault};
	__property Graphics::TFont* Font = {read=GetFont, write=SetFont};
	__property ParentBiDiMode ;
	__property ParentColor ;
	__property ParentFont ;
	__property ParentShowHint ;
	__property AnsiString Text = {read=GetText, write=SetText};
	__property OnClick ;
	__property OnDblClick ;
	__property OnMouseDown ;
	__property OnMouseMove ;
	__property OnMouseUp ;
	__property OnResize ;
	__property bool Enabled = {read=GetEnabled1, write=SetEnabled1, nodefault};
	__property Align ;
	__property Anchors ;
	__property BiDiMode ;
	__property Constraints ;
	__property ShowHint ;
	__property bool Visible = {read=GetVisible, write=SetVisible, nodefault};
	__property int Width = {read=GetWidth, write=SetWidth, nodefault};
	__property int Height = {read=GetHeight, write=SetHeight, nodefault};
	__property TabOrder ;
	__property TabStop ;
	__property OnEnter ;
	__property OnExit ;
	__property OnKeyDown ;
	__property OnKeyPress ;
	__property OnKeyUp ;
	__property TCheckControlType ControlType = {read=FControlType, write=SetControlType, default=0};
	__property int TextAngle = {read=FTextAngle, write=SetTextAngle, default=0};
	__property Classes::TStringList* TextLines = {read=FTextLines, write=SetTextLines};
	__property unsigned GroupIndex = {read=FGroupIndex, write=SetGroupIndex, default=0};
	__property bool Checked = {read=FChecked, write=SetChecked, nodefault};
	__property TCheckControlStyle Style = {read=FStyle, write=SetStyle, default=3};
	__property Controls::TImageList* ImageList = {read=FImageList, write=FImageList, default=0};
	__property TCheckControlTextHAlign TextAlign = {read=FTextAlign, write=SetTextAlign, default=0};
	__property TCheckControlTextVAlign TextFieldVerticalAlign = {read=FTextFieldVerticalAlign, write=SetTextFieldVerticalAlign
		, default=0};
	__property TCheckControlTextHAlign TextFieldHorizontalAlign = {read=FTextFieldHorizontalAlign, write=
		SetTextFieldHorizontalAlign, default=0};
	__property Graphics::TColor CheckFieldColor1 = {read=FCheckFieldColor1, write=SetCheckFieldColor1, 
		default=8421504};
	__property Graphics::TColor CheckFieldColor2 = {read=FCheckFieldColor2, write=SetCheckFieldColor2, 
		default=12632256};
	__property Byte GapTextToImage = {read=FGapTextToImage, write=SetGapTextToImage, default=4};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, default=1};
	__property bool ShowFocus = {read=FShowFocus, write=SetShowFocus, default=1};
	__property TCheckControlText3DStyle Text3DStyle = {read=FText3DStyle, write=SetText3DStyle, default=0
		};
	__property TCheckControlChangeStateProcedure OnChangeState = {read=FOnChangeState, write=SetOnChangeState
		};
	__property bool AutoSize = {read=GetAutoSize, write=SetAutoSize, nodefault};
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TCheckControl(HWND ParentWindow) : Controls::TCustomControl(
		ParentWindow) { }
	#pragma option pop
	
};


class DELPHICLASS TTransparentGroupBox;
class PASCALIMPLEMENTATION TTransparentGroupBox : public Stdctrls::TGroupBox 
{
	typedef Stdctrls::TGroupBox inherited;
	
private:
	Graphics::TColor FBevelLowColor;
	Graphics::TColor FBevelHiColor;
	void __fastcall SetBvlLowColor(Graphics::TColor Value);
	void __fastcall SetBvlHiColor(Graphics::TColor Value);
	HIDESBASE MESSAGE void __fastcall WMEraseBkGnd(Messages::TWMEraseBkgnd &Message);
	HIDESBASE MESSAGE void __fastcall WMMove(Messages::TWMMove &Message);
	
protected:
	virtual void __fastcall CreateParams(Controls::TCreateParams &Params);
	virtual void __fastcall Paint(void);
	virtual void __fastcall SetParent(Controls::TWinControl* AParent);
	void __fastcall DrawFrame(const Windows::TRect &Rect);
	
public:
	__fastcall virtual TTransparentGroupBox(Classes::TComponent* AOwner);
	virtual void __fastcall Invalidate(void);
	
__published:
	__property Graphics::TColor BevelShadowColor = {read=FBevelLowColor, write=SetBvlLowColor, nodefault
		};
	__property Graphics::TColor BevelHighlightColor = {read=FBevelHiColor, write=SetBvlHiColor, nodefault
		};
public:
	#pragma option push -w-inl
	/* TCustomControl.Destroy */ inline __fastcall virtual ~TTransparentGroupBox(void) { }
	#pragma option pop
	
public:
	#pragma option push -w-inl
	/* TWinControl.CreateParented */ inline __fastcall TTransparentGroupBox(HWND ParentWindow) : Stdctrls::TGroupBox(
		ParentWindow) { }
	#pragma option pop
	
};


typedef Shortint TNumGlyphs;

typedef Shortint TBevelWidth;

#pragma option push -b-
enum TButtonLayout { blGlyphBottom, blGlyphLeft, blGlyphRight, blGlyphTop };
#pragma option pop

//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Transparentcomponents */
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Transparentcomponents;
#endif
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// TransparentComponents
