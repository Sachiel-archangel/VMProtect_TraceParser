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

1. 「Debugger setup」ダイアログで、エントリーポイントでデバッガが停止するように設定する。
![](https://github.com/Sachiel-archangel/VMProtect_TraceParser/tree/main/image/01_IDA_Debugger_Setup.png)
2. デバッグを実行し、エントリーポイントで停止する。
![](https://github.com/Sachiel-archangel/VMProtect_TraceParser/tree/main/image/02_IDA_Suspend_at_entrypoint.png)
3. Debugger->Tracing->Tracing options でダイアログを開き、トレースログファイルの出力と「Log return instructions」をチェックする。
![](https://github.com/Sachiel-archangel/VMProtect_TraceParser/tree/main/image/03_TracingOptions.png)
4. Debugger->Tracing->Function tracing でトレースを開始する。
![](https://github.com/Sachiel-archangel/VMProtect_TraceParser/tree/main/image/04_IDA_tracing.png)
5. デバッグを再開する。 *トレースを出力しながらのデバッガの実行は時間がかかります。
![](https://github.com/Sachiel-archangel/VMProtect_TraceParser/tree/main/image/05_IDA_Resume.png)
6. 任意の箇所で停止し、トレースログファイルを取得する。


## How to Use

