
void wchar_to_char(const wchar_t* wchr, char* str, int length) {
	WideCharToMultiByte(CP_ACP, 0, wchr, length / 2, str, 99, NULL, NULL);
	str[length / 2] = '\0';
}