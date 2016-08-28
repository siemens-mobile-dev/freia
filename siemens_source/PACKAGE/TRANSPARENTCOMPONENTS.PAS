unit TransparentComponents;

interface

uses
  Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
  StdCtrls, ComCtrls;

type
  TCMMouseEnter = record
    Msg: Cardinal;
    Unused: Integer;
    Sender: TControl;
    Result: LongInt;

  end;

  TCMMouseLeave = TCMMouseEnter;

  TMouseEnter = procedure (Sender: TControl; var Message: TCMMouseEnter) of object;
  TMouseLeave = procedure (Sender: TControl; var Message: TCMMouseLeave) of object;

  TTransparentFrameStyle = ( tfsNone, tfsSingle, tfsLowered, tfsRaised );

  TCtrl = class(TWinControl);

  TTransparentEdit = class(TEdit)
  private
    FAlignText: TAlignment;
    FTransparent: Boolean;
    FPainting: Boolean;
    procedure SetAlignText(Value: TAlignment);
    procedure SetTransparent(Value: Boolean);
    procedure WMPaint(var Message: TWMPaint); message WM_PAINT;
    procedure WMNCPaint (var Message: TMessage); message WM_NCPAINT;
    procedure WMEraseBkGnd(var Message: TWMEraseBkGnd); message WM_ERASEBKGND;
    procedure CNCtlColorEdit(var Message: TWMCtlColorEdit); message CN_CTLCOLOREDIT;
    procedure CNCtlColorStatic(var Message: TWMCtlColorStatic); message CN_CTLCOLORSTATIC;
    procedure CMParentColorChanged(var Message: TMessage); message CM_PARENTCOLORCHANGED;
    procedure WMSize(var Message: TWMSize); message WM_SIZE;
    procedure WMMove(var Message: TWMMove); message WM_MOVE;
  protected
    procedure RepaintWindow;
    procedure CreateParams(var Params: TCreateParams); override;
    procedure Change; override;
    procedure SetParent(AParent: TWinControl); override;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
  published
    property AlignText: TAlignment read FAlignText write SetAlignText default taLeftJustify;
    property Transparent: Boolean read FTransparent write SetTransparent default false;
  end;

  // Transparent Memo
  TTransparentMemo = class(TMemo)
  private
    FAlignText: TAlignment;
    FTransparent: Boolean;
    FPainting: Boolean;
    procedure SetAlignText(Value: TAlignment);
    procedure SetTransparent(Value: Boolean);
    procedure WMPaint(var Message: TWMPaint); message WM_PAINT;
    procedure WMNCPaint (var Message: TMessage); message WM_NCPAINT;
    procedure WMEraseBkGnd(var Message: TWMEraseBkGnd); message WM_ERASEBKGND;
    procedure CNCtlColorEdit(var Message: TWMCtlColorEdit); message CN_CTLCOLOREDIT;
    procedure CNCtlColorStatic(var Message: TWMCtlColorStatic); message CN_CTLCOLORSTATIC;
    procedure CMParentColorChanged(var Message: TMessage); message CM_PARENTCOLORCHANGED;
    procedure WMSize(var Message: TWMSize); message WM_SIZE;
    procedure WMMove(var Message: TWMMove); message WM_MOVE;
  protected
    procedure RepaintWindow;
    procedure CreateParams(var Params: TCreateParams); override;
    procedure Change; override;
    procedure SetParent(AParent: TWinControl); override;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
  published
    property AlignText: TAlignment read FAlignText write SetAlignText default taLeftJustify;
    property Transparent: Boolean read FTransparent write SetTransparent default false;
  end;

  TTransparentRadioGroup = class(TGraphicControl)
  private
    { Private declarations }
    FTransparent:Boolean;
    FCheckedBmp:TBitmap;
    FUnCheckedBmp:TBitmap;
    FItems:TStringList;
    FSpace:integer;
    FItemIndex:integer;
    FFrame:TTransparentFrameStyle;
    FCaptionAlignment:TAlignment;
    FCaptionTransparent:boolean;
    FMouseEnter: TMouseEnter;
    FMouseLeave: TMouseLeave;
    FOnChange:TNotifyEvent;
    FOnPaint:TNotifyEvent;
    procedure SetCheckedBmp(Value:TBitmap);
    procedure SetUnCheckedBmp(Value:TBitmap);
    procedure SetTransparent(Value:boolean);
    procedure SetFrame(Value:TTransparentFrameStyle);
    procedure SetCaptionAlignment(Value:TAlignment);
    procedure SetCaptionTransparent(Value:boolean);
    procedure OwnMouseEnter;
    procedure OwnMouseLeave;
    procedure DrawFrame;
    procedure DrawCaption;
  protected
    { Protected declarations }
    procedure Paint; override;
    procedure CMMouseEnter(var Message: TCmMouseEnter); message CM_MOUSEENTER;
    procedure CMMouseLeave(var Message: TCmMouseLeave); message CM_MOUSELEAVE;
    procedure OwnMouseDown(var Message: TMessage); message WM_LBUTTONDOWN;
    procedure Click; override;
    procedure SetItems(Value:TStringList);
    procedure SetSpace(Value:integer);
    procedure SetItemIndex(Value:integer);
    //procedure CreateParams(var Params: TCreateParams); override;
    procedure wmerbkgnd( var Msg:TMessage );message WM_ERASEBKGND;
  public
    { Public declarations }
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
  published
    { Published declarations }
    function ESGetItemIndex(x,y:integer):integer;
    property CheckedBmp:TBitmap read FCheckedBmp write SetCheckedBmp;
    property UnCheckedBmp:TBitmap read FUnCheckedBmp write SetUnCheckedBmp;
    property Items:TStringList read FItems write SetItems;
    property Space:integer read FSpace write SetSpace;
    property ItemIndex:integer read FItemIndex write SetItemIndex;
    property OnChange:TNotifyEvent read FOnChange write FOnChange;
    property Transparent:Boolean read FTransparent write SetTransparent;
    property OnPaint:TNotifyEvent read FOnPaint write FOnPaint;
    property Frame:TTransparentFrameStyle read FFrame write SetFrame;
    property CaptionAlign:TAlignment read FCaptionAlignment write SetCaptionAlignment default taLeftJustify;
    property CaptionTransparent:boolean read FCaptionTransparent write SetCaptionTransparent;

    property Caption;
    property Color;
    property DragCursor;
    property DragMode;
    property Enabled;
    property Font;
    property ParentColor;
    property ParentFont;
    property ParentShowHint;
    property PopupMenu;
    property ShowHint;
    property Visible;
    property OnClick;
    property OnDragDrop;
    property OnDragOver;
    property OnEndDrag;
    property OnMouseDown;
    property OnMouseMove;
    property OnMouseUp;
    property OnStartDrag;
    property OnMouseEnter: TMouseEnter read FMouseEnter write FMouseEnter;
    property OnMouseLeave: TMouseLeave read FMouseLeave write FMouseLeave;
  end;

  TCheckControlType = (iacctCheck,iacctSelect);
  TCheckControlStyle = (iaccsStyle1,iaccsBitmap,iaccsStyle2,iaccsStyle1Small,iaccsStyle3);
  TCheckControlTextHAlign = (iaccthaLeft,iaccthaRight,iaccthaCenter);
  TCheckControlTextVAlign = (iacctvaTop,iacctvaBottom,iacctvaCenter);
  TCheckControlText3DStyle =(iscct3dsSimple,iscct3dsUp,iscct3dsDown);
  TCheckControlChangeStateProcedure = procedure(ASender : TPersistent;ANewStateSelected : Boolean) of object;
  TCheckControl = class(TCustomControl)
  private
    FTextAngle: integer;
    FControlType: TCheckControlType;
    FTextLines: TStringList;
    FGroupIndex: Cardinal;
    FChecked: Boolean;
    FCheckFieldColor1: TColor;
    FCheckFieldColor2: TColor;
    FStyle: TCheckControlStyle;
    FTextAlign: TCheckControlTextHAlign;
    FTextFieldHorizontalAlign: TCheckControlTextHAlign;
    FTextFieldVerticalAlign: TCheckControlTextVAlign;
    FImageList: TImageList;
    FGapTextToImage: Byte;
    FTransparent: Boolean;
    FShowFocus: Boolean;
    FText3DStyle: TCheckControlText3DStyle;
    FOnChangeState: TCheckControlChangeStateProcedure;
    function GetAutoSize: Boolean;
    function GetColor: TColor;
    function GetFont: TFont;
    function GetHeight: Integer;
    function GetText: TCaption;
    function GetWidth: Integer;
    procedure SetAutoSize(Value: Boolean);
    procedure SetColor(Value: TColor);
    procedure SetFont(Value: TFont);
    procedure SetHeight(Value: Integer);
    procedure SetText(Value: TCaption);
    procedure SetWidth(Value: Integer);

    procedure WMKeyUp(var Message: TWMKeyUp); message WM_KEYUP;
    procedure WMLButtonDown(var Message: TWMLButtonDown); message WM_LBUTTONDOWN;
    procedure WMLButtonDblClk(var Message: TWMLButtonDblClk); message WM_LBUTTONDBLCLK;
    procedure WMSIZE(var Message: TMessage);message WM_SIZE;
    procedure WMSETFOCUS(var Message : TMessage);message WM_SETFOCUS;
    procedure WMKILLFOCUS(var Message : TMessage);message WM_KILLFOCUS;
    procedure SetControlType(Value: TCheckControlType);
    procedure SetTextAngle(Value: integer);
    procedure SetTextLines(Value: TStringList);
    procedure FontIsChange(Sender: TObject);
    procedure SetGroupIndex(Value: Cardinal);
    procedure SetChecked(Value: Boolean);
    procedure SetCheckFieldColor1(Value: TColor);
    procedure SetCheckFieldColor2(Value: TColor);
    procedure SetStyle(Value: TCheckControlStyle);
    procedure SetTextAlign(Value: TCheckControlTextHAlign);
    procedure SetTextFieldHorizontalAlign(Value: TCheckControlTextHAlign);
    procedure SetTextFieldVerticalAlign(Value: TCheckControlTextVAlign);
    function  GetBMPSize: Tsize;
    function  GetTextSize : TSize;
    function  GetFullSize : TSize;
    function  GetBasis : TSize;
    function  GetOffset : TSize;
    function  GetCurrOffset(i : Integer) : TSize;
    function  GetBMPBasis : TSize;
    procedure RotateFont(AFont: TFont; ADegDeci: Integer);
    function GetEnabled1: Boolean;
    procedure SetEnabled1(Value: Boolean);
    procedure DrawCheckBMP;
    procedure DrawFocus;
    procedure SetGapTextToImage(Value: Byte);
    procedure SetTransparent(Value: Boolean);
    procedure SetProperlyRGN;
    procedure SetShowFocus(Value: Boolean);
    procedure SetText3DStyle(Value: TCheckControlText3DStyle);
    function LessColorDistanceThen(C1,C2 : TColor;ADistance : Integer): Boolean;
    procedure SetOnChangeState(Value: TCheckControlChangeStateProcedure);
    function GetVisible: Boolean;
    procedure SetVisible(Value: Boolean);
  protected
    procedure Paint; override;
    procedure AdjustSize; override;
    procedure CreateHandle; override;
  public
    constructor Create(AOwner: TComponent); override;
    destructor Destroy; override;
    procedure SetBounds(ALeft, ATop, AWidth, AHeight: Integer); override;
    procedure Assign(Source: TPersistent); override;
  published
    //Opened properties from TControl
    property Color : TColor read GetColor write SetColor;
    property Font : TFont read GetFont write SetFont;
    property ParentBiDiMode;
    property ParentColor;
    property ParentFont;
    property ParentShowHint;
    property Text : TCaption read GetText write SetText;
    property OnClick;
    property OnDblClick;
    property OnMouseDown;
    property OnMouseMove;
    property OnMouseUp;
    property OnResize;
    property Enabled : Boolean read GetEnabled1 write SetEnabled1;
    property Align;
    property Anchors;
    property BiDiMode;
    property Constraints;
    property ShowHint;
    property Visible: Boolean read GetVisible write SetVisible;
    property Width: Integer read GetWidth write SetWidth;
    property Height: Integer read GetHeight write SetHeight;

    //Opened properties from TWinControl
    property TabOrder;
    property TabStop;
    property OnEnter;
    property OnExit;
    property OnKeyDown;
    property OnKeyPress;
    property OnKeyUp;
    //New properties
    property ControlType : TCheckControlType read FControlType write SetControlType default iacctCheck;
    property TextAngle : integer read FTextAngle write SetTextAngle default 0;
    property TextLines : TStringList read FTextLines write SetTextLines;
    property GroupIndex : Cardinal read FGroupIndex write SetGroupIndex default 0;
    property Checked : Boolean read FChecked write SetChecked;
    property Style : TCheckControlStyle read FStyle write SetStyle default iaccsStyle1Small;
    property ImageList : TImageList read FImageList write FImageList default nil;
    //4 bitmaps - not checked,checked,disabled,disabled checked
    property TextAlign : TCheckControlTextHAlign read FTextAlign write SetTextAlign default iaccthaLeft;
    property TextFieldVerticalAlign : TCheckControlTextVAlign read FTextFieldVerticalAlign write SetTextFieldVerticalAlign
                                        default iacctvaTop;
    property TextFieldHorizontalAlign : TCheckControlTextHAlign read FTextFieldHorizontalAlign write SetTextFieldHorizontalAlign
                                        default iaccthaLeft;
    property CheckFieldColor1 : TColor read FCheckFieldColor1 write SetCheckFieldColor1 default clGray;
    property CheckFieldColor2 : TColor read FCheckFieldColor2 write SetCheckFieldColor2 default clSilver;
    property GapTextToImage : Byte read FGapTextToImage write SetGapTextToImage default 4;
    property Transparent : Boolean read FTransparent write SetTransparent default True;
    property ShowFocus : Boolean read FShowFocus write SetShowFocus default TRue;
    property Text3DStyle : TCheckControlText3DStyle read FText3DStyle write SetText3DStyle default iscct3dsSimple;
    property OnChangeState : TCheckControlChangeStateProcedure read FOnChangeState write SetOnChangeState;

    //Opened properties from TControl
    property AutoSize : Boolean read GetAutoSize write SetAutoSize;
  end;

  TTransparentGroupBox = class(TGroupBox)
  private
    { Private declarations }
    FBevelLowColor: TColor;
    FBevelHiColor: TColor;
    procedure SetBvlLowColor(Value: TColor);
    procedure SetBvlHiColor(Value: TColor);
    Procedure WMEraseBkGnd(Var Message:TWMEraseBkGnd); Message WM_EraseBkGnd;
    Procedure WMMove(Var Message:TWMMove); Message WM_Move;
  protected
    { Protected declarations }
    Procedure CreateParams(Var Params:TCreateParams); Override;
    Procedure Paint; Override;
    Procedure SetParent(AParent:TWinControl); Override;
    procedure DrawFrame(Rect:TRect);
  public
    { Public declarations }
    Constructor Create(AOwner:TComponent); Override;
    Procedure Invalidate; Override;
  published
    { Published declarations }
    property BevelShadowColor:TColor    read FBevelLowColor
                                       write SetBvlLowColor;
    property BevelHighlightColor:TColor read FBevelHiColor
                                       write SetBvlHiColor;
  end;

  TNumGlyphs    = 0..4;
  TBevelWidth   = 1..10;
  TButtonLayout = (blGlyphBottom, blGlyphLeft, blGlyphRight, blGlyphTop);


