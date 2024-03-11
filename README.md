# VMProtect_TraceParser
A tool that parses the function trace of IDA that executes VMProtect.

## 概　要  
VMProtectで作成されたマルウェアに対し、IDA で出力したFunction Traceを分析して.textセクションから.vmp0を経由してコールしているWindowsAPIを出力する。  
これにより、復号化された.textセクションのコードを静的解析するために必要なAPIコールを把握することができる。  

## Outline
For malware created with VMProtect, analyze the Function Trace output by IDA and output the Windows API being called via .vmp0 from the .text section.
This allows you to understand the API calls required to statically analyze the code in the decrypted .text section.
