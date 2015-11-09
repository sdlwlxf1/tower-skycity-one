#ifndef __STRING_HANDLE_H__
#define __STRING_HANDLE_H__

#include "libiconv/include/iconv.h"

typedef std::vector<std::string> strArray;
#include "cocos2d.h"
USING_NS_CC;

static inline void split(const std::string& src, const std::string& token, strArray& vect)
{
	size_t nend = 0;
	size_t nbegin = 0;
	size_t tokenSize = token.size();
	while(nend != std::string::npos)
	{
		nend = src.find(token, nbegin);
		if(nend == std::string::npos)
			vect.push_back(src.substr(nbegin, src.length()-nbegin));
		else
			vect.push_back(src.substr(nbegin, nend-nbegin));
		nbegin = nend + tokenSize;
	}
}

// first, judge whether the form of the string like this: {x:y}
// if the form is right,the string will be split into the parameter strs;
// or the parameter strs will be empty.
// if the form is right return true,else return false.
static inline cocos2d::ValueMap& stringToValueMap(const std::string& content, cocos2d::ValueMap &map)
{
	strArray strs;
	split(content, "|", strs);
	for(auto str:strs)
	{
		auto leftPos = str.find(":");
		auto left = str.substr(0, leftPos);
		auto right = str.substr(leftPos+1, str.length()-1);
		map[left] = right;
	}
	return map;
}

static inline std::string valueMapToString(cocos2d::ValueMap &map)
{
	std::string rep;
	for(auto pair:map)
	{
		rep += pair.first + ":" + pair.second.asString() + "|";
	}
	rep.pop_back();
	return rep;
}

// string toolkit
static inline cocos2d::ValueVector& stringToValueVector(const std::string& src, const std::string& token, cocos2d::ValueVector& vect)
{
	strArray out;
	split(src,token,out);
	for(auto str:out)
	{
		vect.push_back(cocos2d::Value(str));
	}
	return vect;
}

static inline std::string pointToString(Vec2 point)
{
	return StringUtils::format("{%f,%f}",point.x,point.y);
}

static inline void stringReplace(std::string &strBase, std::string strSrc, std::string strDes)  
{  
	std::string::size_type pos = 0;  
	std::string::size_type srcLen = strSrc.size();  
	std::string::size_type desLen = strDes.size();  
	pos = strBase.find(strSrc, pos);   
	while ((pos != std::string::npos))  
	{  
		strBase.replace(pos, srcLen, strDes);  
		pos=strBase.find(strSrc, (pos+desLen));  
	}  
}  

static inline Color3B stringToColor3B(const std::string& content)
{
	Color3B color(255,255,255);
	do 
	{
		strArray strs;
		CC_BREAK_IF(content.empty());

		size_t nPosLeft  = content.find('{');
		size_t nPosRight = content.find('}');

		// don't have '{' and '}'
		CC_BREAK_IF(nPosLeft == std::string::npos || nPosRight == std::string::npos);
		// '}' is before '{'
		CC_BREAK_IF(nPosLeft > nPosRight);

		const std::string pointStr = content.substr(nPosLeft + 1, nPosRight - nPosLeft - 1);
		// nothing between '{' and '}'
		CC_BREAK_IF(pointStr.length() == 0);

		size_t nPos1 = pointStr.find('{');
		size_t nPos2 = pointStr.find('}');
		// contain '{' or '}' 
		CC_BREAK_IF(nPos1 != std::string::npos || nPos2 != std::string::npos);

		split(pointStr, ",", strs);
		if (strs.size() != 3 || strs[0].length() == 0 || strs[1].length() == 0 || strs[2].length() == 0)
		{
			strs.clear();
			break;
		}
		color = Color3B(cocos2d::Value(strs[0]).asInt(),cocos2d::Value(strs[1]).asInt(),cocos2d::Value(strs[2]).asInt());
	} while (0);

	return color;
}

static inline Color4B stringToColor4B(const std::string& content)
{
	Color4B color(255,255,255,255);
	do 
	{
		strArray strs;
		CC_BREAK_IF(content.empty());

		size_t nPosLeft  = content.find('{');
		size_t nPosRight = content.find('}');

		// don't have '{' and '}'
		CC_BREAK_IF(nPosLeft == std::string::npos || nPosRight == std::string::npos);
		// '}' is before '{'
		CC_BREAK_IF(nPosLeft > nPosRight);

		const std::string pointStr = content.substr(nPosLeft + 1, nPosRight - nPosLeft - 1);
		// nothing between '{' and '}'
		CC_BREAK_IF(pointStr.length() == 0);

		size_t nPos1 = pointStr.find('{');
		size_t nPos2 = pointStr.find('}');
		// contain '{' or '}' 
		CC_BREAK_IF(nPos1 != std::string::npos || nPos2 != std::string::npos);

		split(pointStr, ",", strs);
		if (strs.size() != 4 || strs[0].length() == 0 || strs[1].length() == 0 || strs[2].length() == 0)
		{
			strs.clear();
			break;
		}
		color = Color4B(cocos2d::Value(strs[0]).asInt(),cocos2d::Value(strs[1]).asInt(),cocos2d::Value(strs[2]).asInt(),cocos2d::Value(strs[3]).asInt());
	} while (0);

	return color;
}