procedure Register;

implementation

Var SupportsQuality : Boolean;
    Vinfo : TOSVERSIONINFO;

const
 BorderRec: array[TBorderStyle] of Integer = (1, -1);

procedure Register;
begin
  RegisterComponents('Transparent', [TTransparentEdit, TTransparentMemo, TTransparentRadioGroup, TCheckControl, TTransparentGroupBox]);
end;

function GetScreenClient(Control: TControl): TPoint;
var
 p: TPoint;
begin
 p := Control.ClientOrigin;
 ScreenToClient(Control.Parent.Handle, p);
 Result := p;
end;

constructor TTransparentEdit.Create(AOwner: TComponent);
begin
 inherited Create(AOwner);
 FAlignText := taLeftJustify;
 FTransparent := false;
 FPainting := false; 
end;

destructor TTransparentEdit.Destroy;
begin
 inherited Destroy;
end;

procedure TTransparentEdit.SetAlignText(Value: TAlignment);
begin
 if FAlignText <> Value then
 begin
  FAlignText := Value;
  RecreateWnd;
  Invalidate;
 end;
end;

procedure TTransparentEdit.SetTransparent(Value: Boolean);
begin
 if FTransparent <> Value then
 begin
  FTransparent := Value;
  Invalidate;
 end;
end;

procedure TTransparentEdit.WMEraseBkGnd(var Message: TWMEraseBkGnd);
var
 DC: hDC;
 i: integer;
 p: TPoint;
begin
 if FTransparent then
 begin
  if Assigned(Parent) then
  begin
   DC := Message.DC;
   i := SaveDC(DC);
   p := GetScreenClient(self);
   p.x := -p.x;
   p.y := -p.y;
   MoveWindowOrg(DC, p.x, p.y);
   SendMessage(Parent.Handle, $0014, DC, 0);
   TCtrl(Parent).PaintControls(DC, nil);
   RestoreDC(DC, i);
  end;
 end else inherited;
end;

procedure TTransparentEdit.WMPaint(var Message: TWMPaint);
begin
 inherited;
 if FTransparent then
 if not FPainting then
 RepaintWindow;
end;

procedure TTransparentEdit.WMNCPaint(var Message: TMessage);
begin
 inherited;
end;

procedure TTransparentEdit.CNCtlColorEdit(var Message: TWMCtlColorEdit);
begin
 inherited;
 if FTransparent then
 SetBkMode(Message.ChildDC, 1);
end;

procedure TTransparentEdit.CNCtlColorStatic(var Message: TWMCtlColorStatic);
begin
 inherited;
 if FTransparent then
 SetBkMode(Message.ChildDC, 1);
end;

procedure TTransparentEdit.CMParentColorChanged(var Message: TMessage);
begin
 inherited;
 if FTransparent then
 Invalidate;
end;

procedure TTransparentEdit.WMSize(var Message: TWMSize);
begin
 inherited;
 Invalidate; 
end;

procedure TTransparentEdit.WMMove(var Message: TWMMove);
begin
 inherited;
 Invalidate;
end;

procedure TTransparentEdit.RepaintWindow;
var
 DC: hDC;
 TmpBitmap, Bitmap: hBitmap;
begin
 if FTransparent then
 begin
  FPainting := true;
  HideCaret(Handle);
  DC := CreateCompatibleDC(GetDC(Handle));
  TmpBitmap := CreateCompatibleBitmap(GetDC(Handle), Succ(ClientWidth), Succ(ClientHeight));
  Bitmap := SelectObject(DC, TmpBitmap);
  PaintTo(DC, 0, 0);
  BitBlt(GetDC(Handle), BorderRec[BorderStyle], BorderRec[BorderStyle], ClientWidth, ClientHeight, DC, 1, 1, SRCCOPY);
  SelectObject(DC, Bitmap);
  DeleteDC(DC);
  ReleaseDC(Handle, GetDC(Handle));
  DeleteObject(TmpBitmap);
  ShowCaret(Handle);
  FPainting := false;
 end;
end;

procedure TTransparentEdit.CreateParams(var Params: TCreateParams);
const
 Alignments: array [TAlignment] of DWord = (ES_LEFT, ES_RIGHT, ES_CENTER);
begin
 inherited CreateParams(Params);
 Params.Style := Params.Style or ES_MULTILINE or Alignments[FAlignText];
end;

procedure TTransparentEdit.Change;
begin
 RepaintWindow;
 inherited Change;
end;

procedure TTransparentEdit.SetParent(AParent: TWinControl);
begin
 inherited SetParent(AParent);
end;

// Transparent Memo
constructor TTransparentMemo.Create(AOwner: TComponent);
begin
 inherited Create(AOwner);
 FAlignText := taLeftJustify;
 FTransparent := false;
 FPainting := false;
end;

destructor TTransparentMemo.Destroy;
begin
 inherited Destroy;
end;

procedure TTransparentMemo.SetAlignText(Value: TAlignment);
begin
 if FAlignText <> Value then
 begin
  FAlignText := Value;
  RecreateWnd;
  Invalidate;
 end;
end;

procedure TTransparentMemo.SetTransparent(Value: Boolean);
begin
 if FTransparent <> Value then
 begin
  FTransparent := Value;
  Invalidate;
 end;
end;

procedure TTransparentMemo.WMEraseBkGnd(var Message: TWMEraseBkGnd);
var
 DC: hDC;
 i: integer;
 p: TPoint;
begin
 if FTransparent then
 begin
  if Assigned(Parent) then
  begin
   DC := Message.DC;
   i := SaveDC(DC);
   p := GetScreenClient(self);
   p.x := -p.x;
   p.y := -p.y;
   MoveWindowOrg(DC, p.x, p.y);
   SendMessage(Parent.Handle, $0014, DC, 0);
   TCtrl(Parent).PaintControls(DC, nil);
   RestoreDC(DC, i);
  end;
 end else inherited;
end;

procedure TTransparentMemo.WMPaint(var Message: TWMPaint);
begin
 inherited;
 if FTransparent then
 if not FPainting then
 RepaintWindow;
end;

procedure TTransparentMemo.WMNCPaint(var Message: TMessage);
begin
 inherited;
end;

procedure TTransparentMemo.CNCtlColorEdit(var Message: TWMCtlColorEdit);
begin
 inherited;
 if FTransparent then
 SetBkMode(Message.ChildDC, 1);
end;

procedure TTransparentMemo.CNCtlColorStatic(var Message: TWMCtlColorStatic);
begin
 inherited;
 if FTransparent then
 SetBkMode(Message.ChildDC, 1);
end;

procedure TTransparentMemo.CMParentColorChanged(var Message: TMessage);
begin
 inherited;
 if FTransparent then
 Invalidate;
end;

procedure TTransparentMemo.WMSize(var Message: TWMSize);
begin
 inherited;
 Invalidate;
end;

procedure TTransparentMemo.WMMove(var Message: TWMMove);
begin
 inherited;
 Invalidate;
end;

procedure TTransparentMemo.RepaintWindow;
var
 DC: hDC;
 TmpBitmap, Bitmap: hBitmap;
begin
 if FTransparent then
 begin
  FPainting := true;
  HideCaret(Handle);
  DC := CreateCompatibleDC(GetDC(Handle));
  TmpBitmap := CreateCompatibleBitmap(GetDC(Handle), Succ(ClientWidth), Succ(ClientHeight));
  Bitmap := SelectObject(DC, TmpBitmap);
  PaintTo(DC, 0, 0);
  BitBlt(GetDC(Handle), BorderRec[BorderStyle], BorderRec[BorderStyle], ClientWidth, ClientHeight, DC, 1, 1, SRCCOPY);
  SelectObject(DC, Bitmap);
  DeleteDC(DC);
  ReleaseDC(Handle, GetDC(Handle));
  DeleteObject(TmpBitmap);
  ShowCaret(Handle);
  FPainting := false;
 end;
end;

procedure TTransparentMemo.CreateParams(var Params: TCreateParams);
const
 Alignments: array [TAlignment] of DWord = (ES_LEFT, ES_RIGHT, ES_CENTER);
begin
 inherited CreateParams(Params);
 Params.Style := Params.Style or ES_MULTILINE or Alignments[FAlignText];
end;

procedure TTransparentMemo.Change;
begin
 RepaintWindow;
 inherited Change;
end;

procedure TTransparentMemo.SetParent(AParent: TWinControl);
begin
 inherited SetParent(AParent);
end;

//*******************************************************************

procedure TTransparentRadioGroup.SetTransparent(Value:Boolean);
begin
 FTransparent := Value;
 Invalidate;
 Paint;
end;


procedure TTransparentRadioGroup.SetItemIndex(Value:integer);
begin
 FItemIndex := Value;
 if Assigned( FOnChange ) then FOnChange( Self );
 Invalidate;
 Paint;
end;


