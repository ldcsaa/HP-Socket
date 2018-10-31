object Form1: TForm1
  Left = 457
  Top = 131
  Width = 522
  Height = 393
  Caption = 'Delphi HP Client'
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 13
  object Label1: TLabel
    Left = 24
    Top = 18
    Width = 52
    Height = 13
    Caption = #36830#25509' I P'#65306
  end
  object Label2: TLabel
    Left = 16
    Top = 49
    Width = 60
    Height = 13
    Caption = #36830#25509#31471#21475#65306
  end
  object Label3: TLabel
    Left = 208
    Top = 15
    Width = 60
    Height = 13
    Caption = #36830#25509#29366#24577#65306
  end
  object Label4: TLabel
    Left = 269
    Top = 15
    Width = 36
    Height = 13
    Caption = #26410#36830#25509
  end
  object Edit1: TEdit
    Left = 76
    Top = 15
    Width = 121
    Height = 21
    TabOrder = 0
    Text = '127.0.0.1'
  end
  object Edit2: TEdit
    Left = 76
    Top = 46
    Width = 121
    Height = 21
    TabOrder = 1
    Text = '8808'
  end
  object Button1: TButton
    Left = 360
    Top = 8
    Width = 75
    Height = 25
    Caption = #21551#21160#36830#25509
    TabOrder = 2
    OnClick = Button1Click
  end
  object Button2: TButton
    Left = 360
    Top = 40
    Width = 75
    Height = 25
    Caption = #20851#38381#36830#25509
    TabOrder = 3
    OnClick = Button2Click
  end
  object ListBox1: TListBox
    Left = 8
    Top = 107
    Width = 489
    Height = 238
    Enabled = False
    IntegralHeight = True
    ItemHeight = 13
    ParentShowHint = False
    ShowHint = False
    TabOrder = 4
  end
  object Edit3: TEdit
    Left = 8
    Top = 80
    Width = 337
    Height = 21
    TabOrder = 5
    Text = #36825#26159#19968#27573#27979#35797#25968#25454'~~~'
  end
  object Button3: TButton
    Left = 360
    Top = 76
    Width = 75
    Height = 25
    Caption = #21457#36865#25968#25454
    TabOrder = 6
    OnClick = Button3Click
  end
end
