// Borland C++ Builder
// Copyright (c) 1995, 1999 by Borland International
// All rights reserved

// (DO NOT EDIT: machine generated header) 'OvalBtn.pas' rev: 5.00

#ifndef OvalBtnHPP
#define OvalBtnHPP

#pragma delphiheader begin
#pragma option push -w-
#pragma option push -Vx
#include <Menus.hpp>	// Pascal unit
#include <Messages.hpp>	// Pascal unit
#include <SysUtils.hpp>	// Pascal unit
#include <Controls.hpp>	// Pascal unit
#include <Graphics.hpp>	// Pascal unit
#include <Classes.hpp>	// Pascal unit
#include <Windows.hpp>	// Pascal unit
#include <SysInit.hpp>	// Pascal unit
#include <System.hpp>	// Pascal unit

//-- user supplied -----------------------------------------------------------

namespace Ovalbtn
{
//-- type declarations -------------------------------------------------------
typedef Shortint TNumGlyphs;

typedef Shortint TBevelWidth;

#pragma option push -b-
enum TButtonLayout { blGlyphBottom, blGlyphLeft, blGlyphRight, blGlyphTop };
#pragma option pop

class DELPHICLASS TOvalButton;
class PASCALIMPLEMENTATION TOvalButton : public Controls::TGraphicControl 
{
	typedef Controls::TGraphicControl inherited;
	
private:
	TBevelWidth FBevelWidth;
	Graphics::TColor FColor;
	Graphics::TColor FColorHighlight;
	Graphics::TColor FColorShadow;
	bool FDown;
	bool IsDown;
	bool FFlat;
	Graphics::TFont* FFont;
	Graphics::TBitmap* FGlyph;
	int FGroupIndex;
	TButtonLayout FLayout;
	int FMargin;
	TNumGlyphs FNumGlyphs;
	int FSpacing;
	int FState;
	bool FTransparent;
	Graphics::TColor FTransparentColor;
	bool FMouseDown;
	bool FMouseInside;
	Classes::TNotifyEvent FOnClick;
	Classes::TNotifyEvent FOnDblClick;
	Classes::TNotifyEvent FOnMouseEnter;
	Classes::TNotifyEvent FOnMouseExit;
	Controls::TMouseEvent FOnMouseDown;
	Controls::TMouseMoveEvent FOnMouseMove;
	Controls::TMouseEvent FOnMouseUp;
	HIDESBASE MESSAGE void __fastcall CMMouseEnter(Messages::TMessage &Message);
	HIDESBASE MESSAGE void __fastcall CMMouseLeave(Messages::TMessage &Message);
	
protected:
	virtual void __fastcall Paint(void);
	DYNAMIC void __fastcall Click(void);
	DYNAMIC void __fastcall DblClick(void);
	DYNAMIC void __fastcall MouseDown(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, 
		int Y);
	DYNAMIC void __fastcall MouseMove(Classes::TShiftState Shift, int X, int Y);
	DYNAMIC void __fastcall MouseUp(Controls::TMouseButton Button, Classes::TShiftState Shift, int X, int 
		Y);
	bool __fastcall IsInsideButton(int X, int Y);
	void __fastcall SetBevelWidth(TBevelWidth newValue);
	HIDESBASE void __fastcall SetColor(Graphics::TColor newColor);
	void __fastcall SetDown(bool newValue);
	void __fastcall SetFlat(bool newValue);
	HIDESBASE void __fastcall SetFont(Graphics::TFont* newFont);
	void __fastcall SetGlyph(Graphics::TBitmap* newGlyph);
	void __fastcall SetLayout(TButtonLayout newLayout);
	void __fastcall SetMargin(int newValue);
	void __fastcall SetNumGlyphs(TNumGlyphs newNumGlyphs);
	void __fastcall SetSpacing(int newValue);
	void __fastcall SetTransparent(bool newValue);
	void __fastcall SetTransparentColor(Graphics::TColor newColor);
	void __fastcall PaintBorder(void);
	void __fastcall PaintButton(void);
	
public:
	__fastcall virtual TOvalButton(Classes::TComponent* AOwner);
	__fastcall virtual ~TOvalButton(void);
	MESSAGE void __fastcall CMTextChanged(Messages::TMessage &msg);
	MESSAGE void __fastcall CMDialogChar(Messages::TWMKey &Message);
	
__published:
	__property Action ;
	__property Anchors ;
	__property TBevelWidth BevelWidth = {read=FBevelWidth, write=SetBevelWidth, nodefault};
	__property Caption ;
	__property Graphics::TColor Color = {read=FColor, write=SetColor, nodefault};
	__property bool Down = {read=FDown, write=SetDown, nodefault};
	__property Enabled ;
	__property bool Flat = {read=FFlat, write=SetFlat, nodefault};
	__property Graphics::TFont* Font = {read=FFont, write=SetFont};
	__property Graphics::TBitmap* Glyph = {read=FGlyph, write=SetGlyph};
	__property int GroupIndex = {read=FGroupIndex, write=FGroupIndex, nodefault};
	__property TButtonLayout Layout = {read=FLayout, write=SetLayout, nodefault};
	__property int Margin = {read=FMargin, write=SetMargin, nodefault};
	__property TNumGlyphs NumGlyphs = {read=FNumGlyphs, write=SetNumGlyphs, default=0};
	__property ParentFont ;
	__property ParentShowHint ;
	__property PopupMenu ;
	__property ShowHint ;
	__property int Spacing = {read=FSpacing, write=SetSpacing, nodefault};
	__property bool Transparent = {read=FTransparent, write=SetTransparent, nodefault};
	__property Graphics::TColor TransparentColor = {read=FTransparentColor, write=SetTransparentColor, 
		nodefault};
	__property Visible ;
	__property Classes::TNotifyEvent OnClick = {read=FOnClick, write=FOnClick};
	__property Classes::TNotifyEvent OnDblClick = {read=FOnDblClick, write=FOnDblClick};
	__property Controls::TMouseEvent OnMouseDown = {read=FOnMouseDown, write=FOnMouseDown};
	__property Controls::TMouseMoveEvent OnMouseMove = {read=FOnMouseMove, write=FOnMouseMove};
	__property Controls::TMouseEvent OnMouseUp = {read=FOnMouseUp, write=FOnMouseUp};
	__property Classes::TNotifyEvent OnMouseEnter = {read=FOnMouseEnter, write=FOnMouseEnter};
	__property Classes::TNotifyEvent OnMouseExit = {read=FOnMouseExit, write=FOnMouseExit};
};


//-- var, const, procedure ---------------------------------------------------
extern PACKAGE void __fastcall Register(void);

}	/* namespace Ovalbtn */
#if !defined(NO_IMPLICIT_NAMESPACE_USE)
using namespace Ovalbtn;
#endif
#pragma option pop	// -w-
#pragma option pop	// -Vx

#pragma delphiheader end.
//-- end unit ----------------------------------------------------------------
#endif	// OvalBtn