function TTransparentRadioGroup.ESGetItemIndex(x,y:integer):integer;
var i,iTop,iWidth,iLeft,iHeight:integer;
begin
for i:=1 to FItems.Count do
    begin
      iTop := ( ( i - 1 )*( FCheckedBmp.Height + FSpace ) );
      iWidth := Canvas.TextWidth( FItems.Strings[i - 1] ) + FCheckedBmp.Width;
      iLeft := 10;
      iHeight := FCheckedBmp.Height;
      if ( ( x >= iLeft ) and ( x <= iLeft + iWidth ) ) then
         if ( ( y >= iTop ) and ( y <= iTop + iHeight ) ) then begin
            Result := i;
            Exit;end;
    end;
Result := -1;
end;

procedure TTransparentRadioGroup.OwnMouseDown(var Message: TMessage);
 var oItemIndex:integer;
begin
oItemIndex := ESGetItemIndex( Message.LParamLo, Message.LParamHI );
if ( ( oItemIndex > 0 ) and ( oItemIndex <= FItems.Count ) ) then begin
   ItemIndex := oItemIndex;
   Invalidate;
   Paint;end;
   If Assigned( OnMouseDown ) then begin
                               OnMouseDown( Self, mbLeft, KeysToShiftState( Message.wParam ),
                                            Message.LParamLo, Message.LParamHi );
                              end;
end;

procedure TTransparentRadioGroup.OwnMouseEnter;
begin
//for later development
end;

procedure TTransparentRadioGroup.OwnMouseLeave;
begin
//for later development
end;



procedure TTransparentRadioGroup.CMMouseEnter(var Message: TCmMouseEnter);
begin
  //FDBufBmp.Assign(Bitmap);
  OwnMouseEnter;
  if Assigned(FMouseEnter) then FMouseEnter(Self, Message);
end;

procedure TTransparentRadioGroup.CMMouseLeave(var Message: TCmMouseLeave);
begin
  //FDBufBmp.Assign(Bitmap);
  OwnMouseLeave;
  if Assigned(FMouseLeave) then FMouseLeave(Self, Message);
end;


procedure TTransparentRadioGroup.SetSpace(Value:integer);
begin
 FSpace:=Value;
 Invalidate;
 Paint;
end;

procedure TTransparentRadioGroup.SetItems(Value:TStringList);
begin
 FItems.Assign(Value);
 Invalidate;
 Paint;
end;

procedure TTransparentRadioGroup.Paint;
var i,iTop,iwidth:integer;
    CaptionOffset:integer;
    R:TRect;
begin
 //inherited Paint; Unnecessary
 CaptionOffset := Canvas.TextHeight('A');
 if Assigned( FOnPaint ) then FOnPaint( Self );
 Canvas.Font.Assign( Font );
 Canvas.Brush.Color := Color;

 If fTransparent then SetBkMode(Canvas.Handle, windows.TRANSPARENT )
                 else begin
                      SetBkMode(Canvas.Handle, windows.OPAQUE );
                      Canvas.FillRect( Rect( 0, 0, Width, Height ) );
                      end;
 if ( (Assigned( FCheckedBmp )) and (Assigned( FUnCheckedBmp )) ) then
   for i := 1 to FItems.Count do
    begin
      iTop := ( ( i-1 )*( FCheckedBmp.Height + FSpace ) )+ CaptionOffset ;
      if ItemIndex = i then
         Canvas.Draw( 10, iTop, FCheckedBmp )
                     else
         Canvas.Draw( 10, iTop, FUnCheckedBmp );

      iWidth := Canvas.TextWidth( FItems.Strings[i - 1] );
      R := Bounds( 10 + FCheckedBmp.Width + FSpace, iTop, iWidth, FCheckedBmp.Height );
      DrawText( Canvas.Handle, PChar( FItems.Strings[i - 1] ), -1, R,
      DT_CENTER or DT_VCENTER or DT_SINGLELINE );
    end;
 DrawFrame;
 DrawCaption;
end;

procedure TTransparentRadioGroup.Click;
begin
end;

{ if you inherit it from tcontrol:
procedure TTransparentRadioGroup.CreateParams( var Params:TCreateParams );
begin
inherited;
  with Params do
    ExStyle := ExStyle or WS_EX_TRANSPARENT;
end;}


constructor TTransparentRadioGroup.Create(AOwner: TComponent);
begin
     inherited Create( AOwner );
     ControlStyle := [csSetCaption, csClickEvents];

     FCheckedBmp   := TBitmap.Create;
     FUnCheckedBmp := TBitmap.Create;
     FCheckedBmp.Transparent   := true;
     FUnCheckedBmp.Transparent := true;

     FItems := TStringList.Create;

     SetBounds( 0, 0, 100, 60 );
end;

destructor TTransparentRadioGroup.Destroy;
begin
     if assigned( FCheckedBmp )   then FCheckedBmp.Free;
     if assigned( FUnCheckedBmp ) then FUnCheckedBmp.Free;
     if assigned( FItems )        then FItems.Free;
     inherited Destroy;
end;


procedure TTransparentRadioGroup.SetCheckedBmp(Value:TBitmap);
begin
 FCheckedBmp.Assign( Value );
end;

procedure TTransparentRadioGroup.SetUnCheckedBmp(Value:TBitmap);
begin
 FUnCheckedBmp.Assign( Value );
end;

procedure TTransparentRadioGroup.SetFrame(Value:TTransparentFrameStyle);
begin
 FFrame := Value;
 Invalidate;
 Paint;
end;

procedure TTransparentRadioGroup.DrawFrame;

     function GetCaptionLeft:integer;
     begin
       Case FCaptionAlignment of
            taLeftJustify:  Result := Canvas.TextWidth( 'A' );
            taRightJustify: Result := Width-Canvas.TextWidth( Caption+'A' );
            taCenter:       Result := ( Width-Canvas.TextWidth( Caption ) ) div 2;
       end;//of case
     end;


     procedure uLeftFrameLine( offset, penWidth:integer; drawColor:TColor );
     var CaptionOffset:integer;
         CaptionWidth:integer;
         CaptionLeft:integer;
     begin
      CaptionOffset := Canvas.TextHeight( 'A' ) div 2;
      CaptionWidth := Canvas.TextWidth( Caption );
      CaptionLeft := GetCaptionLeft;
      Canvas.Pen.Width := penWidth;
      Canvas.Pen.Color := drawColor;
      Canvas.MoveTo( offset, CaptionOffset + offset );
      Canvas.LineTo( CaptionLeft - 1, CaptionOffset + offset  );
      Canvas.MoveTo( CaptionLeft + CaptionWidth, CaptionOffset + offset );
      Canvas.LineTo( Width-offset, CaptionOffset + offset  );
      Canvas.MoveTo( offset, CaptionOffset + offset );
      Canvas.LineTo( offset, Height-offset );
     end;

     procedure lRightFrameLine( offset, penWidth:integer; drawColor:TColor );
     var CaptionOffset:integer;
     begin
      CaptionOffset := Canvas.TextHeight( 'A' ) div 2;
      Canvas.Pen.Width := penWidth;
      Canvas.Pen.Color := drawColor;
      Canvas.MoveTo( Width-offset, Height-offset );
      Canvas.LineTo( offset, Height-offset  );
      Canvas.MoveTo( Width-offset, Height-offset );
      Canvas.LineTo( Width-offset, CaptionOffset+offset );
     end;

begin
 Case FFrame of
      tfsNone:if ( csDesigning in ComponentState ) then begin
                 Canvas.Pen.Style := psDash;
                 Canvas.Pen.Color := clBlack;
                 Canvas.Rectangle( 0, 0, Width, Height );
                end;
      tfsSingle: begin
                  uLeftFrameLine( 1, 1, clBtnText );
                  lRightFrameLine( 1, 1, clBtnText );
                 end;
      tfsLowered: begin
                  uLeftFrameLine( 1, 1, clBtnShadow );
                  lRightFrameLine( 1, 1,clBtnHighLight  );
                 end;
      tfsRaised: begin
                  uLeftFrameLine( 1, 1, clBtnHighLight );
                  lRightFrameLine( 1, 1, clBtnShadow );
                 end;
      end;// of case
end;

procedure TTransparentRadioGroup.SetCaptionAlignment(Value:TAlignment);
begin
 FCaptionAlignment := Value;
 Invalidate;
 Paint;
end;

procedure TTransparentRadioGroup.DrawCaption;
var x,y:integer;
    OldBkMode,tmpBkMode:integer;
begin
 Case FCaptionAlignment of
      taLeftJustify: begin
                      x := Canvas.TextWidth( 'A' );
                      y:=0;
                     end;
      taRightJustify: begin
                       x := Width-Canvas.TextWidth( Caption+'A' );
                       y:=0;
                      end;
      taCenter: begin
                 x := ( Width-Canvas.TextWidth( Caption ) ) div 2;
                 y:=0;
                end;
      end;//of case

 OldBkMode := GetBkMode( Canvas.Handle );

 try
  if CaptionTransparent then tmpBkMode :=windows.TRANSPARENT
                        else tmpBkMode :=Windows.OPAQUE;
  SetBkMode( Canvas.Handle, tmpBkMode );

  Canvas.TextOut( x, y, Caption );
 finally
  SetBkMode( Canvas.Handle, OldBkMode );
 end;
end;

procedure TTransparentRadioGroup.SetCaptionTransparent(Value:boolean);
begin
 FCaptionTransparent := Value;
 Invalidate;
 Paint;
end;

procedure TTransparentRadioGroup.wmerbkgnd(var Msg:TMessage );
begin
end;

function GetCColor(Color01,Color02: Tcolor;R,i :integer): Tcolor;
var
  C1,C2 : Tcolor;
  R1,G1,B1,R2,G2,B2 : Byte;
  Nr,Ng,Nb : Integer;
begin
  c1:=ColorTorgb(Color01);
  c2:=ColorTorgb(Color02);
  R1:=Pbyte(@C1)^;
  G1:=Pbyte(integer(@C1)+1)^;
  B1:=Pbyte(integer(@C1)+2)^;
  R2:=Pbyte(@C2)^;
  G2:=Pbyte(integer(@C2)+1)^;
  B2:=Pbyte(integer(@C2)+2)^;
  if R<>0 then
    begin
      Nr:=(R1+(R2-R1)*i div R);
      Ng:=(G1+(G2-G1)*i div R);
      Nb:=(B1+(B2-B1)*i div R);
      if Nr<0   then Nr:=0;
      if Nr>255 then Nr:=255;
      if Ng<0   then Ng:=0;
      if Ng>255 then Ng:=255;
      if Nb<0   then Nb:=0;
      if Nb>255 then Nb:=255;
      Result:=RGB(Nr,Ng,Nb);
    end else result:=Color01;
end;

{ TCheckControl }

procedure TCheckControl.AdjustSize;
begin
  //NoAction;
end;

procedure TCheckControl.Assign(Source: TPersistent);
begin
  if Source is TCheckControl then
    begin
      SetBounds(Left,Top,(Source as TCheckControl).Width,(Source as TCheckControl).Height);
      inherited Color:=(Source as TCheckControl).Color;
      inherited AutoSize:=(Source as TCheckControl).AutoSize;
      inherited Font.Assign((Source as TCheckControl).Font);
      inherited Text:=(Source as TCheckControl).Text;
      FTextLines.Text:=(Source as TCheckControl).FTextLines.Text;

      FControlType:=(Source as TCheckControl).FControlType;
      FGroupIndex:=(Source as TCheckControl).FGroupIndex;
      Checked:=(Source as TCheckControl).FChecked;
      FCheckFieldColor1:=(Source as TCheckControl).FCheckFieldColor1;
      FCheckFieldColor2:=(Source as TCheckControl).FCheckFieldColor2;
      FStyle:=(Source as TCheckControl).FStyle;
      FTextAlign:=(Source as TCheckControl).FTextAlign;
      FTextFieldHorizontalAlign:=(Source as TCheckControl).FTextFieldHorizontalAlign;
      FTextFieldVerticalAlign:=(Source as TCheckControl).FTextFieldVerticalAlign;
      FImageList:=(Source as TCheckControl).FImageList;
      FGapTextToImage:=(Source as TCheckControl).FGapTextToImage;
      FTransparent:=(Source as TCheckControl).FTransparent;
      TabStop:=(Source as TCheckControl).TabStop;
      FShowFocus:=(Source as TCheckControl).FShowFocus;
      FText3DStyle:=(Source as TCheckControl).FText3DStyle;
    end else
    begin
      inherited Assign(Source);
    end;
