#pragma once

template <int T>
struct CXXBYTE
{
	char m_Buf[T];
	
	bool operator==(const CXXBYTE& rhs) const
	{
		return !RC::Unreal::FCStringAnsi::Strncmp(m_Buf, rhs.m_Buf, T);
	}
	
	bool operator==(const char* rhs) const
	{
		return !RC::Unreal::FCStringAnsi::Strncmp(m_Buf, rhs, T);
	}
};