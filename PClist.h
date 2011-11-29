#include "afxinet.h"
#include <vector>
using namespace std;

#pragma once



struct PCNode 
{
	CString name;
	CString ip;
	CString MAC;
	int rate;

};

class PClist
{
public:
	PClist(void);
	~PClist(void);
	vector<PCNode> GetList();
	void GenList();
	void ConvertSpd();			//���ֽ�ÿ��ת����KBÿ��
	int GetListSize();

public:
	vector<PCNode> list;

};