end;

constructor TCheckControl.Create(AOwner: TComponent);
begin
  inherited Create(AOwner);
  ControlStyle:=ControlStyle + [csOpaque];
  inherited SetBounds(Left,Top,80,20);
  inherited ParentFont:=false;
  inherited Font.Name:='Times New Roman';
  inherited Font.OnChange:=FontIsChange;
  inherited Text:='CheckText';

  FTextLines:=TStringList.Create;
  FTextLines.Add('CheckText');
  FTextLines.OnChange:=FontIsChange;
  FControlType:=iacctCheck;
  FTextAngle:=0;
  FGroupIndex:=0;
  FImageList:=nil;
  FTextAlign:=iaccthaLeft;
  FTextFieldVerticalAlign:=iacctvaTop;
  FTextFieldHorizontalAlign:=iaccthaLeft;
  FCheckFieldColor1:=clGray;
  FCheckFieldColor2:=clSilver;
  FGapTextToImage:=4;
  FTransparent:=True;
  TabStop:=True;
  FShowFocus:=True;
  FText3DStyle:=iscct3dsSimple;
  FOnChangeState:=nil;
  FStyle:=iaccsStyle1Small;
  inherited AutoSize:=True;

end;

procedure TCheckControl.CreateHandle;
begin
  inherited CreateHandle;
  SetProperlyRGN;
end;

destructor TCheckControl.Destroy;
begin
  FTextLines.free;
  inherited Destroy;
end;

procedure TCheckControl.DrawCheckBMP;
var AC1,AC2,LC1,LC2,DC1,DC2 : TColor;
    s1,s2 : TSize;
    p1 : array [0..7] of TPoint;
    p2 : array [0..9] of Tpoint;
    p3 : array [0..5] of TPoint;
    p4 : array [0..3] of Tpoint;
    p5 : array [0..11] of TPoint;
    i,dx,dy : integer;
