// WTL CString 使用指南
// 这个文件展示了如何在WTL中使用CString

#ifndef __WTL_CSTRING_GUIDE_H__
#define __WTL_CSTRING_GUIDE_H__

#pragma once

#include <atlbase.h>
#include <atlstr.h>  // WTL CString的主要头文件
#include <atlmisc.h> // 包含其他WTL工具类

namespace WTL {

// CString基本使用示例
class CStringGuide
{
public:
    // 1. CString的构造和初始化
    static void BasicConstruction()
    {
        // 默认构造
        CString str1;
        
        // 从字符串字面量构造
        CString str2 = _T("Hello WTL");
        CString str3(_T("Hello World"));
        
        // 从另一个CString构造
        CString str4 = str2;
        
        // 从字符数组构造
        TCHAR szText[] = _T("Test String");
        CString str5(szText);
        
        // 格式化构造
        CString str6;
        str6.Format(_T("Count: %d, Value: %s"), 42, _T("test"));
    }
    
    // 2. 基本字符串操作
    static void BasicOperations()
    {
        CString str = _T("Hello World");
        
        // 获取长度
        int nLength = str.GetLength();  // 返回11
        
        // 判断是否为空
        BOOL bEmpty = str.IsEmpty();   // 返回FALSE
        
        // 清空字符串
        str.Empty();
        
        // 赋值操作
        str = _T("New String");
        
        // 连接字符串
        str += _T(" Append");
        CString result = str + _T(" More");
    }
    
    // 3. 字符串比较
    static void StringComparison()
    {
        CString str1 = _T("Hello");
        CString str2 = _T("hello");
        CString str3 = _T("Hello");
        
        // 区分大小写的比较
        int nCompare1 = str1.Compare(str2);  // 返回非0值
        int nCompare2 = str1.Compare(str3);  // 返回0
        
        // 不区分大小写的比较
        int nCompare3 = str1.CompareNoCase(str2);  // 返回0
        
        // 比较操作符
        BOOL bEqual1 = (str1 == str3);  // TRUE
        BOOL bEqual2 = (str1 != str2);  // TRUE
    }
    
    // 4. 子串和查找操作
    static void SubstringAndSearch()
    {
        CString str = _T("Hello World Test");
        
        // 获取子串
        CString sub1 = str.Left(5);      // "Hello"
        CString sub2 = str.Right(4);     // "Test"
        CString sub3 = str.Mid(6, 5);   // "World"
        
        // 查找子串
        int nPos1 = str.Find(_T("World"));     // 返回6
        int nPos2 = str.Find(_T("Not"));      // 返回-1
        int nPos3 = str.Find(_T("l"), 2);     // 从位置2开始查找，返回3
        
        // 反向查找
        int nPos4 = str.ReverseFind(_T("o"));  // 返回7
        
        // 查找字符
        int nPos5 = str.FindOneOf(_T("aeiou")); // 返回1 (第一个元音'e')
    }
    
    // 5. 大小写转换
    static void CaseConversion()
    {
        CString str = _T("Hello World");
        
        // 转换为大写
        CString upper = str;
        upper.MakeUpper();  // "HELLO WORLD"
        
        // 转换为小写
        CString lower = str;
        lower.MakeLower();  // "hello world"
        
        // 反转大小写
        CString reverse = str;
        reverse.MakeReverse();  // "hELLO wORLD"
    }
    
    // 6. 格式化字符串
    static void StringFormatting()
    {
        CString str;
        
        // 基本格式化
        str.Format(_T("Integer: %d"), 42);
        
        // 多个参数
        str.Format(_T("Name: %s, Age: %d, Salary: %.2f"), 
                   _T("John"), 30, 2500.50);
        
        // 宽字符格式化
        str.Format(_T("Unicode: %s"), L"Unicode String");
        
        // 格式化缓冲区
        TCHAR buffer[256];
        _stprintf_s(buffer, _T("Buffer: %s"), _T("test"));
        str = buffer;
    }
    
    // 7. 修剪操作
    static void TrimOperations()
    {
        CString str = _T("   Hello World   ");
        
        // 移除两端空格
        CString trimmed = str;
        trimmed.Trim();  // "Hello World"
        
        // 移除左侧空格
        CString leftTrim = str;
        leftTrim.TrimLeft();  // "Hello World   "
        
        // 移除右侧空格
        CString rightTrim = str;
        rightTrim.TrimRight();  // "   Hello World"
        
        // 移除特定字符
        CString custom = _T("xxHelloxx");
        custom.TrimLeft(_T('x'));  // "Helloxx"
    }
    
