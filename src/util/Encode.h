#pragma once

#include <cstdlib>
#include <cstring>
#include <memory>

#ifdef _WIN32

inline std::string GBK_To_UTF8(const std::string& strGbk)//�����strGbk��GBK����  
{
	//gbkתunicode  
	auto len = MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, NULL, 0);
	std::unique_ptr<wchar_t[]> strUnicode(new wchar_t[len + 1] {});
	MultiByteToWideChar(CP_ACP, 0, strGbk.c_str(), -1, strUnicode.get(), len);

	//unicodeתUTF-8  
	len = WideCharToMultiByte(CP_UTF8, 0, strUnicode.get(), -1, NULL, 0, NULL, NULL);
	std::unique_ptr<char[]> strUtf8(new char[len + 1] {});
	WideCharToMultiByte(CP_UTF8, 0, strUnicode.get(), -1, strUtf8.get(), len, NULL, NULL);

	//��ʱ��strUtf8��UTF-8����  
	return std::string(strUtf8.get());
}

// UTF8תGBK
inline std::string UTF8_To_GBK(const std::string& strUtf8)
{
	//UTF-8תunicode  
	int len = MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, NULL, 0);
	std::unique_ptr<wchar_t[]> strUnicode(new wchar_t[len + 1] {});//len = 2  
	MultiByteToWideChar(CP_UTF8, 0, strUtf8.c_str(), -1, strUnicode.get(), len);

	//unicodeתgbk  
	len = WideCharToMultiByte(CP_ACP, 0, strUnicode.get(), -1, NULL, 0, NULL, NULL);
	std::unique_ptr<char[]>strGbk(new char[len + 1] {});//len=3 ����Ϊ2������char*�����Զ�������\0  
	WideCharToMultiByte(CP_ACP, 0, strUnicode.get(), -1, strGbk.get(), len, NULL, NULL);

	//��ʱ��strTemp��GBK����  
	return std::string(strGbk.get());
}

#else

// TODO
inline std::string GBK_To_UTF8(const std::string& strGbk)
{
	return strGbk;
}

// TODO
inline std::string UTF8_To_GBK(const std::string& strUtf8)
{
	return strUtf8;
}

#endif