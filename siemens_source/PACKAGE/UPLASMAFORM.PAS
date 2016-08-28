unit UPlasmaForm;
//version 1.1

{==============================================================================
v1.0      :  01/12/98 - first version

v1.1      :  01/04/99 - LoadMaskFromResource method added
                        (sugestion by Konstantin Leonidov)
                      - Set the form Scaled property to false
                      - The mask is no more cleared when the bitmap changes
==============================================================================}

interface

uses
	Windows, Messages, SysUtils, Classes, Graphics, Controls, Forms, Dialogs,
	ExtCtrls ,dsgnintf, Buttons, StdCtrls,UPLasmaEd;

type

  	TPlasmaForm = class(TImage)
	              	private
                 FMoveable : Boolean;
                 FMoveX    : Integer;
                 FMoveY    : Integer;
              			FRegion   : TPlasmaRegion;
                 procedure   RefreshForm;
                 procedure   PictureUpdate(Sender:TObject);
              			procedure   MouseDown(Button: TMouseButton; Shift: TShiftState;X,Y:Integer);override;
              		 procedure   MouseMove(Shift: TShiftState;X, Y: Integer);override;
                protected
              			procedure   SetRegion(Value:TPlasmaRegion);
              			function    GetRegion:TPlasmaRegion;
              			procedure   SetParent(Value:TWinControl);override;
              		public
              			constructor Create(Aowner:TComponent); override;
              			destructor	 Destroy; override;
                 procedure   Loaded;override;
              			procedure   LoadMaskFromFile(Value:String);
                 procedure   LoadMaskFromResource(Instance:THandle;ResName:string);
              		published
              			property    Mask:TPlasmaRegion read GetRegion write SetRegion;
                 property    Moveable:Boolean   read FMoveable write FMoveable;
                end;

procedure Register;

implementation

{==============================================================================}
constructor TPlasmaForm.Create(Aowner:TComponent);
begin
inherited Create(AOwner);
Align:=alClient;
AutoSize:=True;
FRegion:=TPlasmaRegion.Create(Self);
Picture.OnChange:=PictureUpdate;
FMoveable:=True;
end;

destructor TPlasmaForm.Destroy;
begin
FRegion.Free;
inherited Destroy;
end;

procedure TPlasmaForm.Loaded;
begin
inherited Loaded;
RefreshForm;
end;
{==============================================================================}
procedure TPlasmaForm.RefreshForm;
begin
If (not (csDesigning in ComponentState)) then
    			SetWindowRgn(Parent.Handle,FRegion.Region,true)
end;
{==============================================================================}
procedure TPlasmaForm.PictureUpdate(Sender:TObject);
begin
If Not Picture.Bitmap.Empty then
   begin
   Parent.ClientWidth:=Picture.Width;
   Parent.ClientHeight:=Picture.Height;
   end;
end;

procedure TPlasmaForm.SetParent(Value:TWinControl);
begin
inherited SetParent(Value);
if Value<>nil then
   if (Value is TForm) then
      begin
      TForm(Value).BorderStyle:=bsNone;
      TForm(Value).Scaled:=false;
      end
     else
   			raise Exception.Create('Please Drop on a Form')
end;
{==============================================================================}
procedure TPlasmaForm.SetRegion(Value:TPlasmaRegion);
begin
if Value<>nil then FRegion:=Value;
RefreshForm;
end;

function TPlasmaForm.GetRegion:TPlasmaRegion;
begin
Result:=FRegion;
end;
{===============================================================================}
procedure TPlasmaForm.MouseDown(Button:TMouseButton;Shift:TShiftState;X,Y:Integer);
begin
inherited MouseDown(Button,Shift,X,Y);
FMoveX:=x;
FMoveY:=y;
end;

procedure TPlasmaForm.MouseMove(Shift: TShiftState;X,Y:Integer);
begin
inherited MouseMove(Shift,x,y);
If (FMoveable) and (Shift=[ssLeft]) then
   begin
   Parent.Left:=Parent.Left + (x-FMoveX);
   Parent.Top :=Parent.Top + (y-FMoveY);
   SendMessage(Parent.Handle,WM_PAINT,0,0);
   end;
end;
{==============================================================================}
procedure TPlasmaForm.LoadMaskFromFile(Value:String);
var Reader:TFileStream;
    Data  :PRgnData;
begin
try
  Reader:=TFileStream.Create(Value,fmOpenRead);
	 GetMem(Data,Reader.Size);
		Reader.Read(Data^,Reader.Size);
		FRegion.Region:=ExtCreateRegion(nil,Reader.Size,Data^);
  FreeMem(Data,Reader.Size);
  Reader.free;
  RefreshForm;
except
   raise Exception.Create('Error Loading Mask');
  	end;
end;


//to use number-> resname = #number like #128;
procedure TPlasmaForm.LoadMaskFromResource(Instance:THandle;ResName:string);
var Reader:TResourceStream;
    Data  :PRgnData;
begin
try
  Reader:=TResourceStream.Create(Instance, ResName, RT_RCDATA);
	 GetMem(Data,Reader.Size);
		Reader.Read(Data^,Reader.Size);
		FRegion.Region:=ExtCreateRegion(nil,Reader.Size,Data^);
  FreeMem(Data,Reader.Size);
  Reader.free;
  RefreshForm;
except
   raise Exception.Create('Error Loading Mask form resource');
  	end;
end;
{==============================================================================}
procedure Register;
begin
RegisterComponents('Transparent', [TPlasmaForm]);
RegisterPropertyEditor(TypeInfo(TPlasmaRegion),nil,'',TPlasmaMaskProperty);
end;
{==============================================================================}
end.