static inline std::string stringLineBreakFormat(std::string str,int length)  
{  
	unsigned int beginPos = 0;  //字符串的初始位置  
	std::string resultStr;      //返回的字符串  
	std::vector<std::string > str_vec;  //创建一个字符串类型的顺序容器  
	do     
	{    
		str_vec.push_back(str.substr(beginPos,length)); //substr函数的作用类似剪刀，将str中从beginPos到length之间的字符串剪下来，单独放入容器中   
		if (beginPos+length >str.size())    
		{    
			break;  //当要裁剪的长度超出str的长度，则退出循环  
		}    
		else    
		{    
			beginPos += length;   
		}    

	} while (true);    
	for (unsigned int i = 0;i<str_vec.size();++i)    
	{    
		resultStr.append(str_vec.at(i)).append("\n"); //从容器逐一取出之前裁剪好的一段段字符串，分别在字符串后面加上换行符。append()类似胶水，将\n粘到字符串后面   
	}    
	//resultStr.pop_back();  //这一句是将最后一个多余的\n给删掉  
	return resultStr;    
} 

//static inline std::string AnsiToUtf8(std::string strAnsi)
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//	return strAnsi;
//#else
//	std::string ret;
//	if (strAnsi.length() > 0)
//	{    
//		int nWideStrLength = MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, NULL, 0);
//		WCHAR* pwszBuf = (WCHAR*)malloc((nWideStrLength+1)*sizeof(WCHAR));
//		memset(pwszBuf, 0, (nWideStrLength+1)*sizeof(WCHAR));
//		MultiByteToWideChar(CP_ACP, 0, strAnsi.c_str(), -1, pwszBuf, (nWideStrLength+1)*sizeof(WCHAR));
//
//		int nUtf8Length = WideCharToMultiByte( CP_UTF8,0,pwszBuf,-1,NULL,0,NULL,FALSE );
//		char* pszUtf8Buf = (char*)malloc((nUtf8Length+1)*sizeof(char));
//		memset(pszUtf8Buf, 0, (nUtf8Length+1)*sizeof(char));
//
//		WideCharToMultiByte(CP_UTF8, 0, pwszBuf, -1, pszUtf8Buf, (nUtf8Length+1)*sizeof(char), NULL, FALSE);
//		ret = pszUtf8Buf;
//
//		free(pszUtf8Buf);
//		free(pwszBuf);
//	}
//	return ret;
//#endif
//}
//
//
//
//static inline std::string Utf8ToAnsi(std::string strUTF8)
//{
//#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID || CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
//	return strUTF8;
//#else
//	std::string ret;
//	if (strUTF8.length() > 0)
//	{
//		int nWideStrLength = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
//		WCHAR* pwszBuf = (WCHAR*)malloc((nWideStrLength+1)*sizeof(WCHAR));
//		memset(pwszBuf, 0, (nWideStrLength+1)*sizeof(WCHAR));
//		MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, pwszBuf, (nWideStrLength+1)*sizeof(WCHAR));
//
//		int nAnsiStrLength = WideCharToMultiByte( CP_ACP,0,pwszBuf,-1,NULL,0,NULL,FALSE );
//		char* pszAnsiBuf = (char*)malloc((nAnsiStrLength+1)*sizeof(char));
//		memset(pszAnsiBuf, 0, (nAnsiStrLength+1)*sizeof(char));
//
//		WideCharToMultiByte(CP_ACP, 0, pwszBuf, -1, pszAnsiBuf, (nAnsiStrLength+1)*sizeof(char), NULL, FALSE);
//		ret = pszAnsiBuf;
//
//		free(pszAnsiBuf);
//		free(pwszBuf);
//	}
//
//	return ret;
//#endif
//}

static inline int code_convert( char *from_charset, char *to_charset, char *inbuf, size_t inlen, char *outbuf, size_t outlen )  
{  
	iconv_t cd;  
	char *temp = inbuf;  
	char **pin = &temp;  
	char **pout = &outbuf;  
	memset(outbuf,0,outlen);  
	cd = iconv_open(to_charset,from_charset);  
	if(cd==0) return -1;  
	if(iconv(cd,pin,&inlen,pout,&outlen)==-1) return -1;  
	iconv_close(cd);  
	return 0;  
}  

static inline std::string u2a( const char *_inbuf )  
{  
	char *inbuf = new char[strlen(_inbuf)+1];  
	strcpy(inbuf, _inbuf);  

	size_t inlen = strlen(inbuf);  
	char * outbuf = new char[inlen * 2 + 2];  
	std::string strRet;  
	if(code_convert("utf-8", "gb2312", inbuf, inlen, outbuf, inlen * 2 + 2) == 0)  
	{  
		strRet = outbuf;  
	}  

	delete [] inbuf;  
	delete [] outbuf;  
	return strRet;  
}  

static inline std::string a2u( const char *_inbuf )  
{  
	char *inbuf = new char[strlen(_inbuf)+1];  
	strcpy(inbuf, _inbuf);  

	size_t inlen = strlen(inbuf);  
	char * outbuf = new char[inlen * 2 + 2];  
	std::string strRet;  
	if(code_convert("gb2312", "utf-8", inbuf, inlen, outbuf, inlen * 2 + 2) == 0)  
	{  
		strRet = outbuf;  
	}  

	delete [] outbuf;  
	delete [] inbuf;  
	return strRet;  
} 

static inline float getRand(float range)
{
	return ((double)rand()/RAND_MAX)*(range);
}

//// string toolkit
//static inline cocos2d::ValueMap& stringToValueMap(const std::string& src, cocos2d::ValueMap& map)
//{
//	strArray out;
//	splitWithForm(str,out);
//	split(src,",",out);
//	for(auto str:out)
//	{
//		strArray aout;
//		
//	}
//	return map;
//}

#endif /*__STRING_HANDLE_H__*/
