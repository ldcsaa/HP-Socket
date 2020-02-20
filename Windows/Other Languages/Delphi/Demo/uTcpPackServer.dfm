object F_TcpPackServer: TF_TcpPackServer
  Left = 0
  Top = 0
  Caption = 'F_TcpPackServer'
  ClientHeight = 296
  ClientWidth = 543
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'Tahoma'
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  OnDestroy = FormDestroy
  PixelsPerInch = 96
  TextHeight = 13
  object Label2: TLabel
    Left = 40
    Top = 97
    Width = 31
    Height = 13
    Caption = 'Label2'
  end
  object Label3: TLabel
    Left = 40
    Top = 132
    Width = 31
    Height = 13
    Caption = 'Label3'
  end
  object Label4: TLabel
    Left = 40
    Top = 166
    Width = 31
    Height = 13
    Caption = 'Label4'
  end
  object Label5: TLabel
    Left = 40
    Top = 200
    Width = 31
    Height = 13
    Caption = 'Label5'
  end
  object Label6: TLabel
    Left = 40
    Top = 235
    Width = 31
    Height = 13
    Caption = 'Label6'
  end
  object Label7: TLabel
    Left = 40
    Top = 269
    Width = 31
    Height = 13
    Caption = 'Label7'
  end
  object Label8: TLabel
    Left = 40
    Top = 63
    Width = 31
    Height = 13
    Caption = 'Label8'
  end
  object Label9: TLabel
    Left = 176
    Top = 63
    Width = 31
    Height = 13
    Caption = 'Label7'
  end
  object Label10: TLabel
    Left = 176
    Top = 97
    Width = 31
    Height = 13
    Caption = 'Label7'
  end
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 543
    Height = 41
    Align = alTop
    TabOrder = 0
    object Label1: TLabel
      Left = 72
      Top = 14
      Width = 24
      Height = 13
      Caption = #31471#21475
    end
    object Edt_Port: TEdit
      Left = 118
      Top = 11
      Width = 121
      Height = 21
      NumbersOnly = True
      TabOrder = 1
      Text = '8888'
    end
    object Btn_Start: TButton
      Left = 408
      Top = 9
      Width = 75
      Height = 25
      Caption = #21551#21160
      TabOrder = 0
      OnClick = Btn_StartClick
    end
  end
  object Memo1: TMemo
    Left = 370
    Top = 60
    Width = 165
    Height = 228
    Lines.Strings = (
      'Memo1')
    TabOrder = 1
  end
  object T_SHow: TTimer
    OnTimer = T_SHowTimer
    Left = 496
    Top = 48
  end
end
