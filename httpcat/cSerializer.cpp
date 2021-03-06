/*
 *
 *  Copyright (C) 2011-2012 Amr Thabet <amr.thabet[at]student.alx.edu.eg>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to Amr Thabet
 *  amr.thabet[at]student.alx.edu.eg
 *
 */

#include "SRDFUtils.h"
#include <iostream>

using namespace std;


cString _cdecl cSerializer::Serialize(bool AddRoot)
{
	if (RootName.GetLength() == 0)RootName = "SerializableObject";
	cXMLHash* XMLParams = new cXMLHash();
	SetSerialize(*XMLParams);
	cString XML = "";
	if (AddRoot)
	{
		XML << "<" << RootName << ">";
		//cout << XML << "\n";
	}
	for(DWORD i = 0;i < XMLParams->nItems; i++)
	{
		XML << "\n<" << (char*)XMLParams->GetKey(i) << ">";
		XML << XMLParams->GetValue(i);
		XML<< "</" << (char*)XMLParams->GetKey(i) << ">";
	}
	if (AddRoot)
	{
		XML << "</" << RootName << ">";
		//cout << XML << "\n";
	}
	delete XMLParams;
	return XML;
}

cString cSerializer::SerializeObject(cXMLHash* XMLParams)
{
	cString XML = "";
	for(DWORD i = 0;i < XMLParams->nItems; i++)
	{
		XML << "\n<" << (char*)XMLParams->GetKey(i) << ">";
		XML << XMLParams->GetValue(i);
		XML<< "</" << (char*)XMLParams->GetKey(i) << ">";
	}
	return XML;
}
cXMLHash*  cSerializer::DeserializeObject(cString XMLDocument)
{
	cXMLHash* XMLParams = new cXMLHash();
	int i = 0;
	cString Key;
	cString CheckKey;		//To check the begining and the end if identical <X> </X>
	cString Value;
	while(XMLDocument[i] != '\0')
	{
		if(XMLDocument[i] == ' ' || XMLDocument[i] == '\n')
		{
			i++;
			continue;
		}
		if(XMLDocument[i] == '<' && XMLDocument[i+1] != '/')
		{
			i++;
			int KeyBegin = i;
			//Search for '>' to get the key
			while(1)
			{
				if (XMLDocument[i] == '>')break;
				if (XMLDocument[i] == '\0')goto FINISH;
				i++;
			}
			Key.Substr(XMLDocument,KeyBegin,i-KeyBegin);
			i++;
			int ValueBegin = i;
			i = SkipInside(XMLDocument,i);
			Value.Substr(XMLDocument,ValueBegin,i-ValueBegin);
			if(XMLDocument[i] == '<' && XMLDocument[i+1] == '/')
			{
				i+=2;				//Skip The "</"
				int CheckKeyBegin = i;
				//Search for '>' to get the key
				while(1)
				{
					if (XMLDocument[i] == '>')break;
					if (XMLDocument[i] == '\0')goto FINISH;
					i++;
				}
				CheckKey.Substr(XMLDocument,CheckKeyBegin,i-CheckKeyBegin);
				XMLParams->AddItem(Key,Value);
				if (Key != CheckKey)return NULL;
				i++;
			}else return NULL;
			
		}
		else return NULL;
	}
FINISH:
	return XMLParams;
}
void cSerializer::Deserialize(cString XMLDocument,bool WithRoot)
{
	cXMLHash* XMLParams = NULL;
	if (WithRoot)
	{
		cXMLHash* XMLRoot = DeserializeObject(XMLDocument);
		RootName = XMLRoot->GetKey(0);
		XMLParams = DeserializeObject(XMLRoot->GetValue(0));
		delete XMLRoot;
	}
	else
	{
		XMLParams = DeserializeObject(XMLDocument);
	}
	if (XMLParams != NULL)
	{
		GetSerialize(*XMLParams);
		delete XMLParams;
	}
}

DWORD cSerializer::SkipInside(cString XMLDocument,int offset)
{
	int i = offset;
	while(XMLDocument[i] != '\0')
	{
		if(XMLDocument[i] == '<' && XMLDocument[i+1] != '/')
		{
			i++;
			while(1)
			{
				if(XMLDocument[i] == '\0')return i-1;
				if(XMLDocument[i] == '>')break;
				i++;
			}
			i = SkipInside(XMLDocument,i);
			if(XMLDocument[i] == '<' && XMLDocument[i+1] == '/')
			{
				i+=2;
				while(1)
				{
					if(XMLDocument[i] == '\0')return i-1;
					if(XMLDocument[i] == '>')break;
					i++;
				}	
			}
			i++;
			continue;
		}
		if(XMLDocument[i] == '<' && XMLDocument[i+1] == '/')
		{
			return i;
		}
		i++;
	}
	return i-1;
}

void cSerializer::SetSerialize(cXMLHash& XMLParams)
{
	cout << "Error Inside !!!\n";
}

void cSerializer::GetSerialize(cXMLHash& XMLParams)
{
	cout << "Error Inside !!!\n";
}