begin
  if Enabled then
    begin
      AC1:=CheckFieldColor1;
      AC2:=CheckFieldColor2;
    end else
    begin
      AC1:=GetCColor(Color,CheckFieldColor1,100,30);
      AC2:=GetCColor(Color,CheckFieldColor2,100,30);
    end;
  LC1:=GetCColor(AC1,clWhite,100,30);
  LC2:=GetCColor(AC2,AC1,100,50);
  LC2:=GetCColor(LC2,clWhite,100,50);
  DC1:=GetCColor(AC1,clBlack,100,30);
  DC2:=GetCColor(AC2,AC1,100,50);
  DC2:=GetCColor(DC2,clBlack,100,30);
  s1:=GetBMPBasis;
  s2:=GetBMPSize;
  case FStyle of
      iaccsBitmap :
        begin
          if Assigned(FImageList) and (FImageList.Count>=4)then
            begin
              if Enabled then
                begin
                  if Checked then
                    begin
                      FImageList.Draw(Canvas,s1.cx,s1.cy,1);
                    end else
                    begin
                      FImageList.Draw(Canvas,s1.cx,s1.cy,0);
                    end;
                end else
                begin
                  if Checked then
                    begin
                      FImageList.Draw(Canvas,s1.cx,s1.cy,3);
                    end else
                    begin
                      FImageList.Draw(Canvas,s1.cx,s1.cy,2);
                    end;
                end;
            end;
        end;
      iaccsStyle1,iaccsStyle1Small :
        begin
          if FControlType = iacctCheck then
            begin
              if Checked then
                begin

                  Canvas.Pen.Color:=AC1;
                  Canvas.Brush.Color:=AC1;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Rectangle(s1.cx+2,s1.cy+2,s1.cx+s2.cx-2,s1.cy+s2.cx-2);
                  Canvas.Pen.Color:=LC1;
                  Canvas.MoveTo(s1.cx+0,s1.cy+s2.cy-1);
                  Canvas.LineTo(s1.cx+0,s1.cy+0);
                  Canvas.LineTo(s1.cx+s2.cx-1,s1.cy+0);
                  Canvas.MoveTo(s1.cx+1,s1.cy+s2.cy-2);
                  Canvas.LineTo(s1.cx+1,s1.cy+1);
                  Canvas.LineTo(s1.cx+s2.cx-2,s1.cy+1);
                  Canvas.Pen.Color:=DC1;
                  Canvas.MoveTo(s1.cx+0,s1.cy+s2.cy-1);
                  Canvas.LineTo(s1.cx+s2.cx-1,s1.cy+s2.cy-1);
                  Canvas.LineTo(s1.cx+s2.cx-1,s1.cy+0);
                  Canvas.MoveTo(s1.cx+1,s1.cy+s2.cy-2);
                  Canvas.LineTo(s1.cx+s2.cx-2,s1.cy+s2.cy-2);
                  Canvas.LineTo(s1.cx+s2.cx-2,s1.cy+1);
                  Canvas.Pen.Width:=s2.cx div 8;
                  if LessColorDistanceThen(AC2,clBlack,128*3) then
                  Canvas.Pen.Color:=LC2 else
                  Canvas.Pen.Color:=DC2;
                  Canvas.MoveTo(s1.cx+s2.cx div 3-1      ,s1.cy+s2.cx div 3-1);
                  Canvas.LineTo(s1.cx+s2.cx-s2.cx div 3-1,s1.cy+s2.cy-s2.cx div 3-1);
                  Canvas.MoveTo(s1.cx+s2.cx div 3-1      ,s1.cy+s2.cy-s2.cx div 3-1);
                  Canvas.LineTo(s1.cx+s2.cx-s2.cx div 3-1,s1.cy+s2.cx div 3-1);

                  Canvas.MoveTo(s1.cx+s2.cx div 3+1      ,s1.cy+s2.cx div 3+1);
                  Canvas.LineTo(s1.cx+s2.cx-s2.cx div 3+1,s1.cy+s2.cy-s2.cx div 3+1);
                  Canvas.MoveTo(s1.cx+s2.cx div 3+1      ,s1.cy+s2.cy-s2.cx div 3+1);
                  Canvas.LineTo(s1.cx+s2.cx-s2.cx div 3+1,s1.cy+s2.cx div 3+1);
                  Canvas.Pen.Color:=AC2;
                  Canvas.MoveTo(s1.cx+s2.cx div 3-1        ,s1.cy+s2.cx div 3-1);
                  Canvas.LineTo(s1.cx+s2.cx-s2.cx div 3+1,s1.cy+s2.cy-s2.cy div 3+1);
                  Canvas.MoveTo(s1.cx+s2.cx div 3-1,s1.cy+s2.cy-s2.cy div 3+1);
                  Canvas.LineTo(s1.cx+s2.cx-s2.cx div 3+1,s1.cy+s2.cy div 3-1);
                  Canvas.Pen.Width:=1;
                end else
                begin
                  Canvas.Pen.Color:=AC1;
                  Canvas.Brush.Color:=AC1;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Rectangle(s1.cx+2,s1.cy+2,s1.cx+s2.cx-2,s1.cy+s2.cy-2);
                  Canvas.Pen.Color:=LC1;
                  Canvas.MoveTo(s1.cx+0,s1.cy+s2.cy-1);
                  Canvas.LineTo(s1.cx+0,s1.cy+0);
                  Canvas.LineTo(s1.cx+s2.cx-1,s1.cy+0);
                  Canvas.MoveTo(s1.cx+1,s1.cy+s2.cy-2);
                  Canvas.LineTo(s1.cx+1,s1.cy+1);
                  Canvas.LineTo(s1.cx+s2.cx-2,s1.cy+1);
                  Canvas.Pen.Color:=DC1;
                  Canvas.MoveTo(s1.cx+0,s1.cy+s2.cy-1);
                  Canvas.LineTo(s1.cx+s2.cx-1,s1.cy+s2.cy-1);
                  Canvas.LineTo(s1.cx+s2.cx-1,s1.cy+0);
                  Canvas.MoveTo(s1.cx+1,s1.cy+s2.cy-2);
                  Canvas.LineTo(s1.cx+s2.cx-2,s1.cy+s2.cy-2);
                  Canvas.LineTo(s1.cx+s2.cx-2,s1.cy+1);
                end;
            end else
            begin//select
              if Checked then
                begin
                  p1[0].x:=s1.cx+2;
                  p1[0].y:=s1.cy+s2.cy div 4 +2;
                  p1[1].x:=s1.cx+s2.cx div 4 +2;
                  p1[1].y:=s1.cy+2;
                  p1[2].x:=s1.cx-s2.cx div 4 -2+s2.cx;
                  p1[2].y:=s1.cy+2;
                  p1[3].x:=s1.cx+s2.cx-2;
                  p1[3].y:=s1.cy+s2.cy div 4 +2;
                  p1[4].x:=s1.cx+s2.cx-2;
                  p1[4].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  p1[5].x:=s1.cx-s2.cx div 4 -2+s2.cx;
                  p1[5].y:=s1.cy+s2.cy-2;
                  p1[6].x:=s1.cx+s2.cx div 4 +2;
                  p1[6].y:=s1.cy+s2.cy-2;
                  p1[7].x:=s1.cx+2;
                  p1[7].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p1);
                  Canvas.pen.Style:=psSolid;
                  p2[0].x:=s1.cx+2;
                  p2[0].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  p2[1].x:=s1.cx+2;
                  p2[1].y:=s1.cy+s2.cy div 4 +2;
                  p2[2].x:=s1.cx+s2.cx div 4 +2;
                  p2[2].y:=s1.cy+2;
                  p2[3].x:=s1.cx-s2.cx div 4 -2+s2.cx;
                  p2[3].y:=s1.cy+2;
                  p2[4].x:=s1.cx+s2.cx-2;
                  p2[4].y:=s1.cy+s2.cy div 4 +2;
                  p2[5].x:=s1.cx+s2.cx;
                  p2[5].y:=s1.cy+s2.cy div 4;
                  p2[6].x:=s1.cx-s2.cx div 4+s2.cx;
                  p2[6].y:=s1.cy;
                  p2[7].x:=s1.cx+s2.cx div 4;
                  p2[7].y:=s1.cy;
                  p2[8].x:=s1.cx;
                  p2[8].y:=s1.cy+s2.cy div 4;
                  p2[9].x:=s1.cx;
                  p2[9].y:=s1.cy-s2.cy div 4+s2.cy;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=LC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;

                  p2[0].x:=s1.cx+s2.cx-2;
                  p2[0].y:=s1.cy+s2.cy div 4 +2;
                  p2[1].x:=s1.cx+s2.cx-2;
                  p2[1].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  p2[2].x:=s1.cx-s2.cx div 4 -2+s2.cx;
                  p2[2].y:=s1.cy+s2.cy-2;
                  p2[3].x:=s1.cx+s2.cx div 4 +2;
                  p2[3].y:=s1.cy+s2.cy-2;
                  p2[4].x:=s1.cx+2;
                  p2[4].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  p2[5].x:=s1.cx;
                  p2[5].y:=s1.cy-s2.cy div 4 +s2.cy;
                  p2[6].x:=s1.cx+s2.cx div 4 ;
                  p2[6].y:=s1.cy+s2.cy;
                  p2[7].x:=s1.cx-s2.cx div 4 +s2.cx;
                  p2[7].y:=s1.cy+s2.cy;
                  p2[8].x:=s1.cx+s2.cx;
                  p2[8].y:=s1.cy-s2.cy div 4 +s2.cy;
                  p2[9].x:=s1.cx+s2.cx;
                  p2[9].y:=s1.cy+s2.cy div 4 ;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=DC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;


                  p3[0].x:=s1.cx+s2.cx div 2-s2.cx div 6;
                  p3[0].y:=s1.cy+s2.cy div 2+s2.cy div 6;
                  p3[1].x:=s1.cx+s2.cx div 2-s2.cx div 6;
                  p3[1].y:=s1.cy+s2.cy div 2-s2.cy div 6;
                  p3[2].x:=s1.cx+s2.cx div 2+s2.cx div 6;
                  p3[2].y:=s1.cy+s2.cy div 2-s2.cy div 6;
                  p3[3].x:=s1.cx+s2.cx div 2+s2.cx div 6+2;
                  p3[3].y:=s1.cy+s2.cy div 2-s2.cy div 6-2;
                  p3[4].x:=s1.cx+s2.cx div 2-s2.cx div 6-2;
                  p3[4].y:=s1.cy+s2.cy div 2-s2.cy div 6-2;
                  p3[5].x:=s1.cx+s2.cx div 2-s2.cx div 6-2;
                  p3[5].y:=s1.cy+s2.cy div 2+s2.cy div 6+2;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=LC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p3);
                  Canvas.pen.Style:=psSolid;

                  p3[0].x:=s1.cx+s2.cx div 2-s2.cx div 6;
                  p3[0].y:=s1.cy+s2.cy div 2+s2.cy div 6;
                  p3[1].x:=s1.cx+s2.cx div 2+s2.cx div 6;
                  p3[1].y:=s1.cy+s2.cy div 2+s2.cy div 6;
                  p3[2].x:=s1.cx+s2.cx div 2+s2.cx div 6;
                  p3[2].y:=s1.cy+s2.cy div 2-s2.cy div 6;
                  p3[3].x:=s1.cx+s2.cx div 2+s2.cx div 6+2;
                  p3[3].y:=s1.cy+s2.cy div 2-s2.cy div 6-2;
                  p3[4].x:=s1.cx+s2.cx div 2+s2.cx div 6+2;
                  p3[4].y:=s1.cy+s2.cy div 2+s2.cy div 6+2;
                  p3[5].x:=s1.cx+s2.cx div 2-s2.cx div 6-2;
                  p3[5].y:=s1.cy+s2.cy div 2+s2.cy div 6+2;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=DC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p3);
                  Canvas.pen.Style:=psSolid;


                  Canvas.Brush.Color:=AC2;
                  Canvas.FillRect(rect(s1.cx+s2.cx div 2-s2.cx div 6,
                                       s1.cy+s2.cy div 2-s2.cy div 6,
                                       s1.cx+s2.cx div 2+s2.cx div 6,
                                       s1.cy+s2.cy div 2+s2.cy div 6));

                end else
                begin
                  p1[0].x:=s1.cx+2;
                  p1[0].y:=s1.cy+s2.cy div 4 +2;
                  p1[1].x:=s1.cx+s2.cx div 4 +2;
                  p1[1].y:=s1.cy+2;
                  p1[2].x:=s1.cx-s2.cx div 4 -2+s2.cx;
                  p1[2].y:=s1.cy+2;
                  p1[3].x:=s1.cx+s2.cx-2;
                  p1[3].y:=s1.cy+s2.cy div 4 +2;
                  p1[4].x:=s1.cx+s2.cx-2;
                  p1[4].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  p1[5].x:=s1.cx-s2.cx div 4 -2+s2.cx;
                  p1[5].y:=s1.cy+s2.cy-2;
                  p1[6].x:=s1.cx+s2.cx div 4 +2;
                  p1[6].y:=s1.cy+s2.cy-2;
                  p1[7].x:=s1.cx+2;
                  p1[7].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p1);
                  Canvas.pen.Style:=psSolid;
                  p2[0].x:=s1.cx+2;
                  p2[0].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  p2[1].x:=s1.cx+2;
                  p2[1].y:=s1.cy+s2.cy div 4 +2;
                  p2[2].x:=s1.cx+s2.cx div 4 +2;
                  p2[2].y:=s1.cy+2;
                  p2[3].x:=s1.cx-s2.cx div 4 -2+s2.cx;
                  p2[3].y:=s1.cy+2;
                  p2[4].x:=s1.cx+s2.cx-2;
                  p2[4].y:=s1.cy+s2.cy div 4 +2;
                  p2[5].x:=s1.cx+s2.cx;
                  p2[5].y:=s1.cy+s2.cy div 4;
                  p2[6].x:=s1.cx-s2.cx div 4+s2.cx;
                  p2[6].y:=s1.cy;
                  p2[7].x:=s1.cx+s2.cx div 4;
                  p2[7].y:=s1.cy;
                  p2[8].x:=s1.cx;
                  p2[8].y:=s1.cy+s2.cy div 4;
                  p2[9].x:=s1.cx;
                  p2[9].y:=s1.cy-s2.cy div 4+s2.cy;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=LC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;

                  p2[0].x:=s1.cx+s2.cx-2;
                  p2[0].y:=s1.cy+s2.cy div 4 +2;
                  p2[1].x:=s1.cx+s2.cx-2;
                  p2[1].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  p2[2].x:=s1.cx-s2.cx div 4 -2+s2.cx;
                  p2[2].y:=s1.cy+s2.cy-2;
                  p2[3].x:=s1.cx+s2.cx div 4 +2;
                  p2[3].y:=s1.cy+s2.cy-2;
                  p2[4].x:=s1.cx+2;
                  p2[4].y:=s1.cy-s2.cy div 4 -2+s2.cy;
                  p2[5].x:=s1.cx;
                  p2[5].y:=s1.cy-s2.cy div 4 +s2.cy;
                  p2[6].x:=s1.cx+s2.cx div 4 ;
                  p2[6].y:=s1.cy+s2.cy;
                  p2[7].x:=s1.cx-s2.cx div 4 +s2.cx;
                  p2[7].y:=s1.cy+s2.cy;
                  p2[8].x:=s1.cx+s2.cx;
                  p2[8].y:=s1.cy-s2.cy div 4 +s2.cy;
                  p2[9].x:=s1.cx+s2.cx;
                  p2[9].y:=s1.cy+s2.cy div 4 ;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=DC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                end;
            end;
        end;
      iaccsStyle2 :
        begin
          if FControlType = iacctCheck then
            begin
              if Checked then
                begin
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5)*sin(i*2*pi/10));
                      p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2));
                      p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2));
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=DC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-2)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2));
                          p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2));
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-4)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2))-1;
                          p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2))-1;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=LC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-4)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2));
                          p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2));
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5)*sin(i*2*pi/10));
                      p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2));
                      p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2));
                    end;
                  Canvas.Brush.Style:=bsClear;
                  Canvas.Pen.Color:=clBlack;
                  Canvas.Pen.Style:=psSolid;
                  Canvas.Polygon(p2);
                  Canvas.Brush.Style:=bsSolid;
                  p4[0].x:=s1.cx+s2.cx div 2-5;
                  p4[0].y:=s1.cy+s2.cy div 2-5;
                  p4[1].x:=s1.cx+s2.cx div 2;
                  p4[1].y:=s1.cy+s2.cy div 2;
                  p4[2].x:=s1.cx+s2.cx div 2+5;
                  p4[2].y:=s1.cy+s2.cy div 2-9;
                  p4[3].x:=s1.cx+s2.cx div 2+1;
                  p4[3].y:=s1.cy+s2.cy div 2+7;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC2;
                  if LessColorDistanceThen(AC2,clBlack,128*3) then
                  Canvas.Pen.Color:=LC2 else
                  Canvas.Pen.Color:=clBlack;
                  Canvas.Pen.Style:=psSolid;
                  Canvas.Polygon(p4);

                end else
                begin
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5)*sin(i*2*pi/10));
                      p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2));
                      p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2));
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=DC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-2)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2));
                          p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2));
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-4)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2))-1;
                          p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2))-1;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=LC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-4)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2));
                          p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2));
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5)*sin(i*2*pi/10));
                      p2[i].x:=s1.cx+s2.cx div 2+round((dx+dy)/sqrt(2));
                      p2[i].y:=s1.cy+s2.cy div 2+round((-dx+dy)/sqrt(2));
                    end;
                  Canvas.Brush.Style:=bsClear;
                  Canvas.Pen.Color:=clBlack;
                  Canvas.Pen.Style:=psSolid;
                  Canvas.Polygon(p2);
                  Canvas.Brush.Style:=bsSolid;
                end;
            end else
            begin//select
              if Checked then
                begin
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5)*sin(i*2*pi/10));
                      p2[i].x:=s1.cx+s2.cx div 2+dx;
                      p2[i].y:=s1.cy+s2.cy div 2+dy;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=DC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-2)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+dx;
                          p2[i].y:=s1.cy+s2.cy div 2+dy;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-4)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+dx;
                          p2[i].y:=s1.cy+s2.cy div 2+dy-1;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=LC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-4)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+dx;
                          p2[i].y:=s1.cy+s2.cy div 2+dy;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5)*sin(i*2*pi/10));
                      p2[i].x:=s1.cx+s2.cx div 2+dx;
                      p2[i].y:=s1.cy+s2.cy div 2+dy;
                    end;
                  Canvas.Brush.Style:=bsClear;
                  Canvas.Pen.Color:=clBlack;
                  Canvas.Pen.Style:=psSolid;
                  Canvas.Polygon(p2);
                  Canvas.Brush.Style:=bsSolid;
                  p4[0].x:=s1.cx+s2.cx div 2-5;
                  p4[0].y:=s1.cy+s2.cy div 2-5;
                  p4[1].x:=s1.cx+s2.cx div 2;
                  p4[1].y:=s1.cy+s2.cy div 2;
                  p4[2].x:=s1.cx+s2.cx div 2+5;
                  p4[2].y:=s1.cy+s2.cy div 2-9;
                  p4[3].x:=s1.cx+s2.cx div 2+1;
                  p4[3].y:=s1.cy+s2.cy div 2+7;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC2;
                  if LessColorDistanceThen(AC2,clBlack,128*3) then
                  Canvas.Pen.Color:=LC2 else
                  Canvas.Pen.Color:=clBlack;
                  Canvas.Pen.Style:=psSolid;
                  Canvas.Polygon(p4);

                end else
                begin
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5)*sin(i*2*pi/10));
                      p2[i].x:=s1.cx+s2.cx div 2+dx;
                      p2[i].y:=s1.cy+s2.cy div 2+dy;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=DC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-2)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+dx;
                          p2[i].y:=s1.cy+s2.cy div 2+dy;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-4)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+dx;
                          p2[i].y:=s1.cy+s2.cy div 2+dy-1;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=LC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5-4)*sin(i*2*pi/10));
                          p2[i].x:=s1.cx+s2.cx div 2+dx;
                          p2[i].y:=s1.cy+s2.cy div 2+dy;
                    end;
                  Canvas.Brush.Style:=bsSolid;
                  Canvas.Brush.Color:=AC1;
                  Canvas.pen.Style:=psClear;
                  Canvas.Polygon(p2);
                  Canvas.pen.Style:=psSolid;
                  for i:=0 to 9 do
                    begin
                      dx:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/3)*cos(i*2*pi/10));
                      dy:=round((sqrt(sqr(s2.cx)+sqr(s2.cy))/5)*sin(i*2*pi/10));
                      p2[i].x:=s1.cx+s2.cx div 2+dx;
                      p2[i].y:=s1.cy+s2.cy div 2+dy;
                    end;
                  Canvas.Brush.Style:=bsClear;
                  Canvas.Pen.Color:=clBlack;
                  Canvas.Pen.Style:=psSolid;
                  Canvas.Polygon(p2);
                  Canvas.Brush.Style:=bsSolid;
                end;
            end;
        end;
      iaccsStyle3 :
        begin
          if FControlType = iacctCheck then
            begin
              if Checked then
                begin
                  Canvas.Brush.Color:=AC1;
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                  Canvas.Pen.Color:=LC1 else
                  Canvas.Pen.Color:=DC1;
                  Canvas.Rectangle(s1.cx,s1.cy,s1.cx+s2.cx,s1.cy+s2.cy);

                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                    begin
                      Canvas.Brush.Color:=DC2;
                      Canvas.Pen.Color:=DC1;
                    end else
                    begin
                      Canvas.Brush.Color:=LC2;
                      Canvas.Pen.Color:=LC1;
                    end;
                  Canvas.Rectangle(s1.cx+2,s1.cy+2,s1.cx+s2.cx-2,s1.cy+s2.cy-2);
                  p5[0].x :=s1.cx+s2.cx div 2 - s2.cx div 3 +2;
                  p5[0].y :=s1.cy+s2.cy div 2 - s2.cy div 3 +0;
                  p5[1].x :=s1.cx+s2.cx div 2                 ;
                  p5[1].y :=s1.cy+s2.cy div 2               -2;
                  p5[2].x :=s1.cx+s2.cx div 2 + s2.cx div 3 -2;
                  p5[2].y :=s1.cy+s2.cy div 2 - s2.cy div 3 +0;

                  p5[3].x :=s1.cx+s2.cx div 2 + s2.cx div 3 +0;
                  p5[3].y :=s1.cy+s2.cy div 2 - s2.cy div 3 +2;
                  p5[4].x :=s1.cx+s2.cx div 2               +2;
                  p5[4].y :=s1.cy+s2.cy div 2                 ;
                  p5[5].x :=s1.cx+s2.cx div 2 + s2.cx div 3 +0;
                  p5[5].y :=s1.cy+s2.cy div 2 + s2.cy div 3 -2;

                  p5[6].x :=s1.cx+s2.cx div 2 + s2.cx div 3 -2;
                  p5[6].y :=s1.cy+s2.cy div 2 + s2.cy div 3 +0;
                  p5[7].x :=s1.cx+s2.cx div 2                 ;
                  p5[7].y :=s1.cy+s2.cy div 2               +2;
                  p5[8].x :=s1.cx+s2.cx div 2 - s2.cx div 3 +2;
                  p5[8].y :=s1.cy+s2.cy div 2 + s2.cy div 3 +0;

                  p5[9].x :=s1.cx+s2.cx div 2 - s2.cx div 3 +0;
                  p5[9].y :=s1.cy+s2.cy div 2 + s2.cy div 3 -2;
                  p5[10].x:=s1.cx+s2.cx div 2               -2;
                  p5[10].y:=s1.cy+s2.cy div 2                 ;
                  p5[11].x:=s1.cx+s2.cx div 2 - s2.cx div 3 +0;
                  p5[11].y:=s1.cy+s2.cy div 2 - s2.cy div 3 +2;
                  Canvas.Brush.Color:=AC2;
                  if LessColorDistanceThen(AC2,clBlack,128*3) then
                  Canvas.Pen.Color:=LC2 else
                  Canvas.Pen.Color:=DC2;
                  Canvas.Polygon(p5);
                  p5[0].x :=s1.cx+s2.cx div 2 - s2.cx div 3 +1;
                  p5[0].y :=s1.cy+s2.cy div 2 - s2.cy div 3 +0;
                  p5[1].x :=s1.cx+s2.cx div 2                 ;
                  p5[1].y :=s1.cy+s2.cy div 2               -1;
                  p5[2].x :=s1.cx+s2.cx div 2 + s2.cx div 3 -1;
                  p5[2].y :=s1.cy+s2.cy div 2 - s2.cy div 3 +0;

                  p5[3].x :=s1.cx+s2.cx div 2 + s2.cx div 3 +0;
                  p5[3].y :=s1.cy+s2.cy div 2 - s2.cy div 3 +1;
                  p5[4].x :=s1.cx+s2.cx div 2               +1;
                  p5[4].y :=s1.cy+s2.cy div 2                 ;
                  p5[5].x :=s1.cx+s2.cx div 2 + s2.cx div 3 +0;
                  p5[5].y :=s1.cy+s2.cy div 2 + s2.cy div 3 -1;

                  p5[6].x :=s1.cx+s2.cx div 2 + s2.cx div 3 -1;
                  p5[6].y :=s1.cy+s2.cy div 2 + s2.cy div 3 +0;
                  p5[7].x :=s1.cx+s2.cx div 2                 ;
                  p5[7].y :=s1.cy+s2.cy div 2               +1;
                  p5[8].x :=s1.cx+s2.cx div 2 - s2.cx div 3 +1;
                  p5[8].y :=s1.cy+s2.cy div 2 + s2.cy div 3 +0;

                  p5[9].x :=s1.cx+s2.cx div 2 - s2.cx div 3 +0;
                  p5[9].y :=s1.cy+s2.cy div 2 + s2.cy div 3 -1;
                  p5[10].x:=s1.cx+s2.cx div 2               -1;
                  p5[10].y:=s1.cy+s2.cy div 2                 ;
                  p5[11].x:=s1.cx+s2.cx div 2 - s2.cx div 3 +0;
                  p5[11].y:=s1.cy+s2.cy div 2 - s2.cy div 3 +1;

                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                    begin
                      Canvas.Brush.Color:=LC1;
                      Canvas.Pen.Color:=DC1;
                    end else
                    begin
                      Canvas.Brush.Color:=DC1;
                      Canvas.Pen.Color:=LC1;
                    end;
                  Canvas.Polygon(p5);
                end else
                begin
                  Canvas.Brush.Color:=AC1;
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                  Canvas.Pen.Color:=LC1 else
                  Canvas.Pen.Color:=DC1;
                  Canvas.Rectangle(s1.cx,s1.cy,s1.cx+s2.cx,s1.cy+s2.cy);
                  Canvas.Brush.Color:=Color;
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                  Canvas.Pen.Color:=DC1 else
                  Canvas.Pen.Color:=LC1;
                  Canvas.Rectangle(s1.cx+2,s1.cy+2,s1.cx+s2.cx-2,s1.cy+s2.cy-2);
                end;
            end else
            begin//sellect
              if Checked then
                begin
                  Canvas.Brush.Color:=AC1;
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                  Canvas.Pen.Color:=LC1 else
                  Canvas.Pen.Color:=DC1;
                  Canvas.Ellipse(s1.cx+1,s1.cy+1,s1.cx+s2.cx-1,s1.cy+s2.cy-1);
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                    begin
                      Canvas.Brush.Color:=DC2;
                      Canvas.Pen.Color:=DC1;
                    end else
                    begin
                      Canvas.Brush.Color:=LC2;
                      Canvas.Pen.Color:=LC1;
                    end;
                  Canvas.Ellipse(s1.cx+3,s1.cy+3,s1.cx+s2.cx-3,s1.cy+s2.cy-3);
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                    begin
                      Canvas.Brush.Color:=LC1;
                      Canvas.Pen.Color:=AC2;
                    end else
                    begin
                      Canvas.Brush.Color:=DC1;
                      Canvas.Pen.Color:=AC2;
                    end;
                  Canvas.Ellipse(s1.cx+4,s1.cy+4,s1.cx+s2.cx-4,s1.cy+s2.cy-4);
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                    begin
                      Canvas.Brush.Color:=AC2;
                      Canvas.Pen.Color:=LC1;
                    end else
                    begin
                      Canvas.Brush.Color:=AC2;
                      Canvas.Pen.Color:=DC1;
                    end;
                  Canvas.Ellipse(s1.cx+5,s1.cy+5,s1.cx+s2.cx-5,s1.cy+s2.cy-5);
                end else
                begin
                  Canvas.Brush.Color:=AC1;
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                  Canvas.Pen.Color:=LC1 else
                  Canvas.Pen.Color:=DC1;
                  Canvas.Ellipse(s1.cx+1,s1.cy+1,s1.cx+s2.cx-1,s1.cy+s2.cy-1);
                  Canvas.Brush.Color:=Color;
                  if LessColorDistanceThen(AC1,clBlack,128*3) then
                  Canvas.Pen.Color:=DC1 else
                  Canvas.Pen.Color:=LC1;
                  Canvas.Ellipse(s1.cx+3,s1.cy+3,s1.cx+s2.cx-3,s1.cy+s2.cy-3);
                end;
            end;
        end;//iaccsStyle3    
    end;
