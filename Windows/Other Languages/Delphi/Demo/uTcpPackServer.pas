unit uTcpPackServer;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants,
  System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms, Vcl.Dialogs,
  Vcl.StdCtrls, Vcl.ExtCtrls, HPSocketSDKUnit, HPTypeDef, System.TypInfo,
  Utils.Logger, System.Diagnostics;

type
  TF_TcpPackServer = class(TForm)
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
    Label8: TLabel;
    Label9: TLabel;
    Label10: TLabel;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Btn_StartClick(Sender: TObject);
    procedure T_SHowTimer(Sender: TObject);
  private
    { Private declarations }
    FHP_TcpPackServer: HP_TcpPackServer;
    FHP_TcpPackServerListener: HP_TcpPackServerListener;
  public
    { Public declarations }
  end;

var
  F_TcpPackServer: TF_TcpPackServer;

implementation

{$R *.dfm}
var
  FPrepareListenCount, FAcceptCount, FHandShakeCount, FSendCount, FReceiveCount, FCloseCount: Int64;
  FSentBytes, FRcvdBytes, FLastSent, FLastRcvd: Int64;
  FRunWatch, FSendWatch, FRecvWatch: TStopwatch;

function BytesToStr(const Bytes: Extended): string;
const
  KB = Int64(1024);
  MB = KB * 1024;
  GB = MB * 1024;
  TB = GB * 1024;
  PB = TB * 1024;
begin
  if (Bytes = 0) then
    Result := ''
  else if (Bytes < KB) then
    Result := FormatFloat('0.##B', Bytes)
  else if (Bytes < MB) then
    Result := FormatFloat('0.##KB', Bytes / KB)
  else if (Bytes < GB) then
    Result := FormatFloat('0.##MB', Bytes / MB)
  else if (Bytes < TB) then
    Result := FormatFloat('0.##GB', Bytes / GB)
  else if (Bytes < PB) then
    Result := FormatFloat('0.##TB', Bytes / TB)
  else
    Result := FormatFloat('0.##PB', Bytes / PB)
end;

function WatchToStr(const AWatch: TStopwatch): string;
begin
  Result := '';
  if (AWatch.Elapsed.Days > 0) then
    Result := Result + AWatch.Elapsed.Days.ToString + 'd';
  if (AWatch.Elapsed.Hours > 0) then
    Result := Result + AWatch.Elapsed.Hours.ToString + 'h';
  if (AWatch.Elapsed.Minutes > 0) then
    Result := Result + AWatch.Elapsed.Minutes.ToString + 'm';
  if (AWatch.Elapsed.Seconds > 0) then
    Result := Result + AWatch.Elapsed.Seconds.ToString + 's';
end;

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
  AppendLog('HandShake.dwConnID' + IntToStr(Int64(dwConnID)));
  AtomicIncrement(FHandShakeCount);
  Result := HR_OK;
end;

