unit OvalBtn;

{ TOvalButton (C)opyright 2002 Version 1.33
  Autor : Simon Reinhardt
  eMail : reinhardt@picsoft.de
  Internet : http://www.picsoft.de

  Die Komponente TOvalButton ist eine ovale SpeedButton-Komponente. Sie ist
  abgeleteitet von TGraphicControl und ist eine Weiterentwicklung der Komponente
  TRoundButton von Brendan Rempel, der so nett war, diese als Public Domain
  zu veröffentlichen.

  Vielen Dank auch an Marco Lange <marco@marcolange.de> für die Korrektur
  der OnMouseEnter- und OnMouseLeave-Ereignisse.

  Die Komponente TOvalButton ist Public Domain, das Urheberrecht liegt
  aber beim Autor. }

interface

uses
  Windows, Classes, Graphics, Controls, SysUtils, Messages;

type
  TNumGlyphs    = 0..4;
  TBevelWidth   = 1..10;
  TButtonLayout = (blGlyphBottom, blGlyphLeft, blGlyphRight, blGlyphTop);

  TOvalButton = class(TGraphicControl)
  private
    FBevelWidth:       TBevelWidth;
    FColor,
    FColorHighlight,
    FColorShadow:      TColor;
    FDown,IsDown,
    FFlat:             boolean;
    FFont:             TFont;
    FGlyph:            TBitmap;
    FGroupIndex:       integer;
    FLayout:           TButtonLayout;
    FMargin:           integer;
    FNumGlyphs:        TNumGlyphs;
    FSpacing,
    FState:            integer;
    FTransparent:      boolean;
    FTransparentColor: TColor;

    FMouseDown,
    FMouseInside:      boolean;
    FOnClick,
    FOnDblClick,
    FOnMouseEnter,
    FOnMouseExit:      TNotifyEvent;
    FOnMouseDown:      TMouseEvent;
    FOnMouseMove:      TMouseMoveEvent;
    FOnMouseUp:        TMouseEvent;

    procedure CMMouseEnter(var Message: TMessage); message CM_MOUSEENTER;
    procedure CMMouseLeave(var Message: TMessage); message CM_MOUSELEAVE;

  protected
    procedure Paint; override;
    procedure Click; override;
    procedure DblClick; override;
    procedure MouseDown(Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
      override;
    procedure MouseMove(Shift: TShiftState; X, Y: Integer);
      override;
    procedure MouseUp(Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
      override;

    function  IsInsideButton(X,Y: Integer): boolean;

    procedure SetBevelWidth(newValue: TBevelWidth);
    procedure SetColor(newColor: TColor);
    procedure SetDown(newValue: boolean);
    procedure SetFlat(newValue: boolean);
    procedure SetFont(newFont: TFont);
    procedure SetGlyph(newGlyph: TBitmap);
    procedure SetLayout(newLayout: TButtonLayout);
    procedure SetMargin(newValue: integer);
    procedure SetNumGlyphs(newNumGlyphs: TNumGlyphs);
    procedure SetSpacing(newValue: integer);
    procedure SetTransparent(newValue: boolean);
    procedure SetTransparentColor(newColor: TColor);

    procedure PaintBorder;
    procedure PaintButton;

  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;

    procedure CMTextChanged(var msg: TMessage);message CM_TEXTCHANGED;
    procedure CMDialogChar(var Message: TCMDialogChar);message CM_DIALOGCHAR;

  published
    property Action;
    property Anchors;
    property BevelWidth: TBevelWidth read FBevelWidth write SetBevelWidth;
    property Caption;
    property Color: TColor read FColor write SetColor;
    property Down: boolean read FDown write SetDown;
    property Enabled;
    property Flat: boolean read FFlat write SetFlat;
    property Font: TFont read FFont write SetFont;
    property Glyph: TBitmap read FGlyph write SetGlyph;
    property GroupIndex: integer read FGroupIndex write FGroupIndex;
    property Layout: TButtonLayout read FLayout write SetLayout;
    property Margin: integer read FMargin write SetMargin;
    property NumGlyphs: TNumGlyphs read FNumGlyphs write SetNumGlyphs default 0;
    property ParentFont;
    property ParentShowHint;
    property PopupMenu;
    property ShowHint;
    property Spacing: integer read FSpacing write SetSpacing;
    property Transparent: boolean read FTransparent write SetTransparent;
    property TransparentColor: TColor read FTransparentColor write SetTransparentColor;
    property Visible;
    property OnClick: TNotifyEvent read FOnClick write FOnClick;
    property OnDblClick: TNotifyEvent read FOnDblClick write FOnDblClick;
    property OnMouseDown: TMouseEvent read FOnMouseDown write FOnMouseDown;
    property OnMouseMove: TMouseMoveEvent read FOnMouseMove write FOnMouseMove;
    property OnMouseUp: TMouseEvent read FOnMouseUp write FOnMouseUp;
    property OnMouseEnter: TNotifyEvent read FOnMouseEnter write FOnMouseEnter;
    property OnMouseExit: TNotifyEvent read FOnMouseExit  write FOnMouseExit;
  end;

procedure Register;

implementation

const
  DefaultWidth  = 100;
  DefaultHeight = 50;
  FHLContrast   = 5;
  FShContrast   = 4;

function IsAccellerator(VK: Word; const Str: string): Boolean;
var
  P : Integer;
begin
  P := Pos('&', Str);
  Result := (P <> 0) and (P < Length(Str)) and
    (Upcase(Str[P + 1])=Upcase(Char(VK)));
end;

type
  TColorVector = record
    case Integer of
      0: (Coord: array[0..2] of Double);
      1: (R, G, B: Double);
      2: (H, L, S: Double);
  end;

  EColorConv = class(Exception);

function RGB2HLS(const RGB: TColorVector): TColorVector;
const
  Epsilon = 1E-8;
  Components: array[0..2] of string = (
    'Red',
    'Green',
    'Blue');
var
  i, k: Integer;
  x: Double;
  V: TColorVector;
  W: TColorVector absolute Result;
  Hue: Double;
  Sat: Double;
  Lum: Double;

  function GetHue: Double;
  begin
    case k of
      0: if W.G > W.B then Result := 2+(W.B+1)/2
	 else Result := 4-(W.G+1)/2;
      1: if W.B > W.R then Result := 4+(W.R+1)/2
	 else Result := 6-(W.B+1)/2;
      2: if W.R > W.G then Result := (W.G+1)/2
	 else Result := 2-(W.R+1)/2;
      else Result := 0;
    end;
    Result := Result/6;
  end;

begin
  for i := Low(RGB.Coord) to High(RGB.Coord) do
    if (RGB.Coord[i] < 0) or (RGB.Coord[i] > 1) then
      raise EColorConv.CreateFmt('RGB2HLS: 0 <= %s value <= 1 required', [Components[i]]);

  x := 0;
  for i := 0 to 2 do
  begin
    V.Coord[i] := 2*RGB.Coord[i]-1; // [0, 1] -> [-1, 1]
    if Abs(V.Coord[i]) > x then
    begin
      x := Abs(V.Coord[i]);
      k := i;	// index of RGB coordinate most different from 0.5
    end;
  end;
  if x < Epsilon then	// middle grey
  begin
    Result.H := 0;
    Result.L := 0.5; // could be RGB.G or RGB.B as well
    Result.S := 0;
    Exit;
  end
  else x := 1/x;
  for i := 0 to 2 do W.Coord[i] := V.Coord[i] * x;
  x := 0;
  if V.Coord[k] <= 0 then
  begin
    for i := 0 to 2 do if (W.Coord[i]+1) > x then x := W.Coord[i] +1;
    if x < Epsilon then // R = G = B: location on grey axis
    begin
      Result.H := 0;
      Result.L := RGB.R; // could be RGB.G or RGB.B as well
      Result.S := 0;
      Exit;
    end
    else x := 2/x;
    for i := 0 to 2 do W.Coord[i] := x*(W.Coord[i]+1)-1;
    Hue := GetHue;
    // compute saturation
    if Abs(V.G-V.R) > Epsilon then
      Sat := (V.G-V.R)/(W.Coord[1]*(V.R+1)-W.Coord[0]*(V.G+1))
    else if Abs(V.B-V.G) > Epsilon then
      Sat := (V.B-V.G)/(W.Coord[2]*(V.G+1)-W.Coord[1]*(V.B+1))
    else if Abs(V.B-V.R) > Epsilon then
      Sat := (V.B-V.R)/(W.Coord[2]*(V.R+1)-W.Coord[0]*(V.B+1))
    else Sat := 0;
    // compute luminance
    if Abs(W.Coord[1]-W.Coord[0]) > Epsilon then
      Lum := (W.Coord[1]*(V.R+1)-W.Coord[0]*(V.G+1))/(W.Coord[1]-W.Coord[0])
    else if Abs(W.Coord[2]-W.Coord[1]) > Epsilon then
      Lum := (W.Coord[2]*(V.G+1)-W.Coord[1]*(V.B+1))/(W.Coord[2]-W.Coord[1])
    else if Abs(W.Coord[2]-W.Coord[0]) > Epsilon then
      Lum := (W.Coord[2]*(V.R+1)-W.Coord[0]*(V.B+1))/(W.Coord[2]-W.Coord[0])
    else Lum := V.R+1;
    Lum := Lum * 0.5;
  end else
  begin
    for i := 0 to 2 do if (1-W.Coord[i]) > x then x := 1-W.Coord[i];
    if x < Epsilon then // R = G = B: location on grey axis
    begin
      Result.H := 0;
      Result.L := RGB.R; // could be RGB.G or RGB.B as well
      Result.S := 0;
      Exit;
    end
    else x := 2/x;
    for i := 0 to 2 do W.Coord[i] := x*(W.Coord[i]-1)+1;
    x := 1;
    for i := 0 to 2 do
    if W.Coord[i] < x then
    begin
      x := W.Coord[i];
      k := i;
    end;
    Hue := GetHue;
    // compute saturation
    if Abs(V.G-V.R) > Epsilon then
      Sat := (V.G-V.R)/(W.Coord[0]*(V.G-1)-W.Coord[1]*(V.R-1))
    else if Abs(V.B-V.G) > Epsilon then
      Sat := (V.B-V.G)/(W.Coord[1]*(V.B-1)-W.Coord[2]*(V.G-1))
    else if Abs(V.B-V.R) > Epsilon then
      Sat := (V.B-V.R)/(W.Coord[0]*(V.B-1)-W.Coord[2]*(V.R-1))
    else Sat := 0;
    // compute luminance
    if Abs(W.Coord[1]-W.Coord[0]) > Epsilon then
      Lum := (W.Coord[1]*(V.R-1)-W.Coord[0]*(V.G-1))/(W.Coord[1]-W.Coord[0])
    else if Abs(W.Coord[2]-W.Coord[1]) > Epsilon then
      Lum := (W.Coord[2]*(V.G-1)-W.Coord[1]*(V.B-1))/(W.Coord[2]-W.Coord[1])
    else if Abs(W.Coord[2]-W.Coord[0]) > Epsilon then
      Lum := (W.Coord[2]*(V.R-1)-W.Coord[0]*(V.B-1))/(W.Coord[2]-W.Coord[0])
    else Lum := V.R-1;
    Lum := 1 + Lum * 0.5;
  end;
  W.H := Hue;
  W.L := Lum;
  W.S := Sat;

  for i := 0 to 2 do
    if W.Coord[i] < 0 then W.Coord[i] := 0 else
    if W.Coord[i] > 1 then W.Coord[i] := 1;
end;

function HLS2RGB(const HLS: TColorVector): TColorVector;
const
  Hue: array[0..5, 0..2] of Integer = (
	(1, -1, -1),	// red
	(1, 1, -1),	// yellow
	(-1, 1, -1),	// green
	(-1, 1, 1),	// cyan
	(-1, -1, 1),	// blue
	(1, -1, 1));	// magenta
  Components: array[0..2] of string = (
	'Hue',
	'Luminance',
	'Saturation');
var
  i, j, k: Integer;
  x: Double;
begin
  for i := Low(HLS.Coord) to High(HLS.Coord) do
    if (HLS.Coord[i] < 0) or (HLS.Coord[i] > 1) then
      raise EColorConv.CreateFmt('HLS2RGB: 0 <= %s value <= 1 required', [Components[i]]);

  j := Trunc(HLS.H * 6) mod 6;
  k := (j+1) mod 6;
  x := Frac(HLS.H * 6);
  for i := 0 to 2 do
    Result.Coord[i] := Hue[j, i] + x*(Hue[k, i]-Hue[j, i]);
  for i := 0 to 2 do Result.Coord[i] := Result.Coord[i] * HLS.S;
  if HLS.L <= 0.5 then
    for i := 0 to 2 do Result.Coord[i] := HLS.L * (Result.Coord[i]+ 1)
  else
    for i := 0 to 2 do Result.Coord[i] := HLS.L + Result.Coord[i] * (1-HLS.L);

  for i := 0 to 2 do
    if Result.Coord[i] < 0 then Result.Coord[i] := 0 else
    if Result.Coord[i] > 1 then Result.Coord[i] := 1;
end;

procedure Get3DColors(FaceColor: TColor; var HighLightColor, ShadowColor: TColor; HLFactor, ShFactor: single);
var
  V,HLS : TColorVector;
  R,G,B : Byte;
begin
  FaceColor := ColorToRGB(FaceColor);
  R := GetRValue(FaceColor);
  G := GetGValue(FaceColor);
  B := GetBValue(FaceColor);
  HighLightColor := RGB(
	255-round((256-R) * HLFactor),
	255-round((256-G) * HLFactor),
	255-round((256-B) * HLFactor));
  V.R := R/255;
  V.G := G/255;
  V.B := B/255;
  HLS := RGB2HLS(V);
  HLS.L := HLS.L * ShFactor;	// Luminance := Luminance * Shadowfactor
  V := HLS2RGB(HLS);
  ShadowColor := RGB(
        Round(V.R*255),
        Round(V.G*255),
        Round(V.B*255));
end;

procedure AssignBevelColors(FaceColor:TColor;var HighlightColor,ShadowColor:TColor;HLContrast,ShContrast:integer);
begin
  Get3DColors(FaceColor,HighlightColor,ShadowColor,(10-HLContrast)/10,(10-ShContrast)/10);
end;


{ Komponente TOvalButton }
constructor TOvalButton.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);

  {Vorgabewerte setzen}
  FColor:=clBtnFace;
  AssignBevelColors(FColor,FColorHighlight,FColorShadow,FHLContrast,FShContrast);
  FDown:=false;
  FFlat:=false;
  FFont:=TFont.Create;
  FGlyph:=TBitmap.Create;
  FGroupIndex:=0;
  FNumGlyphs:=0;
  FSpacing :=4;
  FState:=1;
  FTransparent:=false;
  Height:=DefaultHeight;
  Width:=DefaultWidth;

  FMouseInside:=False;
  FMouseDown:=False;
end;

destructor TOvalButton.Destroy;
begin
  FFont.Free;
  FGlyph.Free;
  inherited Destroy;
end;

procedure TOvalButton.Click;
begin
  if Enabled and FMouseInside then
    if Assigned(FOnClick) then
      FOnClick(Self);
end;

procedure TOvalButton.CMDialogChar(var Message: TCMDialogChar);
begin
  with Message do begin
    if IsAccellerator(CharCode, Caption) then begin
      if Enabled then
        Click;
      Result:=1;
    end
    else
      inherited;
  end;
end;

procedure TOvalButton.CMMouseEnter(var Message: TMessage);
begin
  inherited;
  if assigned(FOnMouseEnter) then
    FOnMouseEnter(self);
end;

procedure TOvalButton.CMMouseLeave(var Message: TMessage);
begin
  inherited;
  if FMouseInside and Enabled then begin
    FMouseInside:=False;
    if FFlat and not FDown then begin
      FState:=0;
      Invalidate;
    end;
  end;
  if assigned(FOnMouseExit) then
    FOnMouseExit(Self);
end;

procedure TOvalButton.CMTextChanged(var msg: TMessage);
begin
  Invalidate;
end;

procedure TOvalButton.DblClick;
begin
  if Enabled and FMouseInside then
    if Assigned(FOnDblClick) then
      FOnDblClick(Self);
end;

function TOvalButton.IsInsideButton(X,Y: Integer):boolean;
var BtnEllipse : HRgn;
begin
  { Ist die Maus über der Button-Ellipse? }
  BtnEllipse:=CreateEllipticRgn(0,0,Width,Height);
  Result:=PtInRegion(BtnEllipse,X,Y);
  DeleteObject(BtnEllipse);
end;

procedure TOvalButton.Paint;
begin
  Canvas.Font.Assign(Font);
  with Canvas do begin
     if FTransparent then
       brush.Style:=bsClear
     else begin
       Brush.Style:=bsSolid;
       brush.color:=Color;
     end;
     if FFlat then
       pen.Style:=psClear
     else begin
       pen.Style:=psSolid;
       pen.color:=clBlack;
     end;
     { Button mit Farbe füllen, schwarzen Rand zeichnen }
     Pen.Width:=1;
     {Ellipse(0,0,width-1,height-1);}
  end;

  { Den Rest zeichnen }
  PaintButton;
end;

procedure TOvalButton.MouseDown(Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if (Enabled and IsInsideButton(X,Y)) then begin
    IsDown:=FDown;
    { Im gedrückten Zustand neu zeichnen }
    FDown:=true;
    FState:=-1;
    if FTransparent then
      Invalidate
    else
      PaintButton;
    if Assigned(FOnMouseDown) then
      FOnMouseDown(Self, Button, Shift, X, Y);
  end;
  FMouseDown:= True;
end;

procedure TOvalButton.MouseUp(Button: TMouseButton; Shift: TShiftState; X, Y: Integer);
begin
  if (Enabled and IsInsideButton(X,Y)) then begin
    { Im ungedrückten Zustand neu zeichnen }
    if GroupIndex=0 then begin
      FDown:=false;
      FState:=1;
      if FTransparent then
        Invalidate
      else
        PaintButton;
    end
    else
      SetDown(not IsDown);
    { OnClick-Ereignis abfeuern }
    if Assigned(FOnMouseUp) then
      FOnMouseUp(Self, Button, Shift, X, Y);
  end;
  FMouseDown:= False;
end;

procedure TOvalButton.MouseMove(Shift: TShiftState; X, Y: Integer);
begin
  if FMouseDown then begin
    if not IsInsideButton(X,Y) then begin
      { Button.Down aufheben, falls Maus Button verlässt }
      if (FState=-1) and (GroupIndex=0) and FMouseInside then begin
        FDown:=false;
        FState:=1;
        PaintBorder;
      end;
      FMouseInside:=false;
    end
    else begin
      { Button.Down setzen, falls Maus über Button }
      if (FState=1) and (GroupIndex=0) then begin
        FMouseInside:=true;
        FDown:=true;
        FState:=-1;
        PaintBorder;
      end;
    end;
  end
  else begin
    if FFlat and not FDown then begin
      if not IsInsideButton(X,Y) then begin
        { Button flach }
        if FMouseInside then begin
          FMouseInside:=false;
          FState:=0;
          Invalidate;
        end;
      end
      else begin
        { Button erhaben }
        FMouseInside:=true;
        FState:=1;
        PaintBorder;
      end;
    end
    else
      FMouseInside:=IsInsideButton(X,Y);
  end;
  if FMouseInside then
    if Assigned(FOnMouseMove) then
      FOnMouseMove(Self, Shift, X, Y);
end;

procedure TOvalButton.PaintBorder;
var i : integer;
begin
  with Canvas do begin
    Pen.Style:=psSolid;
    { linke obere Ecke zeichnen }
    if FState=-1 then  {down}
      Pen.Color:=FColorShadow;
    if FState=1 then  {up}
      Pen.Color:=FColorHighlight;
    if FState=0 then  {flat}
      Pen.Style:=psClear;
    for i:=1 to FBevelWidth do
      Arc(i, i, Width-i-1 , Height-i-1,
          Width div 5 * 4, Height div 5,
          Width div 5, Height div 5 * 4);
    { rechte untere Ecke zeichnen }
    if FState=-1 then  {down}
      Pen.Color:=FColorHighlight;
    if FState=1 then   {up}
      Pen.Color:=FColorShadow;
    for i:=1 to FBevelWidth do
      Arc(i, i, Width-i-1, Height-i-1,
          Width div 5, Height div 5 * 4,
          Width div 5 * 4, Height div 5);
    Pen.Style:=psSolid;
  end;
end;

procedure TOvalButton.PaintButton;
var
  Dest,
  Source,
  TextR     : TRect;
  outWidth,
  outHeight,
  TextLeft,
  TextTop   : integer;
  outText   : array [0..79] of char;
begin
  { Glyph zeichnen ? }
  if Assigned(FGlyph) then begin
     with Source do begin
       { Source-Rechteck ermitteln }
       Left:=0;
       Right:=FGlyph.Width;
       Top:=0;
       Bottom:=FGlyph.Height;
       if FNumGlyphs>0 then
         Right:=Right div FNumGlyphs;
     end;
  end;

  PaintBorder;

  with Canvas do begin
    { Glyph anzeigen }
    outWidth:=0;
    outHeight:=0;
    if Assigned(FGlyph) and (FNumGlyphs>0) then begin
      if(Not Enabled and (FNumGlyphs>1)) then begin
        { disabled button }
        Source.Left:=FGlyph.width div FNumGlyphs;
        Source.Right:=Source.Left shl 1;
      end;
      { Größe des Destination-Rechtecks }
      outWidth:=Source.Right-Source.Left;
      outHeight:=Source.Bottom-Source.Top;
      { Glyph-Position ermitteln }
      if (Caption='') or (FLayout=blGlyphTop) or (FLayout=blGlyphBottom) then begin
        Dest.Left:=((Width-outWidth) shr 1);
        Dest.Right:=((Width-outWidth) shr 1)+outWidth;
      end;
      if (Caption<>'') and (FLayout=blGlyphLeft) then begin
        Dest.Left:=((Width-(outWidth+FSpacing+TextWidth(Caption))) shr 1)-FMargin;
        Dest.Right:=Dest.Left+outWidth;
      end;
      if (Caption<>'') and (FLayout=blGlyphRight) then begin
        Dest.Left:=((Width+(outWidth+FSpacing+TextWidth(Caption))) shr 1)-outWidth+FMargin;
        Dest.Right:=Dest.Left+outWidth;
      end;
      if (Caption='') or (FLayout=blGlyphLeft) or (FLayout=blGlyphRight) then begin
        Dest.Top:=((Height-outHeight) shr 1);
        Dest.Bottom:=((Height-outHeight) shr 1)+outHeight;
      end;
      if (Caption<>'') and (FLayout=blGlyphTop) then begin
        Dest.Top:=((Height-(outHeight+FSpacing+TextHeight(Caption))) shr 1)-FMargin;
        Dest.Bottom:=Dest.Top+outHeight;
      end;
      if (Caption<>'') and (FLayout=blGlyphBottom) then begin
        Dest.Top:=((Height+(outHeight+FSpacing+TextHeight(Caption))) shr 1)-outHeight+FMargin;
        Dest.Bottom:=Dest.Top+outHeight;
      end;
      if FTransparent then
        Pen.Style:=psClear
      else begin
        Pen.Style:=psSolid;
        Pen.Color:=Color;
      end;
      if FState=-1 then begin {down}
        { Glyph um 1 Pixel nach rechts unten verschieben }
        Inc(Dest.Left);
        Inc(Dest.Right);
        Inc(Dest.Top);
        Inc(Dest.Bottom);
        { verbleibende Up-Reste löschen }
        MoveTo(Dest.Left-1,Dest.Bottom);
        LineTo(Dest.Left-1,Dest.Top-1);
        LineTo(Dest.Right,Dest.Top-1);
      end
      else begin
        { verbleibende Down-Reste löschen }
        MoveTo(Dest.Right,Dest.Top);
        LineTo(Dest.Right,Dest.Bottom);
        LineTo(Dest.Left,Dest.Bottom);
      end;
      Pen.Style := psSolid;
      if ((FState=-1) and (FNumGlyphs>2)) then begin
        { Glyph für gedrückten Zustand bestimmen }
        Source.Left:=FGlyph.width div FNumGlyphs * 2;
        Source.Right:=FGlyph.width div FNumGlyphs * 3;
      end;
      if FTransparent then
        Brush.Style:=bsClear
      else begin
        Brush.Style:=bsSolid;
        Brush.Color:=Color;
      end;

      { Glyph zeichnen }
      BrushCopy(Dest, FGlyph, Source, FTransparentColor);
    end;

    { Caption zeichnen }
    if Caption<>'' then begin
      {Position ermitteln}
      TextLeft:=(width-TextWidth(Caption)) shr 1;
      if Assigned(FGlyph) and (FNumGlyphs>0) and (FLayout=blGlyphRight) then
        TextLeft:=Dest.Left-TextWidth(Caption)-FSpacing;
      if Assigned(FGlyph) and (FNumGlyphs>0) and (FLayout=blGlyphLeft) then
        TextLeft:=Dest.Left+outWidth+FSpacing;
      TextTop:=(height-TextHeight(Caption)) shr 1;
      if Assigned(FGlyph) and (FNumGlyphs>0) and (FLayout=blGlyphTop) then
        TextTop:=Dest.Top+outHeight+FSpacing;
      if Assigned(FGlyph) and (FNumGlyphs>0) and (FLayout=blGlyphBottom) then
        TextTop:=Dest.Top-TextHeight(Caption)-FSpacing;
      if FState=-1 then begin
        inc(TextTop);
        inc(TextLeft);
      end;
      {Text ausgeben}
      if FTransparent then
        Brush.Style:=bsClear
      else begin
        Brush.Style:=bsSolid;
        Brush.Color:=Color;
      end;
      if FState=-1 then
        { verbleibende Up-Reste löschen }
        FillRect(Rect(TextLeft,
                      TextTop,
                      TextLeft+TextWidth(Caption),
                      TextTop+TextHeight(Caption)))
      else
        { verbleibende Down-Reste löschen }
        FillRect(Rect(TextLeft+1,
                      TextTop+1,
                      TextLeft+1+TextWidth(Caption),
                      TextTop+1+TextHeight(Caption)));
      TextR:=Rect(TextLeft,
                  TextTop,
                  TextLeft+TextWidth(Caption),
                  TextTop+TextHeight(Caption));
      StrPCopy(outText,Caption);
      if not Enabled then
        Font.Color:=clGrayText;
      DrawText(Handle,
               outText,
               length(Caption),
               TextR,
               DT_SingleLine);
    end;
  end;
end;

procedure TOvalButton.SetBevelWidth(newValue: TBevelWidth);
begin
  if FBevelWidth<>newValue then begin
    FBevelWidth:=newValue;
    Invalidate;
  end;
end;

procedure TOvalButton.SetColor(newColor: TColor);
begin
  if FColor<>newColor then begin
    FColor:=newColor;
    AssignBevelColors(FColor,FColorHighlight,FColorShadow,FHLContrast,FShContrast);
    Invalidate;
  end;
end;

procedure TOvalButton.SetDown(newValue: boolean);
begin
  if FDown<>newValue then begin
    FDown:=newValue;
    if FDown then
      FState:=-1
    else begin
      if FFlat then
        FState:=0
      else
        FState:=1;
    end;
    Invalidate;
  end;
end;

procedure TOvalButton.SetFlat(newValue: boolean);
begin
  if FFlat<>newValue then begin
    FFlat:=newValue;
    if FFlat then
      FState:=0
    else
      FState:=1;
    Invalidate;
  end;
end;

procedure TOvalButton.SetFont(newFont: TFont);
begin
  if FFont<>newFont then begin
    FFont.Assign(newFont);
    Invalidate;
  end;
end;

procedure TOvalButton.SetGlyph(newGlyph: TBitmap);
begin
  if(Assigned(FGlyph)) then begin
    FGlyph.Assign(newGlyph);
    FTransparentColor:=FGlyph.Canvas.Pixels[0,FGlyph.Height-1];

    if (csDesigning in ComponentState) then begin
      { Glyph 1: Normal, 2: Disabled, 3: Down;
        Muß die Ausmaße (Height * NumGlyphs) = Width  haben}
      if (newGlyph.width mod newGlyph.height=0) then
        FNumGlyphs:=newGlyph.width div newGlyph.height
      else
        FNumGlyphs:=1;
    end;

    Invalidate;
  end;
end;

procedure TOvalButton.SetLayout(newLayout: TButtonLayout);
begin
  if FLayout<>newLayout then begin
    FLayout:=newLayout;
    Invalidate;
  end;
end;

procedure TOvalButton.SetMargin(newValue: integer);
begin
  if FMargin<>newValue then begin
    FMargin:=newValue;
    Invalidate;
  end;
end;

procedure TOvalButton.SetNumGlyphs(newNumGlyphs: TNumGlyphs);
begin
  if FNumGlyphs<>newNumGlyphs then begin
    FNumGlyphs:=newNumGlyphs;
    Invalidate;
  end;
end;

procedure TOvalButton.SetSpacing(newValue: integer);
begin
  if FSpacing<>newValue then begin
    FSpacing:=newValue;
    Invalidate;
  end;
end;

procedure TOvalButton.SetTransparent(newValue: boolean);
begin
  if FTransparent<>newValue then begin
    FTransparent:=newValue;
    Invalidate;
  end;
end;

procedure TOvalButton.SetTransparentColor(newColor: TColor);
begin
  if FTransparentColor<>newColor then begin
    FTransparentColor:=newColor;
    Invalidate;
  end;
end;

procedure Register;
begin
  RegisterComponents('Justus',[TOvalButton]);
end;

end.