end;

procedure TCheckControl.DrawFocus;
var s1,s2,s5 : TSize;
    RGN2 : HRGN;
    P1 : array [0..3] of Tpoint;
    Ac : TColor;
begin
  if HandleAllocated then
    begin

      s1:=GetBasis;
      s2:=GetOffset;
      s5:=GetTextSize;
      s2.cx:=s2.cx+round(2*cos(FTextAngle*pi/1800)+2*sin(FTextAngle*pi/1800));
      s2.cy:=s2.cy+round(-2*sin(FTextAngle*pi/1800)+2*cos(FTextAngle*pi/1800));
      s5.cx:=s5.cx-4;
      s5.cy:=s5.cy-4;
      p1[0].x:=s1.cx+s2.cx;
      p1[0].y:=s1.cy+s2.cy;
      p1[1].x:=s1.cx+s2.cx+round((s5.cx*cos(FTextAngle*pi/1800)));
      p1[1].y:=s1.cy+s2.cy-round((s5.cx*sin(FTextAngle*pi/1800)));
      p1[2].x:=p1[1].x+round((s5.cy*sin(FTextAngle*pi/1800)));
      p1[2].y:=p1[1].y+round((s5.cy*cos(FTextAngle*pi/1800)));
      p1[3].x:=s1.cx+s2.cx+round((s5.cy*sin(FTextAngle*pi/1800)));
      p1[3].y:=s1.cy+s2.cy+round((s5.cy*cos(FTextAngle*pi/1800)));
      RGN2:=CreatePolygonRGN(P1,4,Alternate);
      AC:=GetCColor(Color,clBlack,100,30);
      Canvas.Brush.Color:=Ac;
      FrameRgn(Canvas.Handle,RGN2,Canvas.Brush.Handle,1,1);

      Deleteobject(RGN2);
    end;
end;

procedure TCheckControl.FontIsChange(Sender: TObject);
begin
  SetProperlyRGN;
  invalidate;
end;

function TCheckControl.GetAutoSize: Boolean;
begin
  result:=inherited AutoSize;
end;

function Max(a:integer;b:integer):integer;
begin
  if a>b then
  begin
     Result:=a;
  end
  else
  begin
     Result:=b;
  end;
end;

function TCheckControl.GetBasis: TSize;
var s1,s2,s3 : TSize;
begin
  s1:=GetBmpSize;
  s2:=GetTextSize;
  s3.cx:=round(abs(s2.cx*cos(FTextAngle*pi/1800.0))+abs(s2.cy*sin(FTextAngle*pi/1800.0)));
  s3.cy:=round(abs(s2.cx*sin(FTextAngle*pi/1800.0))+abs(s2.cy*cos(FTextAngle*pi/1800.0)));
  result.cx:=0;
  result.cy:=0;
  case FTextFieldVerticalAlign of
    iacctvaTop :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=s1.cx+FGapTextToImage;
              result.cy:=0;
            end;
          iaccthaRight :
            begin
              result.cx:=0;
              result.cy:=0;
            end;
          iaccthaCenter :
            begin
              result.cx:=Max(0,(s1.cx-s3.cx) div 2);
              result.cy:=s1.cy+FGapTextToImage;
            end;
        end;
      end;
    iacctvaBottom :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=s1.cx+FGapTextToImage;
              result.cy:=Max(0,s1.cy-s3.cy);
            end;
          iaccthaRight :
            begin
              result.cx:=0;
              result.cy:=Max(0,s1.cy-s3.cy);
            end;
          iaccthaCenter :
            begin
              result.cx:=Max(0,(s1.cx-s3.cx) div 2);
              result.cy:=0;
            end;
        end;
      end;
    iacctvaCenter :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=s1.cx+FGapTextToImage;
              result.cy:=Max(0,(s1.cy-s3.cy) div 2);
            end;
          iaccthaRight :
            begin
              result.cx:=0;
              result.cy:=Max(0,(s1.cy-s3.cy) div 2);
            end;
          iaccthaCenter :
            begin
              //No Action cause unavaible...
            end;
         end;
      end;
  end;
end;

function TCheckControl.GetBMPBasis: TSize;
var s1,s2,s3 : TSize;
begin
  s1:=GetBmpSize;
  s2:=GetTextSize;
  s3.cx:=round(abs(s2.cx*cos(FTextAngle*pi/1800.0))+abs(s2.cy*sin(FTextAngle*pi/1800.0)));
  s3.cy:=round(abs(s2.cx*sin(FTextAngle*pi/1800.0))+abs(s2.cy*cos(FTextAngle*pi/1800.0)));
  result.cx:=0;
  result.cy:=0;
  case FTextFieldVerticalAlign of
    iacctvaTop :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=0;
              result.cy:=0;
            end;
          iaccthaRight :
            begin
              result.cx:=s3.cx+FGapTextToImage;
              result.cy:=0;
            end;
          iaccthaCenter :
            begin
              result.cx:=Max(0,-(s1.cx-s3.cx) div 2);
              result.cy:=0;
            end;
        end;
      end;
    iacctvaBottom :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=0;
              result.cy:=Max(0,-s1.cy+s3.cy);
            end;
          iaccthaRight :
            begin
              result.cx:=s3.cx+FGapTextToImage;
              result.cy:=Max(0,-s1.cy+s3.cy);
            end;
          iaccthaCenter :
            begin
              result.cx:=Max(0,-(s1.cx-s3.cx) div 2);
              result.cy:=s3.cy+FGapTextToImage;
            end;
        end;
      end;
    iacctvaCenter :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=0;
              result.cy:=Max(0,-(s1.cy-s3.cy) div 2);
            end;
          iaccthaRight :
            begin
              result.cx:=s3.cx+FGapTextToImage;
              result.cy:=Max(0,-(s1.cy-s3.cy) div 2);
            end;
          iaccthaCenter :
            begin
              //No Action cause unavaible...
            end;
         end;
      end;
  end;
