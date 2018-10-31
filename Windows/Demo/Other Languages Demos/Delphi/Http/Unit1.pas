unit Unit1;

interface

uses
  Windows, Messages, HPTypeDef, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls,HPSocketSDKUnit, ExtCtrls;

type
  TForm1 = class(TForm)
    lst: TListBox;
    Label1: TLabel;
    Edit1: TEdit;
    Label2: TLabel;
    Edit2: TEdit;
    btn1: TButton;
    btn2: TButton;
    Label3: TLabel;
    Label4: TLabel;
    Timer1: TTimer;
    procedure FormCreate(Sender: TObject);
    procedure FormDestroy(Sender: TObject);
    procedure btn1Click(Sender: TObject);
    procedure btn2Click(Sender: TObject);
    procedure Timer1Timer(Sender: TObject);

  private
    { Private declarations }
  public
    { Public declarations }
  end;

  Pserver = Record
    DListener : Integer;
    DServer : Integer;
  end;

var
  Form1: TForm1;
  PPserver: pserver;
  cs:TRTLCriticalSection;
implementation

{$R *.dfm}
procedure AddMsg(str: string);
begin
  EnterCriticalSection(CS);
  Form1.lst.Items.Add('==> '+str);
  LeaveCriticalSection(CS);

end;

function OnHeader(pSender: HP_Http; dwConnID: HP_CONNID; lpszName: PChar; lpszValue: PChar):En_HP_HttpParseResult; stdcall;
var
  nema:array of PChar;
  i: Pointer;
  bl:bool;
begin
    AddMsg('OnHeader -> ' + lpszName + ':' + lpszValue);
    Result := HPR_OK;
end;

