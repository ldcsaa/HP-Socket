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
 
#pragma once

#include "../Common/Src/RWLock.h"

#define COOKIE_DOMAIN			"domain"
#define COOKIE_PATH				"path"
#define COOKIE_EXPIRES			"expires"
#define COOKIE_MAX_AGE			"Max-Age"
#define COOKIE_HTTPONLY			"HttpOnly"
#define COOKIE_SECURE			"secure"
#define COOKIE_SAMESITE			"SameSite"
#define COOKIE_SAMESITE_STRICT	"Strict"
#define COOKIE_SAMESITE_LAX		"Lax"
#define COOKIE_DEFAULT_PATH		"/"
#define COOKIE_FIELD_SEP		";"
#define COOKIE_DOMAIN_SEP_CHAR	'.'
#define COOKIE_PATH_SEP_CHAR	'/'
#define COOKIE_KV_SEP_CHAR		'='

#pragma warning(push)
#pragma warning(disable: 4503)

class CCookie
{
public:
	enum EnSameSite
	{
		SS_NONE		= 0,
		SS_STRICT	= 1,
		SS_LAX		= 2
	};

	CStringA name;
	CStringA value;
	CStringA domain;
	CStringA path;
	__time64_t expires;
	BOOL httpOnly;
	BOOL secure;
	EnSameSite sameSite;

	CCookie(LPCSTR lpszName = nullptr, LPCSTR lpszValue = nullptr, LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr, int iMaxAge = -1, BOOL bHttpOnly = FALSE, BOOL bSecure = FALSE, EnSameSite enSameSite = SS_NONE)
	: name		(lpszName)
	, value		(lpszValue)
	, domain	(lpszDomain)
	, path		(lpszPath)
	, expires	(MaxAgeToExpires(iMaxAge))
	, httpOnly	(bHttpOnly)
	, secure	(bSecure)
	, sameSite	(enSameSite)
	{
		AdjustDomain(domain);
		AdjustPath(path);
	}

	static __time64_t CurrentUTCTime()					{return _time64(nullptr);}
	static __time64_t MaxAgeToExpires(int iMaxAge)		{return iMaxAge > 0 ? _time64(nullptr) + iMaxAge : (iMaxAge < 0 ? -1 : 0);}
	static int ExpiresToMaxAge(__time64_t tmExpires)	{if(tmExpires < 0) return -1; __time64_t tmDiff = tmExpires - _time64(nullptr); return (tmDiff > 0 ? (int)tmDiff : 0);}

	static __time64_t GetUTCTime(tm& t, int iSecondOffsetTZ);
	static void ParseFieldKV(const CStringA& strField, CStringA& strKey, CStringA& strVal, char chSep);
	static BOOL ParseExpires(LPCSTR lpszExpires, __time64_t& tmExpires);
	static BOOL AdjustDomain(CStringA& strDomain, LPCSTR lpszDefaultDomain = nullptr);
	static BOOL AdjustPath(CStringA& strPath, LPCSTR lpszDefaultPath = nullptr);
	static CStringA MakeExpiresStr(__time64_t tmExpires);
	static BOOL MakeExpiresStr(char lpszBuff[], int& iBuffLen, __time64_t tmExpires);
	static CCookie* FromString(const CStringA& strCookie, LPCSTR lpszDefaultDomain = nullptr, LPCSTR lpszDefaultPath = nullptr);
	static CStringA ToString(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge = -1, BOOL bHttpOnly = FALSE, BOOL bSecure = FALSE, EnSameSite enSameSite = SS_NONE);
	static BOOL ToString(char lpszBuff[], int& iBuffLen, LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge = -1, BOOL bHttpOnly = FALSE, BOOL bSecure = FALSE, EnSameSite enSameSite = SS_NONE);

	CStringA ToString();
	BOOL Match(LPCSTR lpszDomain, LPCSTR lpszPath, BOOL bHttp, BOOL bSecure);
	BOOL IsSameDomain(LPCSTR lpszDomain);

	BOOL IsTransient()	const	{return expires < 0;}
	BOOL IsExpired()	const	{return !IsTransient() && expires <= _time64(nullptr);}
	BOOL IsValid()		const	{return !name.IsEmpty() && !domain.IsEmpty() && !path.IsEmpty();}
};

struct ccookie_hash_func
{
	struct hash
	{
		size_t operator() (const CCookie& c) const
		{
			return hash_value((LPCSTR)(c.name));
		}
	};

	struct equal_to
	{
		bool operator () (const CCookie& cA, const CCookie& cB) const
		{
			return (cA.name == cB.name);
		}
	};

};

// ------------------------------------------------------------------------------------------------------------- //

