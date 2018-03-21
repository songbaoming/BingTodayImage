#include "StdAfx.h"
#include <WS2tcpip.h>
#include <string>
#include <fstream>
#include <atlimage.h>
#include "BingTodayImage.h"
#include "rapidjson/document.h"

#pragma comment(lib, "ws2_32.lib")

LPCSTR CBingTodayImage::s_pHttpHdr = 
	"GET %s HTTP/1.1\r\n"
	"Proxy-Connection: keep-alive\r\n"
	"Upgrade-Insecure-Requests: 1\r\n"
	"User-Agent: Mozilla/5.0 (Windows NT 6.1; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/64.0.3282.186 Safari/537.36\r\n"
	"Accept: text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,image/apng,*/*;q=0.8\r\n"
	"Accept-Language: zh-CN,zh;q=0.9\r\n"
	"Host: cn.bing.com\r\n"
	"Content-Length: 0\r\n"
	"\r\n";


CBingTodayImage::CBingTodayImage(void)
	: m_hBing(INVALID_SOCKET)
	, m_pBuff(nullptr)
{
}


CBingTodayImage::~CBingTodayImage(void)
{
	if(m_hBing != INVALID_SOCKET)
		closesocket(m_hBing);
	if(m_pBuff)
		delete[] m_pBuff;
}

bool CBingTodayImage::GetTodayImage(LPCTSTR lpszSavePath, bool bSetAsWallpaper /*= true*/)
{
	if(!ConnectBing())
		return false;
	int nLen;
	auto pHttp = HttpGet("/HPImageArchive.aspx?format=js&idx=0&n=1&nc=1421741858945&pid=hp", nLen);
	if(!pHttp)
		return false;
	int nDataLen;

	auto pData = GetHttpData(pHttp, nLen, nDataLen);
	if(!pData)
		return false;
	rapidjson::Document doc;
	doc.Parse(pData);
	auto &images = doc["images"];
	auto &arry = images[0];
	auto url = arry["url"].GetString();
	m_strDesc = arry["copyright"].GetString();

	pHttp = HttpGet(url, nLen);
	if(!pHttp)
		return false;
	pData = GetHttpData(pHttp, nLen, nDataLen);
	auto pFileSize = GetHttpItamData(pHttp, "Content-Length");
	if(!pData || !pFileSize)
		return false;
	int nFileSize = atoi(pFileSize);

	std::fstream f(lpszSavePath, std::ios::binary | std::ios::out);
	if(!f)
		return false;
	f.write(pData, nDataLen);
	for(nFileSize -= nDataLen;
		nFileSize > 0 && SOCKET_ERROR != (nDataLen = recv(m_hBing, m_pBuff, BUFF_LEN, 0));
		nFileSize -= nDataLen){
			f.write(m_pBuff, nDataLen);
	}
	f.close();
	if(bSetAsWallpaper){
		CImage image;
		image.Load(lpszSavePath);
		image.Save(lpszSavePath);
		SystemParametersInfo(SPI_SETDESKWALLPAPER, 0, (PVOID)lpszSavePath, SPIF_UPDATEINIFILE | SPIF_SENDCHANGE);
	}
	return true;
}

bool CBingTodayImage::ConnectBing()
{
	ADDRINFO *pRes = nullptr, hints = {};
	hints.ai_family = AF_INET;

	auto nRes = getaddrinfo("www.cn.bing.com", "http", &hints, &pRes);
	if(nRes)
		return false;

	SOCKADDR_IN svrAddr = {};
	for(auto p = pRes; p; p = p->ai_next){
		if(p->ai_addr->sa_family == AF_INET){
			svrAddr = *((SOCKADDR_IN*)p->ai_addr);
			break;
		}
	}

	freeaddrinfo(pRes);
	if(svrAddr.sin_family != AF_INET)
		return false;

	m_hBing = WSASocket(AF_INET, SOCK_STREAM, 0, NULL, 0, 0);
	if(m_hBing == INVALID_SOCKET)
		return false;

	SOCKADDR_IN localAddr;
	localAddr.sin_family = AF_INET;
	localAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	localAddr.sin_port = 0;

	if(SOCKET_ERROR == bind(m_hBing, (sockaddr*)&localAddr, sizeof(localAddr)))
		return false;

	if(SOCKET_ERROR == connect(m_hBing, (sockaddr*)&svrAddr, sizeof(svrAddr)))
		return false;
	return true;
}

char * CBingTodayImage::HttpGet(LPCSTR lpszFile, int &nDataLen)
{
	if(!m_pBuff){
		m_pBuff = new char[BUFF_LEN];
		if(!m_pBuff)
			return nullptr;
	}

	sprintf_s(m_pBuff, BUFF_LEN, s_pHttpHdr, lpszFile);

	if(SOCKET_ERROR == send(m_hBing, m_pBuff, strlen(m_pBuff), 0))
		return nullptr;

	nDataLen = recv(m_hBing, m_pBuff, BUFF_LEN, 0);
	return (nDataLen != SOCKET_ERROR && strstr(m_pBuff, "200 OK")) ? m_pBuff : nullptr;
}

char * CBingTodayImage::GetHttpData(LPSTR pHttp, int nLen, int &nDataLen)
{
	auto pData = GetHttpItamData(pHttp);
	auto pType = GetHttpItamData(pHttp, "Content-Type");
	if(pType && strstr(pType, "charset=utf-8")){
		int nSrcLen = nLen - (pData - pHttp);
		int nDstLen = BUFF_LEN - nLen;
		auto pWide = pHttp + nLen;
		nSrcLen = MultiByteToWideChar(CP_UTF8, 0, pData, nSrcLen, (LPWSTR)pWide, nDstLen / sizeof(WCHAR));
		nDataLen = WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)pWide, nSrcLen, pData, BUFF_LEN - nLen, nullptr, nullptr);
		pData[nDataLen] = 0;
	}else
		nDataLen = nLen - (pData - pHttp);
	return pData;
}
