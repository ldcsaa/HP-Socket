unit uTcpPackAgent;

interface

uses
  Winapi.Windows, Winapi.Messages, System.SysUtils, System.Variants,
  System.Classes, Vcl.Graphics, Vcl.Controls, Vcl.Forms, Vcl.Dialogs, HPTypeDef,
  HPSocketSDKUnit, Vcl.StdCtrls, Vcl.ExtCtrls, Utils.Logger;

type
  TF_TcpPackAgent = class(TForm)
    Panel1: TPanel;
    Label1: TLabel;
    Edt_Port: TEdit;
    Btn_Start: TButton;
    Label2: TLabel;
    Edt_Host: TEdit;
    Memo1: TMemo;
    Panel2: TPanel;
    Btn_ConnectOne: TButton;
    Btn_Connect100: TButton;
    Btn_SendString: TButton;
    procedure FormCreate(Sender: TObject);
    procedure Btn_StartClick(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure Btn_SendStringClick(Sender: TObject);
    procedure Btn_ConnectOneClick(Sender: TObject);
    procedure Btn_Connect100Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }

    FHP_TcpPackAgentListener: HP_TcpPackAgentListener;
    FHP_TcpPackAgent: HP_TcpPackAgent;
  end;

var
  F_TcpPackAgent: TF_TcpPackAgent;
  FHP_CONNIDArray: HP_CONNIDArray;

implementation

{$R *.dfm}

function HP_Agent_OnPrepareConnect(pSender: HP_Client; dwConnID: HP_CONNID; SOCKET: UINT_PTR): En_HP_HandleResult; stdcall;
begin
  Result := HR_OK;
end;

