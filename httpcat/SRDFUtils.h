#pragma once
#include "cString.h"
#include "os.h"

//--------------------------------------//
//--       Serializer Namespace       --//
//--------------------------------------//
class cXMLHash;
class cSerializer
{
private:
	DWORD SkipInside(cString XMLDocument,int offset);		//it returns the new offset of the end;
protected:
	cString RootName;
public:
	cSerializer(){RootName = "SerializableObject";};
	~cSerializer(){};
	cString _cdecl Serialize(bool AddRoot = false);
	void Deserialize(cString XMLDocument,bool WithRoot = false);
	virtual void SetSerialize(cXMLHash& XMLParams);
	virtual void GetSerialize(cXMLHash& XMLParams);
	cString SerializeObject(cXMLHash* XMLParams);
	cXMLHash* DeserializeObject(cString XMLDocument);
};


//--------------------------------------//
//--        Strings Namespace         --//
//--------------------------------------//

class cHash : public cSerializer
{
protected:
	struct HASH_STRUCT
	{
		cString* Name;
		cString* Value;
	};
	HASH_STRUCT* HashArray;
public:
	//To be used in XML
	cString ItemName;
	cString KeyName;
	cString ValueName;
public:
	cHash();
	cHash(cString rootName,cString itemName,cString keyName,cString valueName);
	~cHash();
	DWORD nItems;
	DWORD GetNumberOfItems(cString Name);
	DWORD GetNumberOfItems();
	void AddItem(cString Name,cString Value);
	cString operator[](cString Name);
	cString operator[](DWORD id);
	cString GetKey(DWORD id);
	cString GetValue(DWORD id);
	cString GetValue(cString Name,int id = 0);
	void RemoveItem(DWORD id);
	void RemoveItem(cString Name,int id = 0);
	void ClearItems();
	bool IsFound(cString Name);
	
	virtual void SetSerialize(cXMLHash& XMLParams);
	virtual void GetSerialize(cXMLHash& XMLParams);
};

class cXMLHash : public cHash
{
public:
	void AddXML(cString Name, cString XMLItem);
	void AddText(cString Name, cString str);
	void AddBinary(cString Name, char* buff, DWORD length);
	cString GetXML(cString Name,int id = 0);
	cString GetText(cString Name,int id = 0);
	cString GetBinary(cString Name,DWORD &len,int id = 0);
	cString GetXML(int id);
	cString GetText(int id);
	cString GetBinary(int id,DWORD &len);
	cXMLHash() : cHash(){};
	~cXMLHash();
};


 class cList : public cSerializer
{
	
	char* head;
	DWORD nItems;
	int Ssize;

public:
	cList();
	cList(int size);
	~cList();
	void AddItem(char* item);
	DWORD GetNumberOfItems();
	char* GetItem(int index);
	char* GetLastItem();
	void SetSize(int size);
	int GetSize();
	char* operator[](int index);
	virtual void SetSerialize(cXMLHash& XMLParams);
	virtual void GetSerialize(cXMLHash& XMLParams);
};

//cList in Array .. easy to use
template <typename Type>
struct array {
	cList* data;
	array(cList* y)
	{
		data = y;
	}
	array(){};
	void init(size_t size)
	{
		data = new cList(size);
	}
	void setvalues(cList* values)
	{
		  data = values;
	}
	void additem(Type Item)
	{
		data->AddItem((char*)Item);
	}
	void clear()
	{
		delete data;
	}
	cString toxml()
	{
		return data->Serialize(true);
	}
};




class cEncryptedString
{

public:
	cString EncryptedString;
	cEncryptedString(){};
	cEncryptedString(char* buff,DWORD length){EncryptedString = Encrypt(buff,length);}
	virtual cString Encrypt(char* buff,DWORD length){return "";};
	cEncryptedString(cString str){EncryptedString = Encrypt((char*)str,str.GetLength());}
	~cEncryptedString(void){};
	cString GetEncrypted(){return EncryptedString;}
	operator char*(){return EncryptedString.GetChar();}
	void SetEncrypted(cString encryptedString){EncryptedString = encryptedString;}
	bool operator == (char* x){return (EncryptedString == x);}
};

class cMD5String : public cEncryptedString
{
	HCRYPTPROV	hProv;
    HCRYPTHASH  hHash;

public:
	cMD5String(){};
	cMD5String(char* buff,DWORD length) : cEncryptedString(buff,length){hProv = NULL;hHash = 0;};
	virtual cString Encrypt(char* buff,DWORD length);
	cMD5String(cString str) : cEncryptedString(str){};
	~cMD5String(void){};
};

class cEncodedString
{
protected:
	cString EncodedString;
public:
	cEncodedString(){};
	cEncodedString(cString str){EncodedString = Encode((char*)str,str.GetLength());}
	cEncodedString(char* buff,DWORD length){EncodedString = Encode(buff,length);}
	void SetEncoded(cString encodedString){EncodedString = encodedString;}
	virtual cString Encode(char* buff,DWORD length){cout << "Encoded Error\n\n\n";return "";};
	virtual char* Decode(DWORD &len){len = NULL;return NULL;}
	operator char*(){return EncodedString.GetChar();}
	cString GetEncoded(){return EncodedString;}
	bool operator == (char* x){return (EncodedString == x);}
};

class cBase64String : public cEncodedString
{
public:
	cBase64String(){};
	cBase64String(cString str){EncodedString = Encode((char*)str,str.GetLength());}
	cBase64String(char* buff,DWORD length){EncodedString = Encode(buff,length);}
	virtual cString Encode(char* buff,DWORD length);
	virtual char* Decode(DWORD &len);
	
};

class cXMLEncodedString : public cEncodedString
{
public:
	cXMLEncodedString(){};
	//cXMLEncodedString(char* buff,DWORD length) : cEncodedString(buff,length) {}
	cXMLEncodedString(cString str){EncodedString = Encode((char*)str,str.GetLength());}
	cXMLEncodedString(char* buff,DWORD length){EncodedString = Encode(buff,length);}
	virtual cString Encode(char* buff,DWORD length);
	virtual char* Decode(DWORD &len);
};