end;

function TCheckControl.GetBMPSize: Tsize;
begin
  case Style of
    iaccsStyle1,iaccsStyle2 :
      begin
        result.cx:=20;
        result.cy:=20;
      end;
    iaccsStyle1Small,iaccsStyle3 :
      begin
        result.cx:=16;
        result.cy:=16;
      end;
    iaccsBitmap :
      begin
        if assigned(ImageList) then
          begin
            result.cx:=ImageList.Width;
            result.cy:=ImageList.Height;
          end else
          begin
            result.cx:=20;
            result.cy:=20;
          end;
      end;
  end;
end;

function TCheckControl.GetColor: TColor;
begin
  result:=inherited Color;
end;

function TCheckControl.GetCurrOffset(i: Integer): TSize;
var s1,s2 : TSize;
    Asin,Acos : Double;
    j,dx,dy  : integer;
    s : string;
begin
  if i<=(FTextLines.Count-1) then
    begin
      dx:=0;dy:=0;
      s2:=GetTextSize;
      for j:=0 to i do
        begin
          s:=FTextLines.Strings[j];
          Canvas.Font.Assign(Font);
          GetTextExtentPoint32(Canvas.Handle,PChar(@s[1]),length(s),s1);
          if j<>i then
            begin
              dy:=dy+s1.cy;
            end else
            begin
              case TextAlign of
                iaccthaLeft :
                  begin
                    dx:=0;
                  end;
                iaccthaRight :
                  begin
                    dx:=s2.cx-s1.cx;
                  end;
                iaccthaCenter :
                  begin
                    dx:=(s2.cx-s1.cx) div 2;
                  end;
              end;
            end;
        end;

      ACos:=cos(FTextAngle*pi/1800.0);
      ASin:=sin(FTextAngle*pi/1800.0);
      Result.cx:=round(dx*ACos+dy*ASin);
      Result.cy:=round(-dx*ASin+dy*ACos);
    end else
    begin
      Result.cx:=0;
      Result.cy:=0;
    end;
end;

function TCheckControl.GetEnabled1: Boolean;
begin
  result := inherited Enabled;
end;

function TCheckControl.GetFont: TFont;
begin
  result:=inherited Font;
end;

function TCheckControl.GetFullSize: TSize;
var s1,s2,s3 : TSize;
begin
  s1:=GetBmpSize;
  s2:=GetTextSize;
  s3.cx:=round(abs(s2.cx*cos(FTextAngle*pi/1800.0))+abs(s2.cy*sin(FTextAngle*pi/1800.0)));
  s3.cy:=round(abs(s2.cx*sin(FTextAngle*pi/1800.0))+abs(s2.cy*cos(FTextAngle*pi/1800.0)));
  result.cx:=0;
  result.cy:=0;
  case FTextFieldVerticalAlign of
    iacctvaTop :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=s1.cx+s3.cx+FGapTextToImage;
              result.cy:=Max(s1.cy,s3.cy);
            end;
          iaccthaRight :
            begin
              result.cx:=s1.cx+s3.cx+FGapTextToImage;
              result.cy:=Max(s1.cy,s3.cy);
            end;
          iaccthaCenter :
            begin
              result.cx:=Max(s1.cx,s3.cx);
              result.cy:=s1.cy+s3.cy+FGapTextToImage;
            end;
        end;
      end;
    iacctvaBottom :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=s1.cx+s3.cx+FGapTextToImage;
              result.cy:=Max(s1.cy,s3.cy);
            end;
          iaccthaRight :
            begin
              result.cx:=s1.cx+s3.cx+FGapTextToImage;
              result.cy:=Max(s1.cy,s3.cy);
            end;
          iaccthaCenter :
            begin
              result.cx:=Max(s1.cx,s3.cx);
              result.cy:=s1.cy+s3.cy+FGapTextToImage;
            end;
        end;
      end;
    iacctvaCenter :
      begin
        case FTextFieldHorizontalAlign of
          iaccthaLeft :
            begin
              result.cx:=s1.cx+s3.cx+FGapTextToImage;
              result.cy:=Max(s1.cy,s3.cy);
            end;
          iaccthaRight :
            begin
              result.cx:=s1.cx+s3.cx+FGapTextToImage;
              result.cy:=Max(s1.cy,s3.cy);
            end;
          iaccthaCenter :
            begin
              //No Action cause unavaible...
            end;
         end;
      end;
  end;
end;

function TCheckControl.GetHeight: Integer;
begin
  result:=inherited Height;
end;

function TCheckControl.GetOffset: TSize;
var s2,s3 : TSize;
    Asin,Acos : Double;
begin
  s2:=GetTextSize;
  s3.cx:=round(abs(s2.cx*cos(FTextAngle*pi/1800.0))+abs(s2.cy*sin(FTextAngle*pi/1800.0)));
  s3.cy:=round(abs(s2.cx*sin(FTextAngle*pi/1800.0))+abs(s2.cy*cos(FTextAngle*pi/1800.0)));
  ACos:=cos(FTextAngle*pi/1800.0);
  ASin:=sin(FTextAngle*pi/1800.0);
  if ASin>=0 then
    begin
      if Acos>=0 then
        begin
          Result.cx:=0;
          Result.cy:=round(s2.cx*ASin);
        end else
        begin
          Result.cx:=-round(s2.cx*ACos);
          Result.cy:=s3.cy;
        end;
    end else
    begin
      if Acos>=0 then
        begin
          Result.cx:=round(s3.cx-s2.cx*ACos);
          Result.cy:=0;
        end else
        begin
          Result.cx:=s3.cx;
          Result.cy:=round(s3.cy+s2.cx*ASin);
        end;
    end;
end;

function TCheckControl.GetText: TCaption;
begin
  result:=inherited Text;
end;

function TCheckControl.GetTextSize: TSize;
var i : integer;
    s : string;
    S1 : TSize;
begin
  result.cx:=0;
  result.cy:=0;
  for i:=0 to FTextLines.Count-1 do
    begin
      s:=FTextLines.Strings[i];
      Canvas.Font.Assign(Font);
      GetTextExtentPoint32(Canvas.Handle,PChar(@s[1]),length(s),s1);
      if result.cx<s1.cx then result.cx:=s1.cx;
      result.cy:=result.cy+s1.cy;
    end;
end;

function TCheckControl.GetVisible: Boolean;
begin
  result:=inherited Visible;
end;

function TCheckControl.GetWidth: Integer;
begin
  result:=inherited Width;
end;

function TCheckControl.LessColorDistanceThen(C1, C2: TColor;
  ADistance: Integer): Boolean;
var AD : integer;
begin
  AD:=abs(GetRValue(ColorToRGB(C1))-GetRValue(ColorToRGB(C2)))+
      abs(GetGValue(ColorToRGB(C1))-GetGValue(ColorToRGB(C2)))+
      abs(GetBValue(ColorToRGB(C1))-GetBValue(ColorToRGB(C2)));
  if AD<ADistance then Result:=True else Result:=false;
end;

procedure TCheckControl.Paint;
var s1,ABase,AOffset,ACurrOffset : TSize;
    i : integer;
    DC,LC : TColor;
begin
  if Visible or (csDesigning in ComponentState)then
    begin
      s1:=GetFullSize;
      if AutoSize then
        begin
          if (width<>s1.cx) or (height<>s1.cy) then SetBounds(Left,Top, s1.cx, s1.cy);
        end;
      canvas.Brush.Color:=Color;
      canvas.FillRect(rect(0,0,width,height));
      LC:=GetCColor(Color,clWhite,100,30);
      DC:=GetCColor(Color,clBlack,100,30);
      ABase:=GetBasis;
      AOffset:=GetOffset;
      Canvas.Brush.Style:=bsClear;
      for i:=0 to FTextLines.Count-1 do
        begin
          ACurrOffset:=GetCurrOffset(i);
          canvas.font.Assign(Font);

          rotatefont(canvas.font,TextAngle);
          case FText3DStyle of
            iscct3dsSimple :
              begin
                if not Enabled then Canvas.Font.Color:=GetCColor(Font.Color,Color,100,50) else
                                    Canvas.Font.Color:=Font.Color;
                canvas.TextOut(ABase.Cx+AOffset.Cx+ACurrOffset.cx,ABase.cy+AOffset.Cy+ACurrOffset.cy,FTextLines[i]);
              end;
            iscct3dsUp :
              begin
                Canvas.Font.Color:=DC;
                canvas.TextOut(ABase.Cx+AOffset.Cx+ACurrOffset.cx+1,ABase.cy+AOffset.Cy+ACurrOffset.cy+1,FTextLines[i]);
                Canvas.Font.Color:=LC;
                canvas.TextOut(ABase.Cx+AOffset.Cx+ACurrOffset.cx-1,ABase.cy+AOffset.Cy+ACurrOffset.cy-1,FTextLines[i]);
                if not Enabled then Canvas.Font.Color:=GetCColor(Font.Color,Color,100,50) else
                                    Canvas.Font.Color:=Font.Color;
                canvas.TextOut(ABase.Cx+AOffset.Cx+ACurrOffset.cx,ABase.cy+AOffset.Cy+ACurrOffset.cy,FTextLines[i]);
              end;
            iscct3dsDown :
              begin
                Canvas.Font.Color:=LC;
                canvas.TextOut(ABase.Cx+AOffset.Cx+ACurrOffset.cx+1,ABase.cy+AOffset.Cy+ACurrOffset.cy+1,FTextLines[i]);
                Canvas.Font.Color:=DC;
                canvas.TextOut(ABase.Cx+AOffset.Cx+ACurrOffset.cx-1,ABase.cy+AOffset.Cy+ACurrOffset.cy-1,FTextLines[i]);
                canvas.TextOut(ABase.Cx+AOffset.Cx+ACurrOffset.cx,ABase.cy+AOffset.Cy+ACurrOffset.cy,FTextLines[i]);
                if not Enabled then Canvas.Font.Color:=GetCColor(Font.Color,Color,100,50) else
                                    Canvas.Font.Color:=Font.Color;
                canvas.TextOut(ABase.Cx+AOffset.Cx+ACurrOffset.cx,ABase.cy+AOffset.Cy+ACurrOffset.cy,FTextLines[i]);
              end;
          end;

        end;
      Canvas.Brush.Style:=bsSolid;  
      if FShowFocus and Focused and Enabled then
        begin
          DrawFocus;
        end;  
      DrawCheckBMP;
    end;
end;
procedure TCheckControl.RotateFont(AFont: TFont; ADegDeci: Integer);
var
  MyLFont : LogFONT;
begin
  GETOBJECT(AFont.Handle,sizeof(MyLFont),@MyLFont);
  MyLfont.lfEscapement:=ADegDeci;
  MyLFont.lfOrientation:=ADegDeci;
  MyLFont.lfQuality:=PROOF_QUALITY;//
  AFont.Handle:=CREATEFONTINDIRECT(MyLFont);//As i think it will automatically delete previouse object...
  //as windows delete object...
end;
procedure TCheckControl.SetAutoSize(Value: Boolean);
begin
  inherited AutoSize:=Value;
  SetProperlyRGN;
end;

procedure TCheckControl.SetBounds(ALeft, ATop, AWidth, AHeight: Integer);
begin
  inherited SetBounds(ALeft, ATop, AWidth, AHeight);
end;