typedef unordered_set<const CCookie*>									CCookiePtrSet;
typedef CCookiePtrSet::const_iterator									CCookiePtrSetCI;
typedef CCookiePtrSet::iterator											CCookiePtrSetI;

typedef unordered_set<CCookie,
		ccookie_hash_func::hash, ccookie_hash_func::equal_to>			CCookieSet;
typedef CCookieSet::const_iterator										CCookieSetCI;
typedef CCookieSet::iterator											CCookieSetI;

typedef unordered_map<CStringA, CCookieSet,
		cstringa_hash_func::hash, cstringa_hash_func::equal_to>			CCookiePathMap;
typedef CCookiePathMap::const_iterator									CCookiePathMapCI;
typedef CCookiePathMap::iterator										CCookiePathMapI;

typedef unordered_map<CStringA, CCookiePathMap,
		cstringa_nc_hash_func::hash, cstringa_nc_hash_func::equal_to>	CCookieDomainMap;
typedef CCookieDomainMap::const_iterator								CCookieDomainMapCI;
typedef CCookieDomainMap::iterator										CCookieDomainMapI;

class CCookieMgr
{
public:
	BOOL LoadFromFile(LPCSTR lpszFile, BOOL bKeepExists = TRUE);
	BOOL SaveToFile(LPCSTR lpszFile, BOOL bKeepExists = TRUE);

	BOOL ClearCookies(LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr);
	BOOL RemoveExpiredCookies(LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr);

	BOOL GetCookies(CCookieSet& cookies, LPCSTR lpszDomain, LPCSTR lpszPath, BOOL bHttp, BOOL bSecure);
	BOOL SetCookie(LPCSTR lpszName, LPCSTR lpszValue, LPCSTR lpszDomain, LPCSTR lpszPath, int iMaxAge = -1, BOOL bHttpOnly = FALSE, BOOL bSecure = FALSE, CCookie::EnSameSite enSameSite = CCookie::SS_NONE, BOOL bOnlyUpdateValueIfExists = TRUE);
	BOOL SetCookie(const CStringA& strCookie, BOOL bOnlyUpdateValueIfExists = TRUE);
	BOOL SetCookie(const CCookie& cookie, BOOL bOnlyUpdateValueIfExists = TRUE);
	BOOL DeleteCookie(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName);
	BOOL DeleteCookie(const CCookie& cookie);

private:
	void ClearDomainCookiesNoLock(LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr);
	void ClearPathCookiesNoLock(CCookiePathMap& paths, LPCSTR lpszPath = nullptr);
	void RemoveExpiredCookiesNoLock(LPCSTR lpszDomain = nullptr, LPCSTR lpszPath = nullptr);
	void RemoveDomainExpiredCookiesNoLock(CCookiePathMap& paths, LPCSTR lpszPath = nullptr);
	void RemovePathExpiredCookiesNoLock(CCookieSet& cookies);
	const CCookie* GetCookieNoLock(LPCSTR lpszDomain, LPCSTR lpszPath, LPCSTR lpszName);
	const CCookie* GetCookieNoLock(const CCookie& cookie);
	void MatchCookiesNoLock(CCookieSet& cookies, LPCSTR lpszDomain, LPCSTR lpszPath, BOOL bHttp = TRUE, BOOL bSecure = FALSE);
	BOOL SetCookieNoLock(const CCookie& cookie, BOOL bOnlyUpdateValueIfExists = TRUE);
	BOOL DeleteCookieNoLock(const CCookie& cookie);
	CCookieSet* GetCookieSetNoLock(LPCSTR lpszDomain, LPCSTR lpszPath);

private:
	static BOOL LoadDomainAndPath(LPSTR lpszBuff, CStringA& strDomain, CStringA& strPath);
	static BOOL LoadCookie(LPSTR lpszBuff, LPCSTR lpszDomain, LPCSTR lpszPath, CCookie& cookie);
	static BOOL AdjustDomainAndPath(LPCSTR& lpszDomain, LPCSTR& lpszPath, CStringA& strDomain, CStringA& strPath, BOOL bKeepNull = TRUE);

public:
	CCookieMgr(BOOL bEnableThirdPartyCookie = TRUE);

	void SetEnableThirdPartyCookie	(BOOL bEnableThirdPartyCookie = TRUE)	{m_bEnableThirdPartyCookie = bEnableThirdPartyCookie;}
	BOOL IsEnableThirdPartyCookie	()										{return m_bEnableThirdPartyCookie;}

private:
	CSimpleRWLock		m_cs;
	CCookieDomainMap	m_cookies;
	BOOL				m_bEnableThirdPartyCookie;
};

extern CCookieMgr g_CookieMgr;

#pragma warning(pop)