    // 8. 替换操作
    static void ReplaceOperations()
    {
        CString str = _T("Hello World Hello");
        
        // 替换所有匹配项
        int nReplaced = str.Replace(_T("Hello"), _T("Hi"));  // 返回2
        // str现在是 "Hi World Hi"
        
        // 替换单个字符
        str.Replace(_T(' '), _T('_'));  // "Hi_World_Hi"
    }
    
    // 9. 转换为其他类型
    static void TypeConversion()
    {
        CString str;
        
        // 从整数转换
        str.Format(_T("%d"), 123);
        
        // 转换为整数
        int nValue = _ttoi(str);  // 123
        
        // 转换为长整数
        long lValue = _ttol(str);  // 123
        
        // 转换为浮点数
        str.Format(_T("%.2f"), 3.14);
        double dValue = _ttof(str);  // 3.14
        
        // 转换为64位整数
        str.Format(_T("%I64d"), 123456789012345);
        __int64 i64Value = _ttoi64(str);  // 123456789012345
    }
    
    // 10. 与标准字符串转换
    static void StdStringConversion()
    {
        // CString转std::string (ANSI)
        CString str = _T("Hello");
        std::string stdStr = CT2A(str.GetString());
        
        // CString转std::wstring (Unicode)
        std::wstring stdWStr = str.GetString();
        
        // std::string转CString
        std::string src = "Hello";
        CString dest = CA2T(src.c_str());
        
        // std::wstring转CString
        std::wstring wsrc = L"Hello";
        CString wdest = wsrc.c_str();
    }
    
    // 11. 文件路径操作
    static void FilePathOperations()
    {
        CString path = _T("C:\\Folder\\SubFolder\\file.txt");
        
        // 获取文件名
        int nPos = path.ReverseFind(_T('\\'));
        CString fileName = path.Mid(nPos + 1);  // "file.txt"
        
        // 获取目录路径
        CString dirPath = path.Left(nPos);  // "C:\\Folder\\SubFolder"
        
        // 获取扩展名
        nPos = fileName.ReverseFind(_T('.'));
        CString ext = fileName.Mid(nPos + 1);  // "txt"
        
        // 获取不带扩展名的文件名
        CString nameOnly = fileName.Left(nPos);  // "file"
    }
    
    // 12. 实用示例：解析命令行参数
    static void ParseCommandLineExample()
    {
        LPTSTR lpCmdLine = GetCommandLine();
        CString cmdLine = lpCmdLine;
        
        // 分割命令行参数
        CStringArray args;
        int nPos = 0;
        CString token;
        
        while ((nPos = cmdLine.Find(_T(' '))) != -1)
        {
            token = cmdLine.Left(nPos);
            token.Trim(_T('"'));  // 移除引号
            if (!token.IsEmpty())
            {
                args.Add(token);
            }
            cmdLine = cmdLine.Mid(nPos + 1);
        }
        
        // 添加最后一个参数
        cmdLine.Trim(_T('"'));
        if (!cmdLine.IsEmpty())
        {
            args.Add(cmdLine);
        }
    }
    
    // 13. 实用示例：构建URL
    static CString BuildUrlExample()
    {
        CString baseUrl = _T("https://api.example.com");
        CString endpoint = _T("/users");
        CString param1 = _T("id=123");
        CString param2 = _T("format=json");
        
        CString url;
        url.Format(_T("%s%s?%s&%s"), 
                  baseUrl, endpoint, param1, param2);
        
        return url;  // "https://api.example.com/users?id=123&format=json"
    }
    
    // 14. 实用示例：日志格式化
    static CString FormatLogMessage(LPCTSTR lpszLevel, LPCTSTR lpszMessage, int nLine)
    {
        CTime time = CTime::GetCurrentTime();
        CString strTime = time.Format(_T("%Y-%m-%d %H:%M:%S"));
        
        CString logEntry;
        logEntry.Format(_T("[%s] [%s] [%d] %s"), 
                       strTime, lpszLevel, nLine, lpszMessage);
        
        return logEntry;
    }
};

} // namespace WTL

#endif // __WTL_CSTRING_GUIDE_H__