procedure TCheckControl.SetChecked(Value: Boolean);
var i : integer;
begin
  if Value<>FChecked then
    begin
      FChecked := Value;
      if FChecked and (FControlType=iacctSelect) then
        begin
          if assigned(Parent) and Parent.HandleAllocated then
            begin
              for i:=0 to Parent.ControlCount-1 do
                begin
                  if (Parent.Controls[i] is  TCheckControl) and
                     ((Parent.Controls[i] as  TCheckControl)<>self) and
                     ((Parent.Controls[i] as  TCheckControl).FControlType=iacctSelect) and
                     ((Parent.Controls[i] as  TCheckControl).FGroupIndex=FGroupIndex) then
                     (Parent.Controls[i] as  TCheckControl).Checked:=false;
                end;
            end;
        end;
      if assigned(FOnChangeState) then FOnChangeState(self,FChecked);
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetCheckFieldColor1(Value: TColor);
begin
  if Value<>FCheckFieldColor1 then
    begin
      FCheckFieldColor1 := Value;
      invalidate;
    end;
end;

procedure TCheckControl.SetCheckFieldColor2(Value: TColor);
begin
  if Value<>FCheckFieldColor2 then
    begin
      FCheckFieldColor2 := Value;
      invalidate;
    end;
end;

procedure TCheckControl.SetColor(Value: TColor);
begin
  inherited Color:=Value;
end;

procedure TCheckControl.SetControlType(Value: TCheckControlType);
var i : integer;
begin
  if Value<>FControlType then
    begin
      FControlType := Value;
      if FChecked and (FControlType=iacctSelect) then
        begin
          if assigned(Parent) and Parent.HandleAllocated then
            begin
              for i:=0 to Parent.ControlCount-1 do
                begin
                  if (Parent.Controls[i] is  TCheckControl) and
                     ((Parent.Controls[i] as  TCheckControl)<>self) and
                     ((Parent.Controls[i] as  TCheckControl).FGroupIndex=FGroupIndex) and
                     ((Parent.Controls[i] as  TCheckControl).FControlType=iacctSelect) then
                     (Parent.Controls[i] as  TCheckControl).Checked:=false;
                end;
            end;
        end;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetEnabled1(Value: Boolean);
begin
  inherited Enabled:=Value;
  SetProperlyRGN;
  invalidate;
end;

procedure TCheckControl.SetFont(Value: TFont);
begin
  inherited Font:=Value;
  SetProperlyRGN;
end;

procedure TCheckControl.SetGapTextToImage(Value: Byte);
begin
  if Value<>FGapTextToImage then
    begin
      FGapTextToImage := Value;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetGroupIndex(Value: Cardinal);
begin
  if Value<>FGroupIndex then
    begin
      FGroupIndex := Value;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetHeight(Value: Integer);
begin
  inherited Height:=Value;
  SetProperlyRGN;
end;

procedure TCheckControl.SetOnChangeState(Value: TCheckControlChangeStateProcedure);
begin
  FOnChangeState := Value;
end;

procedure TCheckControl.SetProperlyRGN;
var s1,s2,s3,s4,s5 : TSize;
    RGN1,RGN2 : HRGN;
    P1 : array [0..3] of Tpoint;
begin
  if HandleAllocated then
    begin
      if FTransparent then
        begin
          s1:=GetBasis;
          s2:=GetOffset;
          s5:=GetTextSize;
          s3:=GetBmpBasis;
          s4:=GetBmpSize;
          RGN1:=CreateRectRGN(s3.cx,s3.cy,s3.cx+s4.cx,s3.cy+s4.cy);
          p1[0].x:=s1.cx+s2.cx;
          p1[0].y:=s1.cy+s2.cy;
          p1[1].x:=s1.cx+s2.cx+round((s5.cx*cos(FTextAngle*pi/1800)));
          p1[1].y:=s1.cy+s2.cy-round((s5.cx*sin(FTextAngle*pi/1800)));
          p1[2].x:=p1[1].x+round((s5.cy*sin(FTextAngle*pi/1800)));
          p1[2].y:=p1[1].y+round((s5.cy*cos(FTextAngle*pi/1800)));
          p1[3].x:=s1.cx+s2.cx+round((s5.cy*sin(FTextAngle*pi/1800)));
          p1[3].y:=s1.cy+s2.cy+round((s5.cy*cos(FTextAngle*pi/1800)));
          RGN2:=CreatePolygonRGN(P1,4,Alternate);
          CombineRGN(RGN1,RGN1,RGN2,RGN_OR);
          deleteobject(RGN2);
          setwindowrgn(handle,RGN1,TRUE);
        end else
        begin
          setwindowrgn(handle,0,TRUE);
        end;
    end;
end;

procedure TCheckControl.SetShowFocus(Value: Boolean);
begin
  if Value<>FShowFocus then
    begin
      FShowFocus := Value;
      invalidate;
    end;
end;

procedure TCheckControl.SetStyle(Value: TCheckControlStyle);
begin
  if Value<>FStyle then
    begin
      FStyle := Value;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetText(Value: TCaption);
begin
  inherited Text:=Value;
  FTextLines.Text:=Value;
  SetProperlyRGN;
  invalidate;
end;

procedure TCheckControl.SetText3DStyle(Value: TCheckControlText3DStyle);
begin
  if Value<>FText3DStyle then
    begin
      FText3DStyle := Value;
      invalidate;
    end;
end;

procedure TCheckControl.SetTextAlign(Value: TCheckControlTextHAlign);
begin
  if Value<>FTextAlign then
    begin
      FTextAlign := Value;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetTextAngle(Value: integer);
begin
  while Value>1800  do dec(Value,3600);
  while Value<=-1800 do inc(Value,3600);
  if Value<>FTextAngle then
    begin
      FTextAngle := Value;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetTextFieldHorizontalAlign(Value: TCheckControlTextHAlign);
begin
  if (Value<>FTextFieldHorizontalAlign) then
    begin
      FTextFieldHorizontalAlign := Value;
      //if Value=iaccthaCenter then FTextFieldVerticalAlign:=iacctvaTop;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetTextFieldVerticalAlign(Value: TCheckControlTextVAlign);
begin
  if (Value<>FTextFieldVerticalAlign) then
    begin
      FTextFieldVerticalAlign := Value;
      //if Value=iacctvaCenter then  FTextFieldHorizontalAlign:=iaccthaLeft;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetTextLines(Value: TStringList);
begin
  if Value=nil then
    begin
      FTextLines.Clear;
      inherited Text:='';
    end else
    begin
      FTextLines.Clear;
      FTextLines.Text:=Value.Text;
      inherited Text :=Value.Text;
    end;
  SetProperlyRGN;  
  invalidate;  
end;

procedure TCheckControl.SetTransparent(Value: Boolean);
begin
  if Value<>FTransparent then
    begin
      FTransparent := Value;
      SetProperlyRGN;
      invalidate;
    end;
end;

procedure TCheckControl.SetVisible(Value: Boolean);
begin
 inherited Visible:=Value;
 invalidate;
end;

procedure TCheckControl.SetWidth(Value: Integer);
begin
  inherited Width:=Value;
  SetProperlyRGN;
end;

procedure TCheckControl.WMKeyUp(var Message: TWMKeyUp);
begin
  inherited;
  if Enabled and (Message.CharCode=32) then
    begin
      case FControlType of
        iacctCheck :Checked:=not Checked;
        iacctSelect : Checked:=TRUE;
      end;
    end;
end;

procedure TCheckControl.WMKILLFOCUS(var Message: TMessage);
begin
  inherited;
  invalidate;
end;

procedure TCheckControl.WMLButtonDblClk(var Message: TWMLButtonDblClk);
begin
  perform(WM_LButtonDown,0,0);
end;

procedure TCheckControl.WMLButtonDown(var Message: TWMLButtonDown);
begin
  inherited;
  if Enabled then
    begin
      case FControlType of
        iacctCheck :Checked:=not Checked;
        iacctSelect : Checked:=TRUE;
      end;
      self.SetFocus;
    end;
end;

procedure TCheckControl.WMSETFOCUS(var Message: TMessage);
begin
  inherited;
  invalidate;
end;

procedure TCheckControl.WMSIZE(var Message: TMessage);
begin
  inherited;
  SetProperlyRGN;
end;

constructor TTransparentGroupBox.Create (AOwner: TComponent);
Begin
  Inherited Create(AOwner);
  ControlStyle:= ControlStyle - [csOpaque];
  FBevelLowColor := clBtnShadow;
  FBevelHiColor := clBtnHighlight;
End;

Procedure TTransparentGroupBox.CreateParams(Var Params:TCreateParams);
Begin
  inherited CreateParams (Params);
  Params.ExStyle := Params.ExStyle + WS_EX_TRANSPARENT;
End;

Procedure TTransparentGroupBox.Paint;
var
  H: Integer;
  R: TRect;
begin
  with Canvas do
  begin
    Font := Self.Font;
    H := TextHeight('0');
    R := Rect(0, H div 2 - 1, Width, Height);
    if Ctl3D then
    begin
      DrawFrame(R);
    end else
    begin
      Brush.Color := clWindowFrame;
      FrameRect(R);
    end;
    if Text <> '' then
    begin
      R := Rect(8, 0, 0, H);
      DrawText(Handle, PChar(Text), Length(Text), R, DT_LEFT or
DT_SINGLELINE or
        DT_CALCRECT);
      if Ctl3D then
        Brush.Style:= bsClear
      else
        Brush.Color := Color;
      DrawText(Handle, PChar(Text), Length(Text), R, DT_LEFT or
DT_SINGLELINE);
    end;
  end;
End;

Procedure TTransparentGroupBox.WMEraseBkGnd(Var Message:TWMEraseBkGnd);
Begin
  Repaint;
End;

Procedure TTransparentGroupBox.SetParent(AParent:TWinControl);
Begin
  Inherited SetParent(AParent);
  If Parent <> Nil then
    SetWindowLong(Parent.Handle, GWL_STYLE,
       GetWindowLong(Parent.Handle, GWL_STYLE)
          And Not WS_ClipChildren);
End;

Procedure TTransparentGroupBox.Invalidate;
Var
  Rect :TRect;
Begin
  Rect:= BoundsRect;
  If (Parent <> Nil) and Parent.HandleAllocated then
    InvalidateRect(Parent.Handle, @Rect, True)
  Else
    Inherited Invalidate;
End;

Procedure TTransparentGroupBox.WMMove(Var Message:TWMMove);
Begin
  Invalidate;
End;

procedure TTransparentGroupBox.SetBvlLowColor(Value: TColor);
begin
  if FBevelLowColor <> Value then
    FBevelLowColor := Value;
  Invalidate;
end;

procedure TTransparentGroupBox.SetBvlHiColor(Value: TColor);
begin
  if FBevelHiColor <> Value then
    FBevelHiColor := Value;
  Invalidate;
end;

procedure TTransparentGroupBox.DrawFrame(Rect: TRect);
var
  CaptionLength: Integer;
begin
  with Canvas do
  begin
    Inc(Rect.Left);
    Inc(Rect.Top);
    Dec(Rect.Right);
    Dec(Rect.Bottom);
    CaptionLength := TextWidth(Text);
    Pen.Color := FBevelHiColor;
    MoveTo(Rect.Left, Rect.Top);
    LineTo(6,Rect.Top);
    MoveTo(8+CaptionLength+2, Rect.Top);
    LineTo(Rect.Right,Rect.Top);
    LineTo(Rect.Right,Rect.Bottom);
    LineTo(Rect.Left,Rect.Bottom);
    LineTo(Rect.Left, Rect.Top);
    OffsetRect(Rect, -1, -1);
    Pen.Color := FBevelLowColor;
    MoveTo(Rect.Left, Rect.Top);
    LineTo(6,Rect.Top);
    MoveTo(8+CaptionLength+2, Rect.Top);
    LineTo(Rect.Right,Rect.Top);
    LineTo(Rect.Right,Rect.Bottom);
    LineTo(Rect.Left,Rect.Bottom);
    LineTo(Rect.Left, Rect.Top);
  end;
end;

initialization
  begin
    SupportsQuality:=false;
    VInfo.dwOSVersionInfoSize:=sizeof(TOSVERSIONINFO);
    GetVersionEx(Vinfo);
    Case VInfo.dwPlatformId of
      VER_PLATFORM_WIN32_WINDOWS://95,98....
        begin
          if (Vinfo.dwMajorVersion>=4) and (Vinfo.dwMinorVersion>0) then SupportsQuality:=True;

        end;
      VER_PLATFORM_WIN32_NT:
        begin
          if (Vinfo.dwMajorVersion>=4) then SupportsQuality:=True;
        end;
    end;
  end;

end.