function HP_Agent_OnConnect(pSender: HP_Client; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
begin
  FHP_CONNIDArray := FHP_CONNIDArray + [dwConnID];
end;

function HP_Agent_OnHandShake(pSender: HP_Client; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
begin
  Result := HR_OK;
end;

function HP_Agent_OnSend(pSender: HP_Client; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;
begin
  Result := HR_OK;
end;

function HP_Agent_OnReceive(pSender: HP_Client; dwConnID: HP_CONNID; const pData: PBYTE; iLength: Integer): En_HP_HandleResult; stdcall;
var
  s: string;
begin
  SetLength(s, iLength);
  Move(pData^, s[1], iLength);
  TThread.Queue(nil,
    procedure
    begin
      F_TcpPackAgent.Memo1.Lines.Add(s);
    end);
  Result := HR_OK;
end;

function HP_Agent_OnClose(pSender: HP_Client; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;
var
  I: Integer;
begin

  for I := 0 to Length(FHP_CONNIDArray) - 1 do
  begin
    if FHP_CONNIDArray[I] = dwConnID then
    begin
      Delete(FHP_CONNIDArray, I, 1);
      Break;
    end;
  end;
end;

procedure TF_TcpPackAgent.Btn_Connect100Click(Sender: TObject);
var
  I: Integer;
begin
  for I := 0 to 99 do
  begin
    Btn_ConnectOneClick(Btn_ConnectOne);
  end;
end;

procedure TF_TcpPackAgent.Btn_ConnectOneClick(Sender: TObject);
var
  APConnId: PInteger;
begin
  if HP_Agent_ConnectWithExtra(FHP_TcpPackAgent, PChar(Edt_Host.Text), StrToIntDef(Edt_Port.Text, 8888), APConnId, PChar('abcdef')) then
  begin
    AppendLog('连接服务器成功,地址:%s,端口: %d,ConnId=', [Edt_Host.Text, StrToIntDef(Edt_Port.Text, 8888)]);
    Memo1.Lines.Add(Format('连接服务器成功,地址:%s,端口: %d,Connid=%d', [Edt_Host.Text, StrToIntDef(Edt_Port.Text, 8888), Int64(APConnId)]));
  end
  else
  begin
    AppendLog('连接服务器失败,原因为:%s', [HP_Agent_GetLastErrorDesc(FHP_TcpPackAgent)]);
    Memo1.Lines.Add(Format('连接服务器失败,原因为:%s', [HP_Agent_GetLastErrorDesc(FHP_TcpPackAgent)]));
    Exit;
  end;
end;

procedure TF_TcpPackAgent.Btn_SendStringClick(Sender: TObject);
var
  sData: Ansistring;
  len: Integer;
begin
  sData := Memo1.Text;
  if HP_Agent_Send(FHP_TcpPackAgent, FHP_CONNIDArray[0], @sData[1], Length(sData)) then
  begin
    ShowMessage('发送成功!')
  end
  else
  begin
    AppendLog('发送失败,原因为:%s', [HP_Agent_GetLastErrorDesc(FHP_TcpPackAgent)]);
    ShowMessageFmt('发送失败,原因为:%s,错误代码为:%d', [HP_Agent_GetLastErrorDesc(FHP_TcpPackAgent), ord(HP_Agent_GetLastError(FHP_TcpPackAgent))]);
  end;
end;

procedure TF_TcpPackAgent.Btn_StartClick(Sender: TObject);
begin
  if Self.Tag = 0 then
  begin
    if HP_Agent_Start(FHP_TcpPackAgent, PChar(Edt_Host.Text), True) then
    begin
      AppendLog('启动Agent组件成功,地址:%s', [Edt_Host.Text]);
      Self.Tag := 1;
      Btn_Start.Caption := '停止';
    end
    else
    begin
      AppendLog('启动Agent组件失败,原因为:%s', [HP_Agent_GetLastErrorDesc(FHP_TcpPackAgent)]);
      Exit;
    end;
  end
  else
  begin
    if HP_Agent_Stop(FHP_TcpPackAgent) then
    begin
      AppendLog('停止Agent组件成功,地址:%s', [Edt_Host.Text]);
      Self.Tag := 0;
      Btn_Start.Caption := '启动';
    end
    else
    begin
      AppendLog('停止Agent组件失败,原因为:%s', [HP_Agent_GetLastErrorDesc(FHP_TcpPackAgent)]);
    end;
  end;
end;

procedure TF_TcpPackAgent.FormCreate(Sender: TObject);
var
  FHP_FN_Agent_OnPrepareConnect: HP_FN_Agent_OnPrepareConnect;
  FHP_FN_Agent_OnConnect: HP_FN_Agent_OnConnect;
  FHP_FN_Agent_OnHandShake: HP_FN_Agent_OnHandShake;
  FHP_FN_Agent_OnSend: HP_FN_Agent_OnSend;
  FHP_FN_Agent_OnReceive: HP_FN_Agent_OnReceive;
  FHP_FN_Agent_OnClose: HP_FN_Agent_OnClose;
begin
  FHP_CONNIDArray := [];
  FHP_FN_Agent_OnPrepareConnect := HP_Agent_OnPrepareConnect;
  FHP_FN_Agent_OnConnect := HP_Agent_OnConnect;
  FHP_FN_Agent_OnHandShake := HP_Agent_OnHandShake;
  FHP_FN_Agent_OnSend := HP_Agent_OnSend;
  FHP_FN_Agent_OnReceive := HP_Agent_OnReceive;
  FHP_FN_Agent_OnClose := HP_Agent_OnClose;

  FHP_TcpPackAgentListener := Create_HP_TcpPackAgentListener();
  FHP_TcpPackAgent := Create_HP_TcpPackAgent(FHP_TcpPackAgentListener);

  HP_Set_FN_Agent_OnPrepareConnect(FHP_TcpPackAgentListener, FHP_FN_Agent_OnPrepareConnect);
  HP_Set_FN_Agent_OnConnect(FHP_TcpPackAgentListener, FHP_FN_Agent_OnConnect);
  HP_Set_FN_Agent_OnHandShake(FHP_TcpPackAgentListener, FHP_FN_Agent_OnHandShake);
  HP_Set_FN_Agent_OnSend(FHP_TcpPackAgentListener, FHP_FN_Agent_OnSend);
  HP_Set_FN_Agent_OnReceive(FHP_TcpPackAgentListener, FHP_FN_Agent_OnReceive);
  HP_Set_FN_Agent_OnClose(FHP_TcpPackAgentListener, FHP_FN_Agent_OnClose);
end;

procedure TF_TcpPackAgent.FormDestroy(Sender: TObject);
begin
  Destroy_HP_TcpPackAgent(FHP_TcpPackAgent);
  Destroy_HP_TcpPackAgentListener(FHP_TcpPackAgentListener);
end;

end.

