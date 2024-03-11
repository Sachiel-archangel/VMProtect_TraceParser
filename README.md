# VMProtect_TraceParser
A tool that parses the function trace of IDA that executes VMProtect.

## 概　要  
VMProtectで作成されたマルウェアに対し、IDA で出力したFunction Traceを分析して.textセクションから.vmp0を経由してコールしているWindowsAPIを出力する。  
これにより、復号化された.textセクションのコードを静的解析するために必要なAPIコールを把握することができる。  
  
## Outline
For malware created with VMProtect, analyze the Function Trace output by IDA and output the Windows API being called via .vmp0 from the .text section.  
This allows you to understand the API calls required to statically analyze the code in the decrypted .text section.  

## 制限事項 Restriction
* IDA Ver7および8シリーズの Function Trace でのみ動作確認済み。  
* VMProtectのバージョンにより、正しく解析できない可能性あり。  
* 開発時にマルウェアで使用されていた VMProtect のバージョンは不明。  
* Confirmed to work only with IDA Ver7 and 8 series Function Trace.
* Depending on the version of VMProtect, it may not be possible to analyze correctly.
* The version of VMProtect used in the malware when this tool was developed is unknown.

## 使い方
IDAを用いてFunction Traceを取得します。  
操作例は以下の通り。  

## How to Use

