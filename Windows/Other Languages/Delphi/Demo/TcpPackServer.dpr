program TcpPackServer;

uses
  Vcl.Forms,
  uTcpPackServer in 'uTcpPackServer.pas' {F_TcpPackServer},
  HPSocketSDK_SSLUnit in '..\Lib\HPSocketSDK_SSLUnit.pas',
  HPSocketSDKUnit in '..\Lib\HPSocketSDKUnit.pas',
  HPTypeDef in '..\Lib\HPTypeDef.pas';

{$R *.res}

begin
  Application.Initialize;
  Application.MainFormOnTaskbar := True;
  Application.CreateForm(TF_TcpPackServer, F_TcpPackServer);
  Application.Run;
end.
