#include <memory>
#include <string>
#include <stdarg.h>

inline [[nodiscard]] std::unique_ptr<char[]> pszprintf(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	const std::size_t size = vsnprintf(nullptr, 0, format, va);
	std::unique_ptr<char[]> up(new char[size + 1]);
	vsprintf(up.get(), format, va);
	return up;
}

inline [[nodiscard]] std::string strprintf(const char* format, ...)
{
	va_list va;
	va_start(va, format);
	const std::size_t size = vsnprintf(nullptr, 0, format, va);
	std::string str(size, '\0');
	vsprintf(str.data(), format, va);
	return str;
}
