unit uTcpServer;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants,
  System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms, Vcl.Dialogs,
  Vcl.StdCtrls, Vcl.ExtCtrls, HPSocketSDKUnit, HPTypeDef, System.TypInfo,
  Utils.Logger;

type
  TF_TcpServer = class(TForm)
    Panel1: TPanel;
    Label1: TLabel;
    Edt_Port: TEdit;
    Btn_Start: TButton;
    T_SHow: TTimer;
    Memo1: TMemo;
    Label2: TLabel;
    Label3: TLabel;
    Label4: TLabel;
    Label5: TLabel;
    Label6: TLabel;
    Label7: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Btn_StartClick(Sender: TObject);
    procedure T_SHowTimer(Sender: TObject);
  private
    { Private declarations }
    FHP_TcpServer: HP_Object;
    FHP_TcpServerListener: HP_Object;
  public
    { Public declarations }
  end;

var
  F_TcpServer: TF_TcpServer;

implementation

{$R *.dfm}
var
  FPrepareListenCount, FAcceptCount, FHandShakeCount, FSendCount, FReceiveCount, FCloseCount: Int64;

function HP_Server_OnPrepareListen(pSender: HP_Server; soListen: UINT_PTR): En_HP_HandleResult; stdcall;
begin
  ShowMessage(IntToStr(soListen));
  AtomicIncrement(FPrepareListenCount);
  Result := HR_OK;
end;

function HP_Server_OnAccept(pSender: HP_Server; dwConnID: HP_CONNID; pClient: UINT_PTR): En_HP_HandleResult; stdcall;
begin
  AtomicIncrement(FAcceptCount);
  Result := HR_OK;
end;

function HP_Server_OnHandShake(pSender: HP_Server; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
begin
  AppendLog('HandShake.dwConnID'+IntToStr(Int64(dwConnID)));
  AtomicIncrement(FHandShakeCount);
  Result := HR_OK;
end;

function HP_Server_OnSend(pSender: HP_Server; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;
begin
  AtomicIncrement(FSendCount);
  Result := HR_OK;
end;

function HP_Server_OnReceive(pSender: HP_Server; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;
var
 s: string;
begin
  AtomicIncrement(FReceiveCount);
  Result := HR_OK;
end;

function HP_Server_OnClose(pSender: HP_Server; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;
begin
  AppendLog('Close.dwConnID'+IntToStr(Int64(dwConnID)));
  AtomicIncrement(FCloseCount);
  Result := HR_OK;
end;

function HP_Server_OnShutdown(pSender: HP_Server): En_HP_HandleResult; stdcall;
begin
  Result := HR_OK;
end;

procedure TF_TcpServer.Btn_StartClick(Sender: TObject);
begin
  if Self.Tag = 0 then
  begin
    if HP_Server_Start(FHP_TcpServer, '127.0.0.1', StrToIntDef(Edt_Port.Text, 8888)) then
    begin
      Self.Tag := 1;
      Btn_Start.Caption := 'Í£Ö¹';
    end
    else
    begin
      ShowMessage(HP_Server_GetLastErrorDesc(FHP_TcpServer));
    end;
  end
  else
  begin
    if HP_Server_Stop(FHP_TcpServer) then
    begin
      Self.Tag := 0;
      Btn_Start.Caption := 'Æô¶¯';
    end;
  end;
end;

procedure TF_TcpServer.FormCreate(Sender: TObject);
var
  FHP_FN_Server_OnPrepareListen: HP_FN_Server_OnPrepareListen;
  FHP_FN_Server_OnAccept: HP_FN_Server_OnAccept;
  FHP_FN_Server_OnHandShake: HP_FN_Server_OnHandShake;
  FHP_FN_Server_OnSend: HP_FN_Server_OnSend;
  FHP_FN_Server_OnReceive: HP_FN_Server_OnReceive;
  FHP_FN_Server_OnClose: HP_FN_Server_OnClose;
  FHP_FN_Server_OnShutdown: HP_FN_Server_OnShutdown;
begin
  FHP_FN_Server_OnPrepareListen := HP_Server_OnPrepareListen;
  FHP_FN_Server_OnAccept := HP_Server_OnAccept;
  FHP_FN_Server_OnHandShake := HP_Server_OnHandShake;
  FHP_FN_Server_OnSend := HP_Server_OnSend;
  FHP_FN_Server_OnReceive := HP_Server_OnReceive;
  FHP_FN_Server_OnClose := HP_Server_OnClose;
  FHP_FN_Server_OnShutdown := HP_Server_OnShutdown;

  FHP_TcpServerListener := Create_HP_TcpServerListener();
  FHP_TcpServer := Create_HP_TcpServer(FHP_TcpServerListener);
  HP_Set_FN_Server_OnPrepareListen(FHP_TcpServerListener, FHP_FN_Server_OnPrepareListen);
  HP_Set_FN_Server_OnAccept(FHP_TcpServerListener, FHP_FN_Server_OnAccept);
  HP_Set_FN_Server_OnHandShake(FHP_TcpServerListener, FHP_FN_Server_OnHandShake);
  HP_Set_FN_Server_OnSend(FHP_TcpServerListener, FHP_FN_Server_OnSend);
  HP_Set_FN_Server_OnReceive(FHP_TcpServerListener, FHP_FN_Server_OnReceive);
  HP_Set_FN_Server_OnClose(FHP_TcpServerListener, FHP_FN_Server_OnClose);
  HP_Set_FN_Server_OnShutdown(FHP_TcpServerListener, FHP_FN_Server_OnShutdown);
end;

procedure TF_TcpServer.FormDestroy(Sender: TObject);
begin
  Destroy_HP_TcpServer(FHP_TcpServer);
  Destroy_HP_TcpServerListener(FHP_TcpServerListener);
end;

procedure TF_TcpServer.T_SHowTimer(Sender: TObject);
begin
  Label2.Caption := 'FPrepareListenCount=' + FPrepareListenCount.ToString;
  Label3.Caption := 'FAcceptCount=' + FAcceptCount.ToString;
  Label4.Caption := 'FHandShakeCount=' + FHandShakeCount.ToString;
  Label5.Caption := 'FSendCount=' + FSendCount.ToString;
  Label6.Caption := 'FReceiveCount=' + FReceiveCount.ToString;
  Label7.Caption := 'FCloseCount=' + FCloseCount.ToString;
end;

end.

