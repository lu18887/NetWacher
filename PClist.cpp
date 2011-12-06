#include "StdAfx.h"
#include "PClist.h"
#include <atlstr.h>


PClist::PClist(void)
{
}

PClist::~PClist(void)
{
}

void PClist::GenList()
{

	//Declare a thread-safe, growable, private heap with initial size 0
	CWin32Heap g_stringHeap( 0, 0, 0 );
	// Declare a string manager that uses the private heap
	CAtlStringMgr g_stringMgr( &g_stringHeap ); 

	CInternetSession m_InterSession(_T("session"));

	CHttpConnection* pServer = NULL;
	CHttpFile* pFile = NULL;
	CString strHtml(&g_stringMgr);
	
	CString strAuth = "Authorization: Basic YWRtaW46YWRtaW4=\r\n";
	CString strUA = "User-Agent: Mozilla/5.0 (Windows NT 6.1) AppleWebKit/535.2 (KHTML, like Gecko) Chrome/15.0.874.121 Safari/535.2\r\n";
	CString strAccept = "Accept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n";
	CString strReferer = "Referer: http://192.168.1.1/userRpm/MenuRpm.htm\r\n";
	CString strAcceptEncode = "Accept-Encoding: gzip,deflate,sdch\r\n";
	CString strAccLang = "Accept-Language: zh-CN,zh;q=0.8\r\n";
	CString strAccCharset = "Accept-Charset: GBK,utf-8;q=0.7,*;q=0.3\r\n";
	CString strRateReqURI = "/userRpm/SystemStatisticRpm.htm";				//获取流量页面的URI
	CString strAssReqURI  = "/userRpm/AssignedIpAddrListRpm.htm";				//获取DHCP页面的URI
	CString strHeader = strAuth+strUA+strAccept+strReferer+strAcceptEncode+strAccLang+strAccCharset+"\r\n";
	vector<PCNode> pclist;
	
	try
	{
		INTERNET_PORT nPort = 80;
		pServer = m_InterSession.GetHttpConnection(_T("192.168.1.1"));
		pFile = pServer->OpenRequest(_T("GET"),(LPCTSTR)strRateReqURI,_T("http://192.168.1.1/userRpm/MenuRpm.htm"),1,NULL,_T("HTTP/1.1"));
		pFile->SendRequest(strHeader);
		
		DWORD dwRet;
		pFile->QueryInfoStatusCode(dwRet);
		
		bool goon = true;
		CString stln(&g_stringMgr);
		
		if (dwRet==HTTP_STATUS_OK)
		{
			
			pFile->ReadString(stln);
			pFile->ReadString(stln);
			while (goon)
			{
				pFile->ReadString(stln);
				if (stln.ReverseFind(L' ')!=stln.GetLength()-1)   //查找空格是否在最后一个位置
				{
					goon = false;
				}
				else							//说明是有效行，进行字符串拆分，取IP，MAC，流量字节数
				{
					PCNode temp;				//具体格式如：0, "192.168.1.100", "8C-7B-9D-EF-C6-63", 9846, 5620267, 0, 0, 0, 0, 0, 0, 1, 0, 4,最末尾还有个空格
					CString strTemp(&g_stringMgr);
					
					int pos = stln.Find('"');
					stln = stln.Right(stln.GetLength()-pos-1);

					pos = stln.Find('"');
					temp.ip = stln.Left(pos);        //取出IP了
					stln = stln.Right(stln.GetLength()-pos-1);

					pos = stln.Find('"');						//开始截获MAC
					stln =stln.Right(stln.GetLength()-pos-1);

					pos = stln.Find('"');
					temp.MAC = stln.Left(pos);
					stln = stln.Right(stln.GetLength()-pos-1);		//取出MAC了
				//这时剩下：, 9846, 5620267, 0, 0, 0, 0, 0, 0, 1, 0, 4,最末尾还有个空格

					pos = stln.Find(' ');
					stln = stln.Right(stln.GetLength()-pos-1);
					pos = stln.Find(' ');
					stln = stln.Right(stln.GetLength()-pos-1);
					pos = stln.Find(' ');
					stln = stln.Right(stln.GetLength()-pos-1);
					pos = stln.Find(' ');
					stln = stln.Right(stln.GetLength()-pos-1);

					pos = stln.Find(',');
					strTemp = stln.Left(pos);

					temp.rate=atoi(strTemp);
					
					//result.WriteString(temp.ip+_T(";")+temp.MAC+_T(";")+strTemp+_T("\r"));
					pclist.push_back(temp);
					
					strTemp.Empty();
					
				}

			}
		}

		

		pFile->Close();
		delete pFile;
		
		//接下来获取MAC对用的PC名字

		pFile = pServer->OpenRequest(_T("GET"),(LPCTSTR)strAssReqURI,_T("http://192.168.1.1/userRpm/MenuRpm.htm"),1,NULL,_T("HTTP/1.1"));
		pFile->SendRequest(strHeader);

		pFile->QueryInfoStatusCode(dwRet);
	    if (dwRet==HTTP_STATUS_OK)
	    {
			goon = true;
			pFile->ReadString(stln);
			pFile->ReadString(stln);
			while(goon)
			{
				pFile->ReadString(stln);
				if (stln.ReverseFind(L' ')!=stln.GetLength()-1)   //查找空格是否在最后一个位置
				{
					goon = false;
				}
				else
				{
					CString nameTemp(&g_stringMgr);   //"XXX", "00-22-5F-7A-B5-F1", "192.168.1.103", "01:21:29", 
					CString macTemp(&g_stringMgr);
					int pos = stln.Find('"');
					stln = stln.Right(stln.GetLength()-pos-1);

					pos = stln.Find('"');
					nameTemp = stln.Left(pos);        //取出PC名了
					stln = stln.Right(stln.GetLength()-pos-1);

					pos = stln.Find('"');
					stln = stln.Right(stln.GetLength()-pos-1);
					pos = stln.Find('"');
					macTemp = stln.Left(pos);        //取出PC名了

					bool aa = true;
					for (int i=0; i<pclist.size()&&aa;i++)
					{
						if (pclist[i].MAC==macTemp)
						{
							aa = false;
							pclist[i].name = nameTemp;
						}
					}


				}



			}


	    }
		stln.Empty();
	
		
		pServer->Close();
		pFile->Close();
		
		delete pServer;
		delete pFile;
		m_InterSession.Close();
	}
	catch (CInternetException* e)
	{
		e->m_dwContext;
		
	}
	strHeader.Empty();
	strHtml.Empty();
	strRateReqURI.Empty();
	strReferer.Empty();
	strUA.Empty();
	strAssReqURI.Empty();
	strAuth.Empty();
	strRateReqURI.Empty();
	

	this->list=pclist;
}

vector<PCNode> PClist::GetList()
{
	return this->list;
}


void PClist::ConvertSpd()
{
	for (int i = 0; i<list.size();i++)
	{
		list[i].rate/=1024;
	}

}

int PClist::GetListSize()
{

	return list.size();

}