function HP_Server_OnSend(pSender: HP_Server; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;
begin
  AtomicIncrement(FSendCount);
  AtomicIncrement(FSentBytes, iLength);
  Result := HR_OK;
end;

function HP_Server_OnReceive(pSender: HP_Server; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;
var
  s: String;

begin


  TThread.Queue(nil,
  procedure
  begin
     SetLength(s, iLength);
  Move(pData^, s[1], iLength);
   F_TcpPackServer.Memo1.Lines.Add(s);

  end);
  AtomicIncrement(FReceiveCount);
  AtomicIncrement(FRcvdBytes, iLength);
  HP_Server_Send(pSender,dwConnID,pData,iLength);
  Result := HR_OK;
end;

function HP_Server_OnClose(pSender: HP_Server; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;
begin
  AppendLog('Close.dwConnID' + IntToStr(Int64(dwConnID)));
  AtomicIncrement(FCloseCount);
  Result := HR_OK;
end;

function HP_Server_OnShutdown(pSender: HP_Server): En_HP_HandleResult; stdcall;
begin
  Result := HR_OK;
end;

procedure TF_TcpPackServer.Btn_StartClick(Sender: TObject);
begin

  if Self.Tag = 0 then
  begin
    if HP_Server_Start(FHP_TcpPackServer, '127.0.0.1', StrToIntDef(Edt_Port.Text, 8888)) then
    begin
      Self.Tag := 1;
      Btn_Start.Caption := '停止';
    end
    else
    begin
      ShowMessage(HP_Server_GetLastErrorDesc(FHP_TcpPackServer));
    end;
  end
  else
  begin
    if HP_Server_Stop(FHP_TcpPackServer) then
    begin
      Self.Tag := 0;
      Btn_Start.Caption := '启动';
    end;
  end;
end;

procedure TF_TcpPackServer.FormCreate(Sender: TObject);
var
  FHP_FN_Server_OnPrepareListen: HP_FN_Server_OnPrepareListen;
  FHP_FN_Server_OnAccept: HP_FN_Server_OnAccept;
  FHP_FN_Server_OnHandShake: HP_FN_Server_OnHandShake;
  FHP_FN_Server_OnSend: HP_FN_Server_OnSend;
  FHP_FN_Server_OnReceive: HP_FN_Server_OnReceive;
  FHP_FN_Server_OnClose: HP_FN_Server_OnClose;
  FHP_FN_Server_OnShutdown: HP_FN_Server_OnShutdown;
begin
  FRunWatch := TStopwatch.StartNew;
  FHP_FN_Server_OnPrepareListen := HP_Server_OnPrepareListen;
  FHP_FN_Server_OnAccept := HP_Server_OnAccept;
  FHP_FN_Server_OnHandShake := HP_Server_OnHandShake;
  FHP_FN_Server_OnSend := HP_Server_OnSend;
  FHP_FN_Server_OnReceive := HP_Server_OnReceive;
  FHP_FN_Server_OnClose := HP_Server_OnClose;
  FHP_FN_Server_OnShutdown := HP_Server_OnShutdown;

  FHP_TcpPackServerListener := Create_HP_TcpPackServerListener();
  FHP_TcpPackServer := Create_HP_TcpPackServer(FHP_TcpPackServerListener);

  //HP_TcpPackServer_SetMaxPackSize(FHP_TcpPackServer,32*1024*1024);
  HP_Set_FN_Server_OnPrepareListen(FHP_TcpPackServerListener, FHP_FN_Server_OnPrepareListen);
  HP_Set_FN_Server_OnAccept(FHP_TcpPackServerListener, FHP_FN_Server_OnAccept);
  HP_Set_FN_Server_OnHandShake(FHP_TcpPackServerListener, FHP_FN_Server_OnHandShake);
  HP_Set_FN_Server_OnSend(FHP_TcpPackServerListener, FHP_FN_Server_OnSend);
  HP_Set_FN_Server_OnReceive(FHP_TcpPackServerListener, FHP_FN_Server_OnReceive);
  HP_Set_FN_Server_OnClose(FHP_TcpPackServerListener, FHP_FN_Server_OnClose);
  HP_Set_FN_Server_OnShutdown(FHP_TcpPackServerListener, FHP_FN_Server_OnShutdown);
end;

procedure TF_TcpPackServer.FormDestroy(Sender: TObject);
begin
  Destroy_HP_TcpPackServer(FHP_TcpPackServer);
  Destroy_HP_TcpPackServerListener(FHP_TcpPackServerListener);
end;

procedure TF_TcpPackServer.T_SHowTimer(Sender: TObject);
begin
  Label2.Caption := 'FPrepareListenCount=' + FPrepareListenCount.ToString;
  Label3.Caption := 'FAcceptCount=' + FAcceptCount.ToString;
  Label4.Caption := 'FHandShakeCount=' + FHandShakeCount.ToString;
  Label5.Caption := 'FSendCount=' + FSendCount.ToString;
  Label6.Caption := 'FReceiveCount=' + FReceiveCount.ToString;
  Label7.Caption := 'FCloseCount=' + FCloseCount.ToString;
  Label8.Caption := '运行时间:' + WatchToStr(FRunWatch);
  Label9.Caption := 'FRcvdBytes=' + FRcvdBytes.ToString;
  Label10.Caption := 'FSendByte=' + FSentBytes.ToString;
end;

end.

