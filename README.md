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
<br>
1. 「Debugger setup」ダイアログで、エントリーポイントでデバッガが停止するように設定する。  
![image1](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/01_IDA_Debugger_Setup.png)

2. デバッグを実行し、エントリーポイントで停止する。  
![image2](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/02_IDA_Suspend_at_entrypoint.png)

3. Debugger->Tracing->Tracing options でダイアログを開き、トレースログファイルの出力と「Log return instructions」をチェックする。  
![image3](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/03_TracingOptions.png)

4. Debugger->Tracing->Function tracing でトレースを開始する。  
![image4](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/04_IDA_tracing.png)

5. デバッグを再開する。 *トレースを出力しながらのデバッガの実行は時間がかかります。  
![image5](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/05_IDA_Resume.png)

6. 任意の箇所で停止し、トレースログファイルを取得する。  
<br>
<br>

トレースログの取得ができたら、コマンドプロンプトを起動します。  
ツールの置いてあるフォルダに移動し、トレースログを読み込ませて実行します。  
  
オプション  
-d 入力されるトレースファイル名 (デフォルト：data.log)  
-o 出力される解析結果ファイル名（デフォルト：output.txt）  
  
![image11](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/11_command.png)
  
<br>
  
解析前のログと解析結果は以下の通り。  
  
![image12](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/12_tracelog_70.png)
![image13](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/13_parsedlog_70.png)
  
<br>

解析結果を復号化された.textセクションのコードに反映させることで、静的解析に利用することができる。  
以下の図では、解析結果のAPIコールをIDAの"n"コマンドでリネームした例。  

![image14](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/14_IDA_ParsedCode.png)

<br>
  
## How to Use
Obtain Function Trace using IDA.  
The operation example is as follows.  
  
1. Set the debugger to stop at the entry point in the Debugger setup dialog.  
![image1](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/01_IDA_Debugger_Setup.png)

2. Run debug and stop at entry point.  
![image2](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/02_IDA_Suspend_at_entrypoint.png)

3. Open the dialog with "Debugger->Tracing->Tracing options" and set "Trace text file" and check "Log return instructions".  
![image3](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/03_TracingOptions.png)

4. Start tracing with "Debugger->Tracing->Function tracing".  
![image4](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/04_IDA_tracing.png)

5. Resume debug. *Running the debugger while outputting traces takes time.  
![image5](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/05_IDA_Resume.png)

6. Stop at any point and obtain the trace log file.  
<br>
<br>

After obtaining the trace log, start the command prompt.  
Go to the folder where the tool is located and run the tool with trace file name.  
  
Option  
-d input(trace) file name (default:data.log)  
-o output file name（default:output.txt）  
  
![image11](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/11_command.png)
  
<br>
  
Below is a sample of the log before analysis and the analysis results.  
  
![image12](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/12_tracelog_70.png)
![image13](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/13_parsedlog_70.png)
  
<br>

Manually reflect the analysis results in the code of the decrypted .text section. This can be used for static analysis.  
The following figure shows an example of an API call parsed and renamed with IDA's "n" command.  

![image14](https://github.com/Sachiel-archangel/VMProtect_TraceParser/blob/main/image/14_IDA_ParsedCode.png)

<br>
  
