unit Unit1;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, HPTypeDef, HPSocketSDKUnit;

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
  DPListener: Integer;
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

function OnPrepareConnect(pSender: HP_Client; dwConnID: HP_CONNID; socket: Pointer): En_HP_HandleResult; stdcall;
begin
    AddMsg('׼������ -> ' + inttostr(pClient));
    Result := HR_OK;
end;

function OnConnect(pSender: HP_Client; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
begin
    AddMsg('���ӳɹ� -> ' + inttostr(pClient));
    Result := HR_OK;
end;

function OnHandShake(pSender: HP_Client; dwConnID: HP_CONNID): En_HP_HandleResult; stdcall;
begin
    AddMsg('���ֳɹ� -> ' + inttostr(pClient));
    Result := HR_OK;
end;

function OnSend(pSender: HP_Client; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
begin
    AddMsg('�������� -> ' + inttostr(pClient));
    Result := HR_OK;
end;

function OnReceive(pSender: HP_Client; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer): En_HP_HandleResult; stdcall;
begin
    AddMsg('�յ����� -> ' + inttostr(iLength) + ' byte');
    Result :=HR_OK;
end;

function OnPullReceive(pSender: HP_Client; dwConnID: HP_CONNID; iLength: Integer): En_HP_HandleResult; stdcall;
begin
    AddMsg('�յ����� -> ' + inttostr(iLength) + ' byte');
    Result :=HR_OK;
end;

function OnCloseCon(pSender: HP_Client; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer): En_HP_HandleResult; stdcall;
begin
    AddMsg('���ӹر� -> ' + inttostr(pClient) );
    Result := HR_OK;
end;



procedure TForm1.FormCreate(Sender: TObject);
begin
  //����������
  DPListener:= Create_HP_TcpClientListener();

  //��������
  PClient:= Create_HP_TcpClient(DPListener);

  //���ûص�����
  HP_Set_FN_Client_OnPrepareConnect(DPListener, OnPrepareConnect);
  HP_Set_FN_Client_OnConnect(DPListener, OnConnect);
  HP_Set_FN_Client_OnHandShake(DPListener, OnHandShake);
  HP_Set_FN_Client_OnSend(DPListener, OnSend);
  HP_Set_FN_Client_OnReceive(DPListener, OnReceive);
  HP_Set_FN_Client_OnPullReceive(DPListener, OnPullReceive);
  HP_Set_FN_Client_OnClose(DPListener, OnCloseCon);

  AddMsg('�ͻ������Ӵ����ɹ� -> ');
end;

procedure TForm1.Button1Click(Sender: TObject);
var
  Ip:PChar;
  Port:Word;
begin
    ip := PChar(Edit1.Text);
    port := StrToInt(Edit2.Text);

    if HP_Client_Start(PClient, ip, port, False) then
    begin
      AddMsg(Format('���������ɹ� -> (%s:%d)', [ip, port]));
      Label4.Caption:= '������'
    end
    else
    begin
      AddMsg('��������ʧ�� -> %s(%d)');
    end;
end;

procedure TForm1.Button2Click(Sender: TObject);
begin
    if HP_Client_Stop(PClient) then
    begin
      AddMsg('���ӹرճɹ�');
      Label4.Caption:= 'δ����'
    end
    else
      AddMsg('���ӹر�ʧ��');

end;

procedure TForm1.Button3Click(Sender: TObject);
var
  s: string;
  len: Integer;
begin
  s:= Edit3.Text;
  len:= Length(s);
    HP_Client_Send(PClient, Pointer(s), len) ;
end;

end.
 