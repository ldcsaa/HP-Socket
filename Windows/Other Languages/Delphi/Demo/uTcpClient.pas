unit uTcpClient;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants,
  System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms, Vcl.Dialogs, HPTypeDef,
  HPSocketSDKUnit, Vcl.StdCtrls, Vcl.ExtCtrls;

type
  TF_TcpClient = class(TForm)
    Panel1: TPanel;
    Label1: TLabel;
    Edt_Port: TEdit;
    Btn_Start: TButton;
    Label2: TLabel;
    Edt_Host: TEdit;
    Memo1: TMemo;
    Btn_SendString: TButton;
    procedure FormCreate(Sender: TObject);
    procedure Btn_StartClick(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Btn_SendStringClick(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
    FHP_TcpClientListener: HP_TcpClientListener;
    FHP_TcpClient: HP_TcpClient;
  end;

var
  F_TcpClient: TF_TcpClient;

implementation

{$R *.dfm}
function HP_Client_OnPrepareConnect(pSender: HP_Client; dwConnID: HP_CONNID; SOCKET: UINT_PTR): En_HP_HandleResult; stdcall;
begin

end;

function HP_Client_OnConnect(pSender: HP_Client; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
begin

end;

function HP_Client_OnHandShake(pSender: HP_Client; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
begin

end;

function HP_Client_OnSend(pSender: HP_Client; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;
begin

end;

function HP_Client_OnReceive(pSender: HP_Client; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;
begin

end;

function HP_Client_OnClose(pSender: HP_Client; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;
begin

end;

procedure TF_TcpClient.Btn_SendStringClick(Sender: TObject);

var
  sData: string;
begin
  sData := Memo1.Text;
  HP_Client_Send(FHP_TcpClient,@sData[1], Length(sData));
end;

procedure TF_TcpClient.Btn_StartClick(Sender: TObject);
begin
 if Self.Tag = 0 then
  begin
    if HP_Client_Start(FHP_TcpClient,PChar( Edt_Host.Text), StrToIntDef(Edt_Port.Text,8888),True) then
    begin
      Self.Tag := 1;
      Btn_Start.Caption := 'Í£Ö¹';
    end else
    begin
     ShowMessage( HP_Client_GetLastErrorDesc(FHP_TcpClient));
    end;
  end
  else
  begin
    if HP_Client_Stop(FHP_TcpClient) then
    begin
      Self.Tag := 0;
      Btn_Start.Caption := 'Æô¶¯';
    end;
  end;
end;

procedure TF_TcpClient.FormCreate(Sender: TObject);
var
  FHP_FN_Client_OnPrepareConnect: HP_FN_Client_OnPrepareConnect;
  FHP_FN_Client_OnConnect: HP_FN_Client_OnConnect;
  FHP_FN_Client_OnHandShake: HP_FN_Client_OnHandShake;
  FHP_FN_Client_OnSend: HP_FN_Client_OnSend;
  FHP_FN_Client_OnReceive: HP_FN_Client_OnReceive;
  FHP_FN_Client_OnClose: HP_FN_Client_OnClose;
begin
  FHP_FN_Client_OnPrepareConnect := HP_Client_OnPrepareConnect;
  FHP_FN_Client_OnConnect := HP_Client_OnConnect;
  FHP_FN_Client_OnHandShake := HP_Client_OnHandShake;
  FHP_FN_Client_OnSend := HP_Client_OnSend;
  FHP_FN_Client_OnReceive := HP_Client_OnReceive;
  FHP_FN_Client_OnClose := HP_Client_OnClose;

  FHP_TcpClientListener := Create_HP_UdpClientListener();
  FHP_TcpClient := Create_HP_TcpClient(FHP_TcpClientListener);

  HP_Set_FN_Client_OnPrepareConnect(FHP_TcpClientListener, FHP_FN_Client_OnPrepareConnect);
  HP_Set_FN_Client_OnConnect(FHP_TcpClientListener, FHP_FN_Client_OnConnect);
  HP_Set_FN_Client_OnHandShake(FHP_TcpClientListener, FHP_FN_Client_OnHandShake);
  HP_Set_FN_Client_OnSend(FHP_TcpClientListener, FHP_FN_Client_OnSend);
  HP_Set_FN_Client_OnReceive(FHP_TcpClientListener, FHP_FN_Client_OnReceive);
  HP_Set_FN_Client_OnClose(FHP_TcpClientListener, FHP_FN_Client_OnClose);
end;

procedure TF_TcpClient.FormDestroy(Sender: TObject);
begin
  Destroy_HP_TcpClient(FHP_TcpClient);
  Destroy_HP_TcpClientListener(FHP_TcpClientListener);
end;

end.