function OnMessageBegin(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('MessageBegin -> ' + inttostr(Integer(dwConnID)));


    Result := HPR_OK;
end;

function OnPrepareListen(pSender: HP_Http; soListen: Pointer):En_HP_HandleResult; stdcall;
begin
    AddMsg('PrepareListen -> ' + IntToStr(Integer(soListen)));

    Result := HR_OK;
end;

function OnAccept(pSender: HP_Server; dwConnID: HP_CONNID; pClient: Pointer):En_HP_HandleResult; stdcall;
var
    sAddr:array[0..40] of Char;
    port: Word;
    len: Integer;
    str: PChar;
begin

    HP_Server_GetRemoteAddress(pSender,dwConnID,sAddr,len,port);
    str:= PChar(@sAddr);
    AddMsg('OnAcceptError -> ' + inttostr(dwConnId) + '  IP:' +str);
    Result := HR_OK;
end;

function OnRequestLine(pSender: HP_Http; dwConnID: HP_CONNID; lpszMethod: PChar; lpszUrl: PChar):En_HP_HttpParseResult; stdcall;
var
  Request: TNVPair;
  DFileText: string;
begin
  //特么的  简单的要死要死的
   AddMsg('RequestLine -> ' + lpszMethod+ ':' +  lpszUrl);
   if lpszMethod = 'GET' then
   begin
     if lpszUrl <> '' then
     begin
       OutputDebugString(lpszMethod);
       OutputDebugString(lpszUrl);
       SetLength(Request,2);
       Request[0].name:= PChar('Content-Type');
       Request[0].value:= PChar('text/html');
       Request[1].name:= PChar('Connection');
       Request[1].value:= PChar('Keep-Alive');

       DFileText:= ExtractFilePath(ParamStr(0)) + 'wwwroot' + string(lpszUrl);
       HP_HttpServer_SendLocalFile(pSender, dwConnID, PChar(DFileText), 200, PChar('OK'), Request, Length(Request));

       SetLength(Request,0);
     end;

   end
   else
   if lpszMethod = 'POST' then
   begin
      OutputDebugString(lpszMethod);
   end;


   Result := HPR_OK;
end;

function OnHandShake(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HandleResult; stdcall;
begin
   AddMsg('HandShake -> ');
   Result := HR_OK;
end;

function OnSend(pSender: HP_Http; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
begin
    AddMsg('Send -> ' + inttostr(iLength) + ' bytes');
    Result := HR_OK;
end;

function OnReceive(pSender: HP_Http; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HandleResult; stdcall;
begin
    AddMsg('OnReceive -> ');
    Result := HR_OK;
end;

function OnPullReceive (pSender: HP_Http; dwConnID: HP_CONNID; iLength: Integer):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('OnPullReceive -> ');
    Result := HPR_OK;
end;

function OnCloseConn(pSender: HP_Server; dwConnID: HP_CONNID; enOperation: En_HP_SocketOperation; iErrorCode: Integer):En_HP_HandleResult; stdcall;
begin
    AddMsg('CloseConn -> ' + inttostr(Integer(dwConnId)));
    Result := HR_OK;
end;

function OnShutdown(pSender: HP_Server):En_HP_HandleResult; stdcall;
begin
    AddMsg('Shutdown -> ');
    Result:= HR_OK;
end;

function OnHeadersComplete(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('HeadersComplete -> ');
    Result:= HPR_OK;
end;

function OnBody(pSender: HP_Http; dwConnID: HP_CONNID; const pData: Pointer; iLength: Integer):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('Body -> '+ string(pData));
    Result:= HPR_OK;
end;

function OnChunkHeader(pSender: HP_Http; dwConnID: HP_CONNID; iLength: Integer):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('ChunkHeader -> '+ IntToStr(dwConnID));
    Result:= HPR_OK;
end;

function OnChunkComplete(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('ChunkComplete -> '+ IntToStr(dwConnID));
    Result:= HPR_OK;
end;

function OnMessageComplete(pSender: HP_Http; dwConnID: HP_CONNID):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('MessageComplete -> '+ IntToStr(dwConnID));
    Result:= HPR_OK;
end;

function OnUpgrade(pSender: HP_Http; dwConnID: HP_CONNID; enUpgradeType: En_HP_HttpUpgradeType):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('Upgrade -> '+ IntToStr(dwConnID));
    Result:= HPR_OK;
end;

function OnParseError(pSender: HP_Http; dwConnID: HP_CONNID; iErrorCode: Integer; lpszErrorDesc: PChar):En_HP_HttpParseResult; stdcall;
begin
    AddMsg('OnParseError -> '+ IntToStr(dwConnID));
    Result:= HPR_OK;
end;

procedure TForm1.FormCreate(Sender: TObject);
begin
     // 创建监听器对象
    PPserver.DListener:= Create_HP_HttpServerListener();

    // 创建 Socket 对象
    PPserver.DServer := Create_HP_HttpServer(PPserver.DListener);

    // 设置 Socket 监听器回调函数
    HP_Set_FN_HttpServer_OnPrepareListen(PPserver.DListener, OnPrepareListen);
    HP_Set_FN_HttpServer_OnAccept(PPserver.DListener, OnAccept);
    HP_Set_FN_HttpServer_OnHandShake(PPserver.DListener,OnHandShake);
    HP_Set_FN_HttpServer_OnSend(PPserver.DListener, OnSend);
    HP_Set_FN_HttpServer_OnReceive(PPserver.DListener, OnReceive);
    HP_Set_FN_HttpServer_OnClose(PPserver.DListener, OnCloseConn);
    HP_Set_FN_HttpServer_OnShutdown(PPserver.DListener, OnShutdown);


    HP_Set_FN_HttpServer_OnMessageBegin(PPserver.DListener, OnMessageBegin);
    HP_Set_FN_HttpServer_OnRequestLine(PPserver.DListener, OnRequestLine);
    HP_Set_FN_HttpServer_OnHeader(PPserver.DListener, OnHeader);
    HP_Set_FN_HttpServer_OnHeadersComplete(PPserver.DListener, OnHeadersComplete);
    HP_Set_FN_HttpServer_OnBody(PPserver.DListener, OnBody);
    HP_Set_FN_HttpServer_OnChunkHeader(PPserver.DListener, OnChunkHeader);
    HP_Set_FN_HttpServer_OnChunkComplete(PPserver.DListener, OnChunkComplete);
    HP_Set_FN_HttpServer_OnMessageComplete(PPserver.DListener, OnMessageComplete);
    HP_Set_FN_HttpServer_OnUpgrade(PPserver.DListener, OnUpgrade);
    HP_Set_FN_HttpServer_OnParseError(PPserver.DListener, OnParseError);

    // 设置HP模式为并发发送模式
    HP_Server_SetSendPolicy(PPserver.DServer, SP_DIRECT);

    //设置工作线程
    HP_Server_SetWorkerThreadCount(PPserver.DServer, 30);

    //初始化临界区
    InitializeCriticalSection(CS);

    AddMsg('系统初始化完成');  
end;

procedure TForm1.FormDestroy(Sender: TObject);
begin
  DeleteCriticalSection(CS);   //删除临界区
end;

procedure TForm1.btn1Click(Sender: TObject);
var
  Ip:PChar;
  Port:Word;
  errorId: En_HP_SocketError;
  errorMsg: WideString;
begin
  {$D+}

    
    Ip := PChar(EDIT1.Text);

    Port := StrToInt(Edit2.TEXT);

    if HP_Server_Start(PPserver.DServer, ip, port) then
    begin
      AddMsg(Format('服务启动成功 -> (%s:%d)', [ip, port]));
    end
    else
    begin
        errorId := HP_Server_GetLastError(PPserver.DServer);
        errorMsg := HP_Server_GetLastErrorDesc(PPserver.DServer);
        AddMsg(Format('服务启动失败 -> %s(%d)', [errorMsg, Integer(errorId)]));
    end;

end;

procedure TForm1.btn2Click(Sender: TObject);
begin
  if HP_Server_Stop(PPserver.DServer) then
  begin
    AddMsg('服务停止成功 -> ');
  end
  else
  begin
    AddMsg('服务停止失败 -> ');
  end;

end;

procedure TForm1.Timer1Timer(Sender: TObject);
var
  Count:Integer;
begin
    Count:= HP_Server_GetConnectionCount(PPserver.DServer);
    Label4.Caption:= IntToStr(Count);
end;


end.
