object Form1: TForm1
  Left = 205
  Top = 125
  BorderStyle = bsSingle
  Caption = 'Freia dongle management'
  ClientHeight = 230
  ClientWidth = 460
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 8
    Top = 48
    Width = 43
    Height = 13
    Caption = 'DevIDLo'
  end
  object Label2: TLabel
    Left = 8
    Top = 72
    Width = 41
    Height = 13
    Caption = 'DevIDHi'
  end
  object Label3: TLabel
    Left = 8
    Top = 96
    Width = 35
    Height = 13
    Caption = 'Version'
  end
  object Label4: TLabel
    Left = 8
    Top = 120
    Width = 45
    Height = 13
    Caption = 'Blocked?'
  end
  object Label5: TLabel
    Left = 8
    Top = 136
    Width = 48
    Height = 26
    Caption = 'Misusage counter'
    WordWrap = True
  end
  object Button1: TButton
    Left = 8
    Top = 8
    Width = 169
    Height = 25
    Caption = 'Load device info'
    TabOrder = 0
    OnClick = Button1Click
  end
  object devidlo: TEdit
    Left = 56
    Top = 40
    Width = 121
    Height = 21
    TabOrder = 1
  end
  object devidhi: TEdit
    Left = 56
    Top = 64
    Width = 121
    Height = 21
    TabOrder = 2
  end
  object version: TEdit
    Left = 56
    Top = 88
    Width = 121
    Height = 21
    TabOrder = 3
  end
  object blocked: TEdit
    Left = 56
    Top = 112
    Width = 121
    Height = 21
    TabOrder = 4
  end
  object misusage_counter: TEdit
    Left = 56
    Top = 136
    Width = 121
    Height = 21
    TabOrder = 5
  end
  object Button2: TButton
    Left = 8
    Top = 168
    Width = 169
    Height = 25
    Caption = 'Save device info'
    TabOrder = 6
    OnClick = Button2Click
  end
  object memo: TMemo
    Left = 184
    Top = 8
    Width = 273
    Height = 217
    Lines.Strings = (
      '')
    ReadOnly = True
    TabOrder = 7
  end
  object Button3: TButton
    Left = 8
    Top = 200
    Width = 169
    Height = 25
    Caption = 'Generate update'
    TabOrder = 8
    OnClick = Button3Click
  end
  object opendialog: TOpenDialog
    Left = 24
    Top = 8
  end
end
