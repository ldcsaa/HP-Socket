/*
 * Copyright: JessMA Open Source (ldcsaa@gmail.com)
 *
 * Author	: Bruce Liang
 * Website	: http://www.jessma.org
 * Project	: https://github.com/ldcsaa
 * Blog		: http://www.cnblogs.com/ldcsaa
 * Wiki		: http://www.oschina.net/p/hp-socket
 * QQ Group	: 75375912, 44636872
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
 
#include "stdafx.h"
#include "HttpCookie.h"

#ifdef _HTTP_SUPPORT

#pragma warning(disable: 4503)
#pragma warning(disable: 4840)

static const char* s_short_week[]	= {"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"};
static const char* s_short_month[]	= {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

CCookieMgr g_CookieMgr;

CCookie* CCookie::FromString(const CStringA& strCookie, LPCSTR lpszDefaultDomain, LPCSTR lpszDefaultPath)
{
	CStringA strName;
	CStringA strValue;
	CStringA strDomain;
	CStringA strPath;

	int iMaxAge			  = -1;
	BOOL bHttpOnly		  = FALSE;
	BOOL bSecure		  = FALSE;
	EnSameSite enSameSite = SS_NONE;

	int i		= 0;
	int iStart	= 0;

	while(TRUE)
	{
		CStringA strField = strCookie.Tokenize(COOKIE_FIELD_SEP, iStart);
		strField.Trim();

		if(strField.IsEmpty())
			break;

		if(i == 0)
		{
			ParseFieldKV(strField, strName, strValue, COOKIE_KV_SEP_CHAR);

			if(strName.IsEmpty())
				return nullptr;
		}
		else
		{
			CStringA strKey;
			CStringA strVal;

			ParseFieldKV(strField, strKey, strVal, COOKIE_KV_SEP_CHAR);

			if(strKey.CompareNoCase(COOKIE_DOMAIN) == 0)
				strDomain = strVal;
			else if(strKey.CompareNoCase(COOKIE_PATH) == 0)
				strPath = strVal;
			else if(strKey.CompareNoCase(COOKIE_MAX_AGE) == 0 && !strVal.IsEmpty())
				iMaxAge = atoi(strVal);
			else if(strKey.CompareNoCase(COOKIE_EXPIRES) == 0 && !strVal.IsEmpty() && iMaxAge == -1)
			{
				__time64_t tmExpires = -1;

				if(!ParseExpires(strVal, tmExpires))
					return nullptr;

				iMaxAge = ExpiresToMaxAge(tmExpires);
			}
			else if(strKey.CompareNoCase(COOKIE_HTTPONLY) == 0)
				bHttpOnly = TRUE;
			else if(strKey.CompareNoCase(COOKIE_SECURE) == 0)
				bSecure = TRUE;
			else if(strKey.CompareNoCase(COOKIE_SAMESITE) == 0)
			{
				if(strVal.IsEmpty() || strVal.CompareNoCase(COOKIE_SAMESITE_STRICT) == 0)
					enSameSite = SS_STRICT;
				else if(strVal.CompareNoCase(COOKIE_SAMESITE_LAX) == 0)
					enSameSite = SS_LAX;
			}
		}

		++i;
	}

	if(!AdjustDomain(strDomain, lpszDefaultDomain) || !AdjustPath(strPath, lpszDefaultPath))
		return nullptr;

	CCookie* pCookie = new CCookie(strName, strValue, strDomain, strPath, iMaxAge, bHttpOnly, bSecure, enSameSite);
	ASSERT(pCookie->IsValid());

	return pCookie;
}

CStringA CCookie::ToString(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge, BOOL bHttpOnly, BOOL bSecure, EnSameSite enSameSite)
{
	CCookie cookie(lpszName, lpszValue, lpszDomain, lpszPath, iMaxAge, bHttpOnly, bSecure, enSameSite);
	return cookie.ToString();
}

BOOL CCookie::ToString(char lpszBuff[], int& iBuffLen, LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge, BOOL bHttpOnly, BOOL bSecure, EnSameSite enSameSite)
{
	BOOL isOK	 = FALSE;
	CStringA str = ToString(lpszName, lpszValue, lpszDomain, lpszPath, iMaxAge, bHttpOnly, bSecure, enSameSite);
	int iLength	 = str.GetLength() + 1;

	if(lpszBuff && iBuffLen >= iLength)
	{
		memcpy(lpszBuff, (LPCSTR)str, iLength * sizeof(char));
		isOK = TRUE;
	}

	iBuffLen = iLength;

	return isOK;
}

CStringA CCookie::ToString()
{
	ASSERT(!name.IsEmpty());

	CStringA strCookie;

	strCookie.AppendFormat("%s=%s", name, value);

	if(!domain.IsEmpty())
		strCookie.AppendFormat("; %s=%s", COOKIE_DOMAIN, domain);
	if(!path.IsEmpty())
		strCookie.AppendFormat("; %s=%s", COOKIE_PATH, path);
	if(expires >= 0)
		strCookie.AppendFormat("; %s=%s", COOKIE_EXPIRES, MakeExpiresStr(expires));
	if(httpOnly)
		strCookie.AppendFormat("; %s", COOKIE_HTTPONLY);
	if(secure)
		strCookie.AppendFormat("; %s", COOKIE_SECURE);
	if(sameSite != SS_NONE)
		strCookie.AppendFormat("; %s=%s", COOKIE_SAMESITE, sameSite == SS_LAX ? COOKIE_SAMESITE_LAX : COOKIE_SAMESITE_STRICT);

	return strCookie;
}

BOOL CCookie::Match(LPCSTR lpszDomain, LPCSTR lpszPath, BOOL bHttp, BOOL bSecure)
{
	int iLen  = (int)strlen(lpszDomain);
	int iDiff = iLen - domain.GetLength();

	if(iDiff < 0 || _stricmp(lpszDomain + iDiff, domain) != 0)
		return FALSE;
	if(iDiff > 0 && *(lpszDomain + iDiff - 1) != COOKIE_DOMAIN_SEP_CHAR)
		return FALSE;
	if(strncmp(lpszPath, path, path.GetLength()) != 0)
		return FALSE;

	return (bHttp || !httpOnly) && (bSecure || !secure);
}

BOOL CCookie::IsSameDomain(LPCSTR lpszDomain)
{
	int iLen  = (int)strlen(lpszDomain);
	int iDiff = iLen - domain.GetLength();

	LPCSTR lpszLong, lpszShort;

	if(iDiff < 0)
	{
		lpszLong  = (LPCSTR)domain + iDiff;
		lpszShort = lpszDomain;
	}
	else
	{
		lpszLong  = lpszDomain + iDiff;
		lpszShort = (LPCSTR)domain;
	}

	if(_stricmp(lpszLong, lpszShort) != 0)
		return FALSE;
	if(iDiff != 0 && *(lpszLong - 1) != COOKIE_DOMAIN_SEP_CHAR)
		return FALSE;

	return TRUE;
}

void CCookie::ParseFieldKV(const CStringA& strField, CStringA& strKey, CStringA& strVal, char chSep)
{
	int i = strField.Find(chSep);

	if(i < 0)
		strKey = strField;
	else
	{
		strKey = strField.Left(i);
		strVal = strField.Mid(i + 1);

		strVal.Trim();
	}

	strKey.Trim();
}

BOOL CCookie::ParseExpires(LPCSTR lpszExpires, __time64_t& tmExpires)
{
	int iLength = (int)strlen(lpszExpires);

	if(iLength == 0 || iLength > 50)
		return FALSE;

	char szMonth[10];
	char szZone[10];

	tm t = {0};

	if(sscanf_s(lpszExpires, "%*[^, ]%*[, ]%2d%*[-/ ]%[^-/ ]%*[-/ ]%4d %2d:%2d:%2d %s", 
				&t.tm_mday, szMonth, (UINT)_countof(szMonth), &t.tm_year, &t.tm_hour, &t.tm_min, &t.tm_sec, szZone, (UINT)_countof(szZone)) != 7)
		return FALSE;

	if(t.tm_year < 70)
		t.tm_year += 100;
	else if (t.tm_year > 100)
		t.tm_year -= 1900;

	int i	 = 0;
	int size = _countof(s_short_month);

	for(; i < size; i++)
	{
		if(_strnicmp(szMonth, s_short_month[i], 3) == 0)
			break;
	}

	if(i == size)
		return FALSE;

	t.tm_mon = i;

	CStringA strZone = szZone;

	int iZone	= 0;
	int iMix	= 0;
	int iPos	= strZone.Find('+');

	if(iPos >= 0)
		iMix = 1;
	else
	{
		iPos = strZone.Find('-');

		if(iPos >= 0)
			iMix = -1;
	}

	if(iPos >= 0)
	{
		strZone = strZone.Mid(iPos + 1);
		strZone.Remove(':');

		int val = atoi(strZone);

		if(val > 0)
		{
			int minutes	= val % 100;
			int hours	= val / 100;

			iZone = iMix * (minutes * 60 + hours * 3600);
		}
	}

	tmExpires = GetUTCTime(t, iZone);

	return tmExpires >= 0;
}

BOOL CCookie::AdjustDomain(CStringA& strDomain, LPCSTR lpszDefaultDomain)
{
	if(strDomain.IsEmpty() && lpszDefaultDomain)
		strDomain = lpszDefaultDomain;

	strDomain.TrimLeft(COOKIE_DOMAIN_SEP_CHAR).MakeLower();

	return !strDomain.IsEmpty();
}

BOOL CCookie::AdjustPath(CStringA& strPath, LPCSTR lpszDefaultPath)
{
	if(strPath.IsEmpty() && lpszDefaultPath)
		strPath = lpszDefaultPath;

	int iLength = strPath.GetLength();

	if(iLength == 0)
		return FALSE;

	if(strPath.GetAt(iLength - 1) != COOKIE_PATH_SEP_CHAR)
	{
		int iPos = strPath.ReverseFind(COOKIE_PATH_SEP_CHAR);

		if(iPos >= 0)
			strPath = strPath.Left(iPos + 1);
		else
			strPath.Empty();
	}

	if(!strPath.IsEmpty() && strPath.GetAt(0) != COOKIE_PATH_SEP_CHAR)
		strPath.Insert(0, COOKIE_PATH_SEP_CHAR);

	return !strPath.IsEmpty();
}

__time64_t CCookie::GetUTCTime(tm& t, int iSecondOffsetTZ)
{
	__time64_t v = _mkgmtime64(&t);
	if(v >= 0) v -= iSecondOffsetTZ;

	return v;
}

CStringA CCookie::MakeExpiresStr(__time64_t tmExpires)
{
	ASSERT( tmExpires >= 0);

	if(tmExpires < 1) tmExpires = 1;

	tm t;
	ENSURE(_gmtime64_s(&t, &tmExpires) == 0);

	CStringA str;
	str.Format("%s, %02d-%s-%04d %02d:%02d:%02d GMT", 
				s_short_week[t.tm_wday], t.tm_mday, s_short_month[t.tm_mon], t.tm_year + 1900, t.tm_hour, t.tm_min, t.tm_sec);

	return str;
}

BOOL CCookie::MakeExpiresStr(char lpszBuff[], int& iBuffLen, __time64_t tmExpires)
{
	BOOL isOK	 = FALSE;
	CStringA str = MakeExpiresStr(tmExpires);
	int iLength	 = str.GetLength() + 1;

	if(lpszBuff && iBuffLen >= iLength)
	{
		memcpy(lpszBuff, (LPCSTR)str, iLength * sizeof(char));
		isOK = TRUE;
	}

	iBuffLen = iLength;

	return isOK;
}

// ------------------------------------------------------------------------------------------------------------- //

BOOL CCookieMgr::LoadFromFile(LPCSTR lpszFile, BOOL bKeepExists)
{
	BOOL isOK	= FALSE;
	FILE* pFile = nullptr;

	if(fopen_s(&pFile, lpszFile, "r") != NO_ERROR)
	{
		::SetLastError(errno == ENOENT ? ERROR_FILE_NOT_FOUND : (errno == EACCES ? ERROR_ACCESS_DENIED : ERROR_OPEN_FAILED));
		goto _ERROR_END;
	}

	{
		CStringA strDomain;
		CStringA strPath;
		CCookie cookie;


		char szBuffer[8192];
		int iBufferSize			= _countof(szBuffer);
		__time64_t tmCurrent	= _time64(nullptr);
		CCookieSet* pCookieSet	= nullptr;

		CWriteLock locallock(m_cs);

		if(!bKeepExists)
			ClearDomainCookiesNoLock();

		while(fgets(szBuffer, iBufferSize, pFile) != nullptr)
		{
			char c = szBuffer[0];

			if(c == '\n' || c == '\r')
				continue;
			else if(c != '\t')
			{
				if(!LoadDomainAndPath(szBuffer, strDomain, strPath))
					goto _ERROR_END;

				pCookieSet	= GetCookieSetNoLock(strDomain, strPath);
			}
			else
			{
				if(!LoadCookie(szBuffer, strDomain, strPath, cookie))
					goto _ERROR_END;

				if(cookie.expires <= tmCurrent)
					continue;

				if(pCookieSet)
				{
					if(bKeepExists)
					{
						CCookieSetCI it = pCookieSet->find(cookie);
						if(it != pCookieSet->end())
							continue;
					}

					pCookieSet->emplace(move(cookie));
				}
				else
				{
					SetCookieNoLock(cookie, FALSE);
					pCookieSet = GetCookieSetNoLock(strDomain, strPath);
				}
			}
		}

		if(!feof(pFile))
		{
			::SetLastError(ERROR_READ_FAULT);
			goto _ERROR_END;
		}
	}

	isOK = TRUE;

_ERROR_END:

	if(pFile) fclose(pFile);

	return isOK;
}

BOOL CCookieMgr::SaveToFile(LPCSTR lpszFile, BOOL bKeepExists)
{
	if(bKeepExists)
	{
		if(!LoadFromFile(lpszFile, TRUE) && ::GetLastError() != ERROR_FILE_NOT_FOUND)
			return FALSE;
	}

	BOOL isOK	= FALSE;
	FILE* pFile = nullptr;

	if(fopen_s(&pFile, lpszFile, "w") != NO_ERROR)
	{
		::SetLastError(errno == ENOENT ? ERROR_FILE_NOT_FOUND : (errno == EACCES ? ERROR_ACCESS_DENIED : ERROR_OPEN_FAILED));
		goto _ERROR_END;
	}

	{
		__time64_t tmCurrent = _time64(nullptr);

		CReadLock locallock(m_cs);

		for(CCookieDomainMapCI it = m_cookies.begin(), end = m_cookies.end(); it != end; ++it)
		{
			const CStringA& strDomain	= it->first;
			const CCookiePathMap& paths	= it->second;

			for(CCookiePathMapCI it2 = paths.begin(), end2 = paths.end(); it2 != end2; ++it2)
			{
				const CStringA& strPath		= it2->first;
				const CCookieSet& cookies	= it2->second;

				if(fprintf_s(pFile, "%s %s\n", (LPCSTR)strDomain, (LPCSTR)strPath) < 0)
				{
					::SetLastError(ERROR_WRITE_FAULT);
					goto _ERROR_END;
				}

				for(CCookieSetCI it3 = cookies.begin(), end3 = cookies.end(); it3 != end3; ++it3)
				{
					const CCookie& cookie = *it3;

					if(cookie.expires <= tmCurrent)
						continue;

					LPCSTR lpszValue = (LPCSTR)cookie.value;

					if(lpszValue[0] == 0)
						lpszValue = " ";

					if(fprintf_s(pFile, "\t%s;%s;%I64d;%d;%d;%d\n", (LPCSTR)cookie.name, lpszValue, cookie.expires, cookie.httpOnly, cookie.secure, cookie.sameSite) < 0)
					{
						::SetLastError(ERROR_WRITE_FAULT);
						goto _ERROR_END;
					}
				}
			}
		}
	}

	isOK = TRUE;

_ERROR_END:

	if(pFile) fclose(pFile);

	return isOK;
}

BOOL CCookieMgr::ClearCookies(LPCSTR lpszDomain, LPCSTR lpszPath)
{
	CStringA strDomain;
	CStringA strPath;

	if(!AdjustDomainAndPath(lpszDomain, lpszPath, strDomain, strPath, TRUE))
		return FALSE;

	CWriteLock locallock(m_cs);

	ClearDomainCookiesNoLock(lpszDomain, lpszPath);

	return TRUE;
}

BOOL CCookieMgr::RemoveExpiredCookies(LPCSTR lpszDomain, LPCSTR lpszPath)
{
	CStringA strDomain;
	CStringA strPath;

	if(!AdjustDomainAndPath(lpszDomain, lpszPath, strDomain, strPath, TRUE))
		return FALSE;

	CWriteLock locallock(m_cs);

	RemoveExpiredCookiesNoLock(lpszDomain, lpszPath);

	return TRUE;
}

BOOL CCookieMgr::GetCookies(CCookieSet& cookies, LPCSTR lpszDomain, LPCSTR lpszPath, BOOL bHttp, BOOL bSecure)
{
	ASSERT(lpszDomain && lpszPath);

	CStringA strDomain;
	CStringA strPath;

	if(!AdjustDomainAndPath(lpszDomain, lpszPath, strDomain, strPath, FALSE))
		return FALSE;

	list<LPCSTR> lsDomains(1, lpszDomain);
	list<CStringA> lsPaths(1, lpszPath);

	char c;
	LPCSTR lpszTemp = lpszDomain;

	while((c = *(++lpszTemp)) != 0)
	{
		if(c == COOKIE_DOMAIN_SEP_CHAR)
		{
			if((c = *(++lpszTemp)) != 0)
				lsDomains.push_back(lpszTemp);
			else
				break;
		}
	}

	lpszTemp = lpszPath + strlen(lpszPath) - 1;

	while(--lpszTemp >= lpszPath)
	{
		if((c = *lpszTemp) == COOKIE_PATH_SEP_CHAR)
		{
			*(LPSTR)(lpszTemp + 1) = 0;
			lsPaths.push_back(lpszPath);
		}
	}

	CReadLock locallock(m_cs);

	for(list<LPCSTR>::const_iterator it = lsDomains.begin(), end = lsDomains.end(); it != end; ++it)
	{
		for(list<CStringA>::const_iterator it2 = lsPaths.begin(), end2 = lsPaths.end(); it2 != end2; ++it2)
			MatchCookiesNoLock(cookies, *it, *it2, bHttp, bSecure);
	}

	return TRUE;
}

BOOL CCookieMgr::SetCookie(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge, BOOL bHttpOnly, BOOL bSecure, CCookie::EnSameSite enSameSite, BOOL bOnlyUpdateValueIfExists)
{
	CCookie cookie(lpszName, lpszValue, lpszDomain, lpszPath, iMaxAge, bHttpOnly, bSecure, enSameSite);

	return SetCookie(cookie, bOnlyUpdateValueIfExists);
}

BOOL CCookieMgr::SetCookie(const CStringA& strCookie, BOOL bOnlyUpdateValueIfExists)
{
	unique_ptr<CCookie> pCookie(CCookie::FromString(strCookie));
	if(!pCookie) return FALSE;

	return SetCookie(*pCookie, bOnlyUpdateValueIfExists);
}

BOOL CCookieMgr::SetCookie(const CCookie& cookie, BOOL bOnlyUpdateValueIfExists)
{
	if(!cookie.IsValid()) return FALSE;

	CWriteLock locallock(m_cs);

	return SetCookieNoLock(cookie, bOnlyUpdateValueIfExists);
}

BOOL CCookieMgr::DeleteCookie(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName)
{
	CCookie cookie(lpszName, nullptr, lpszDomain, lpszPath);

	return DeleteCookie(cookie);
}

BOOL CCookieMgr::DeleteCookie(const CCookie& cookie)
{
	if(!cookie.IsValid()) return FALSE;

	CWriteLock locallock(m_cs);

	return DeleteCookieNoLock(cookie);
}

void CCookieMgr::ClearDomainCookiesNoLock(LPCSTR lpszDomain, LPCSTR lpszPath)
{
	if(!lpszDomain && !lpszPath)
		m_cookies.clear();
	else if(!lpszPath)
		m_cookies.erase(lpszDomain);
	else
	{
		if(!lpszDomain)
		{
			for(CCookieDomainMapI it = m_cookies.begin(), end = m_cookies.end(); it != end; ++it)
				ClearPathCookiesNoLock(it->second, lpszPath);
		}
		else
		{
			CCookieDomainMapI it = m_cookies.find(lpszDomain);
			if(it != m_cookies.end())
				ClearPathCookiesNoLock(it->second, lpszPath);
		}
	}
}

void CCookieMgr::ClearPathCookiesNoLock(CCookiePathMap& paths, LPCSTR lpszPath)
{
	if(!lpszPath)
		paths.clear();
	else
	{
		CCookiePathMapI it = paths.find(lpszPath);
		if(it != paths.end())
			paths.erase(it->first);
	}
}

void CCookieMgr::RemoveExpiredCookiesNoLock(LPCSTR lpszDomain, LPCSTR lpszPath)
{
	if(!lpszDomain)
	{
		for(CCookieDomainMapI it = m_cookies.begin(), end = m_cookies.end(); it != end; ++it)
			RemoveDomainExpiredCookiesNoLock(it->second, lpszPath);
	}
	else
	{
		CCookieDomainMapI it = m_cookies.find(lpszDomain);
		if(it != m_cookies.end())
			RemoveDomainExpiredCookiesNoLock(it->second, lpszPath);
	}
}

void CCookieMgr::RemoveDomainExpiredCookiesNoLock(CCookiePathMap& paths, LPCSTR lpszPath)
{
	if(!lpszPath)
	{
		for(CCookiePathMapI it = paths.begin(), end = paths.end(); it != end; ++it)
			RemovePathExpiredCookiesNoLock(it->second);
	}
	else
	{
		CCookiePathMapI it = paths.find(lpszPath);
		if(it != paths.end())
			RemovePathExpiredCookiesNoLock(it->second);
	}
}

void CCookieMgr::RemovePathExpiredCookiesNoLock(CCookieSet& cookies)
{
	CCookiePtrSet ptrs;

	for(CCookieSetI it = cookies.begin(), end = cookies.end(); it != end; ++it)
	{
		if(it->IsExpired())
			ptrs.emplace(&*it);
	}

	if(!ptrs.empty())
	{
		for(CCookiePtrSetI it = ptrs.begin(), end = ptrs.end(); it != end; ++it)
			cookies.erase(**it);
	}
}

const CCookie* CCookieMgr::GetCookieNoLock(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName)
{
	const CCookie cookie(lpszName, nullptr, lpszDomain, lpszPath);
	return GetCookieNoLock(cookie);
}

const CCookie* CCookieMgr::GetCookieNoLock(const CCookie& cookie)
{
	const CCookie* pCookie = nullptr;

	CCookieDomainMapCI it = m_cookies.find(cookie.domain);
	if(it != m_cookies.end())
	{
		CCookiePathMapCI it2 = it->second.find(cookie.path);
		if(it2 != it->second.end())
		{
			CCookieSetCI it3 = it2->second.find(cookie);
			if(it3 != it2->second.end())
				pCookie = &*it3;
		}
	}

	return pCookie;
}

void CCookieMgr::MatchCookiesNoLock(CCookieSet& cookies, LPCSTR lpszDomain, LPCSTR lpszPath, BOOL bHttp, BOOL bSecure)
{
	CCookieDomainMapCI it = m_cookies.find(lpszDomain);
	if(it != m_cookies.end())
	{
		CCookiePathMapCI it2 = it->second.find(lpszPath);
		if(it2 != it->second.end())
		{
			for(CCookieSetCI it3 = it2->second.begin(), end3 = it2->second.end(); it3 != end3; ++it3)
			{
				const CCookie& cookie = *it3;

				if(!cookie.IsExpired() && (bHttp || !cookie.httpOnly) && (bSecure || !cookie.secure))
					cookies.emplace(cookie);
			}
		}
	}
}

BOOL CCookieMgr::SetCookieNoLock(const CCookie& cookie, BOOL bOnlyUpdateValueIfExists)
{
	if(cookie.IsExpired())
		return DeleteCookieNoLock(cookie);

	CCookieDomainMapI it = m_cookies.find(cookie.domain);

	if(it == m_cookies.end())
		it = m_cookies.emplace(move(CCookieDomainMap::value_type(cookie.domain, move(CCookiePathMap())))).first;

	CCookiePathMapI it2 = it->second.find(cookie.path);

	if(it2 == it->second.end())
		it2 = it->second.emplace(move(CCookiePathMap::value_type(cookie.path, move(CCookieSet())))).first;

	CCookieSet& cookies	= it2->second;
	CCookieSetI it3		= cookies.find(cookie);

	if(it3 != cookies.end())
	{
		if(bOnlyUpdateValueIfExists && !it3->IsExpired() && cookie.IsTransient())
		{
			((CCookie*)&*it3)->value = cookie.value;
			return TRUE;
		}
		
		cookies.erase(*it3);
	}

	return cookies.emplace(cookie).second;
}

BOOL CCookieMgr::DeleteCookieNoLock(const CCookie& cookie)
{
	BOOL isOK = FALSE;

	CCookieDomainMapI it = m_cookies.find(cookie.domain);
	if(it != m_cookies.end())
	{
		CCookiePathMapI it2 = it->second.find(cookie.path);
		if(it2 != it->second.end())
		{
			CCookieSetI it3 = it2->second.find(cookie);
			if(it3 != it2->second.end())
			{
				it2->second.erase(*it3);
				isOK = TRUE;
			}
		}
	}

	return isOK;
}

CCookieSet* CCookieMgr::GetCookieSetNoLock(LPCSTR lpszDomain, LPCSTR lpszPath)
{
	CCookieSet* pCookieSet	= nullptr;
	CCookieDomainMapI it	= m_cookies.find(lpszDomain);

	if(it != m_cookies.end())
	{
		CCookiePathMapI it2 = it->second.find(lpszPath);
		if(it2 != it->second.end())
			pCookieSet = &(it2->second);
	}

	return pCookieSet;
}

BOOL CCookieMgr::LoadDomainAndPath(LPSTR lpszBuff, CStringA& strDomain, CStringA& strPath)
{
	int i = 0;
	char* lpszCtx = nullptr;

	for(; i < 2; i++)
	{
		char* lpszToken = strtok_s(lpszBuff, " \n\r", &lpszCtx);

		if(!lpszToken)
		{
			::SetLastError(ERROR_BAD_FORMAT);
			return FALSE;
		}

		if(i == 0)
		{
			strDomain = lpszToken;
			lpszBuff  = nullptr;
		}
		else
			strPath = lpszToken;
	}

	if(!CCookie::AdjustDomain(strDomain))
		return FALSE;
	if(!CCookie::AdjustPath(strPath))
		return FALSE;

	return TRUE;
}

BOOL CCookieMgr::LoadCookie(LPSTR lpszBuff, LPCSTR lpszDomain, LPCSTR lpszPath, CCookie& cookie)
{
	cookie.domain	= lpszDomain;
	cookie.path		= lpszPath;

	int i			= 0;
	char* lpszCtx	= nullptr;

	for(; i < 6; i++)
	{
		char* lpszToken = strtok_s(lpszBuff, "\t;\n\r", &lpszCtx);

		if(!lpszToken)
		{
			::SetLastError(ERROR_BAD_FORMAT);
			return FALSE;
		}

		if(i == 0)
		{
			cookie.name		= lpszToken;
			lpszBuff		= nullptr;
		}
		else if(i == 1)
			cookie.value	= lpszToken;
		else if(i == 2)
			cookie.expires	= _atoi64(lpszToken);
		else if(i == 3)
			cookie.httpOnly	= atoi(lpszToken);
		else if(i == 4)
			cookie.secure	= atoi(lpszToken);
		else if(i == 5)
			cookie.sameSite	= (CCookie::EnSameSite)atoi(lpszToken);
	}

	cookie.name.Trim();
	cookie.value.Trim();

	if(cookie.name.IsEmpty() || cookie.expires <= 0 || cookie.httpOnly < 0 || cookie.secure < 0 || cookie.sameSite < 0)
	{
		::SetLastError(ERROR_INVALID_DATA);
		return FALSE;
	}

	return TRUE;
}

BOOL CCookieMgr::AdjustDomainAndPath(LPCSTR& lpszDomain, LPCSTR& lpszPath, CStringA& strDomain, CStringA& strPath, BOOL bKeepNull)
{
	if(!bKeepNull || lpszDomain)
	{
		strDomain = lpszDomain;

		if(!CCookie::AdjustDomain(strDomain))
			return FALSE;

		lpszDomain = (LPCSTR)strDomain;
	}

	if(!bKeepNull || lpszPath)
	{
		strPath = lpszPath;

		if(!CCookie::AdjustPath(strPath))
			return FALSE;

		lpszPath = (LPCSTR)strPath;
	}

	return TRUE;
}

CCookieMgr::CCookieMgr(BOOL bEnableThirdPartyCookie)
: m_bEnableThirdPartyCookie(bEnableThirdPartyCookie)
{

}

#endif