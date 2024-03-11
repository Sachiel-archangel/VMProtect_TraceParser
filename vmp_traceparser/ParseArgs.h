#pragma once
class ParseArgs
{
private:
	wchar_t* strDataFileName;
	wchar_t* strOutFileName;

public:
	ParseArgs();
	~ParseArgs();

	int Parse(int argc, wchar_t* argv[]);
	wchar_t* GetDataFileName();
	wchar_t* GetOutFileName();

};

