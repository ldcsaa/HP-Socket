unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, HPSocketSDKUnit;

type
  TForm1 = class(TForm)
    Label1: TLabel;
    Label2: TLabel;
    Edit1: TEdit;
    Label3: TLabel;
    Label4: TLabel;
    Edit2: TEdit;
    Button1: TButton;
    Button2: TButton;
    ListBox1: TListBox;
    Edit3: TEdit;
    Button3: TButton;
    procedure FormCreate(Sender: TObject);
    procedure Button1Click(Sender: TObject);
    procedure Button2Click(Sender: TObject);
    procedure Button3Click(Sender: TObject);
  private
    { Private declarations }
  public
    { Public declarations }
  end;

var
  Form1: TForm1;
  PClient: Integer;
  PListener: Integer;
implementation

{$R *.dfm}
procedure AddMsg(str: string);
begin
  Form1.ListBox1.Items.add('==> '+str);
end;

function DoStrToWideChar(s: string): PWideChar;
begin 
Result := PWideChar(WideString(s)); 
end;

function OnPrepareConnect(pClient: HP_Client; SOCKET: Pointer): En_HP_HandleResult; stdcall;
begin
    AddMsg('准备连接 -> ' + inttostr(pClient));
    Result := HP_HR_OK;
end;

function OnConnect(pClient: HP_Client): En_HP_HandleResult; stdcall;
begin
    AddMsg('连接成功 -> ' + inttostr(pClient));
    Result := HP_HR_OK;
end;

function OnSend(pClient: HP_Client; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
begin
    AddMsg('发送数据 -> ' + inttostr(pClient));
    Result := HP_HR_OK;
end;

function OnReceive(pClient: HP_Client; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
begin
    AddMsg('收到数据 -> ' + inttostr(iLength) + ' byte');
    Result := HP_HR_OK;
end;

function OnCloseCon(pClient: HP_Client; enOperation: En_HP_SocketOperation; iLength: Integer): En_HP_HandleResult; stdcall;
begin
    AddMsg('连接关闭 -> ' + inttostr(pClient) );
    Result := HP_HR_OK;
end;



procedure TForm1.FormCreate(Sender: TObject);
begin
  //创建监听器
  PListener:= Create_HP_TcpClientListener();

  //创建服务
  PClient:= Create_HP_TcpClient(PListener);

  //设置回调函数
  HP_Set_FN_Client_OnPrepareConnect(PListener, OnPrepareConnect);
  HP_Set_FN_Client_OnConnect(PListener, OnConnect);
  HP_Set_FN_Client_OnSend(PListener, OnSend);
  HP_Set_FN_Client_OnReceive(PListener, OnReceive);
  HP_Set_FN_Client_OnClose(PListener,OnCloseCon);

  AddMsg('客户端连接创建成功 -> ');
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  Ip:PWideChar;
  Port:Word;
begin
    ip := DoStrToWideChar(Edit1.Text);
    port := StrToInt(Edit2.Text);

    if HP_Client_Start(PClient, ip, port, False) then
    begin
      AddMsg(Format('连接启动成功 -> (%s:%d)', [ip, port]));
      Label4.Caption:= '已连接'
    end
    else
    begin
      AddMsg('连接启动失败 -> %s(%d)');
    end;
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
    if HP_Client_Stop(PClient) then
    begin
      AddMsg('连接关闭成功');
      Label4.Caption:= '未连接'
    end
    else
      AddMsg('连接关闭失败');

end;

procedure TForm1.Button3Click(Sender: TObject);
var
  s: string;
  len: Integer;
begin
  s:= Edit3.Text;
  len:= Length(s);
    HP_Client_Send(PClient, @s, len) ;
end;

end.
 