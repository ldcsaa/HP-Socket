object F_TcpClient: TF_TcpClient
  Left = 0
  Top = 0
  Caption = 'F_TcpClient'
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
      TabOrder = 3
      Text = '8888'
    end
    object Btn_Start: TButton
      Left = 304
      Top = 9
      Width = 75
      Height = 25
      Caption = #21551#21160
      TabOrder = 1
      OnClick = Btn_StartClick
    end
    object Edt_Host: TEdit
      Left = 62
      Top = 11
      Width = 123
      Height = 21
      TabOrder = 2
      Text = '127.0.0.1'
    end
    object Btn_SendString: TButton
      Left = 416
      Top = 8
      Width = 89
      Height = 25
      Caption = #21457#36865#23383#31526#20018
      TabOrder = 0
      OnClick = Btn_SendStringClick
    end
  end
  object Memo1: TMemo
    Left = 62
    Top = 59
    Width = 161
    Height = 169
    Lines.Strings = (
      #30001#20110'Client'#21482#36866#21512#21333#24182#21457','#25152
      #21040#20889#21040#19968#21322#19981#20889#20102'.')
    TabOrder = 1
  end
end
