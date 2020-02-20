object F_TcpPackAgent: TF_TcpPackAgent
  Left = 0
  Top = 0
  Caption = 'F_TcpPackAgent'
  ClientHeight = 276
  ClientWidth = 576
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
  object Panel1: TPanel
    Left = 0
    Top = 0
    Width = 576
    Height = 41
    Align = alTop
    TabOrder = 0
    object Label1: TLabel
      Left = 200
      Top = 14
      Width = 24
      Height = 13
      Caption = #31471#21475
    end
    object Label2: TLabel
      Left = 8
      Top = 14
      Width = 48
      Height = 13
      Caption = #20027#26426#22320#22336
    end
    object Edt_Port: TEdit
      Left = 230
      Top = 11
      Width = 51
      Height = 21
      NumbersOnly = True
      TabOrder = 2
      Text = '8888'
    end
    object Btn_Start: TButton
      Left = 304
      Top = 9
      Width = 75
      Height = 25
      Caption = #21551#21160
      TabOrder = 0
      OnClick = Btn_StartClick
    end
    object Edt_Host: TEdit
      Left = 62
      Top = 11
      Width = 123
      Height = 21
      TabOrder = 1
      Text = '127.0.0.1'
    end
  end
  object Memo1: TMemo
    Left = 24
    Top = 58
    Width = 185
    Height = 210
    Lines.Strings = (
      '0123456789')
    TabOrder = 1
  end
  object Panel2: TPanel
    Left = 328
    Top = 58
    Width = 201
    Height = 193
    TabOrder = 2
    object Btn_ConnectOne: TButton
      Left = 8
      Top = 8
      Width = 105
      Height = 25
      Caption = #21019#24314#19968#20010#36830#25509
      TabOrder = 0
      OnClick = Btn_ConnectOneClick
    end
    object Btn_Connect100: TButton
      Left = 8
      Top = 48
      Width = 105
      Height = 25
      Caption = #21019#24314'100'#20010#36830#25509
      TabOrder = 1
      OnClick = Btn_Connect100Click
    end
    object Btn_SendString: TButton
      Left = 8
      Top = 85
      Width = 105
      Height = 25
      Caption = #21457#36865#23383#31526#20018
      TabOrder = 2
      OnClick = Btn_SendStringClick
    end
  end
end
