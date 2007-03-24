using System;
using System.Collections.Generic;
using System.Text;
using System.Runtime.InteropServices;
using System.IO;

namespace EditIni
{
  class Program
  {
#region Data
    static string m_iniFile;
    static string m_section;
    static string m_key;
    static string m_value;
#endregion

    [DllImport("kernel32.dll", SetLastError = true)]
    static extern uint GetPrivateProfileString( string lpAppName,
                                                string lpKeyName,
                                                string lpDefault,
                                                StringBuilder lpReturnedString,
                                                uint nSize,
                                                string lpFileName );
    [DllImport("kernel32.dll", SetLastError = true)]
    static extern uint GetPrivateProfileString( string lpAppName,
                                                string lpKeyName,
                                                string lpDefault,
                                                [In, Out] char[] lpReturnedString,
                                                uint nSize,
                                                string lpFileName );

    [DllImport("kernel32.dll", SetLastError=true)]
    static extern bool WritePrivateProfileString( string lpAppName,
                                                  string lpKeyName,
                                                  string lpString,
                                                  string lpFileName );

    static void PrintUsage()
    {
      Console.WriteLine(
        @"EditIni.exe <Full path to file.ini> /sec <Section name> /key <Key name> /val <value>"
      );
    }

    static void Parse(string[] args)
    {
      for (int i = 0; i < args.Length; ++i)
      {
        if (args[i][0] != '-' && args[i][0] != '/')
        {
          m_iniFile = args[i];
        }
        else
        {
          if (args.Length <= i + 1)
          {
            throw new Exception(string.Format("The value is missed for key: {0}", args[i]));
          }
          string key = args[i].Substring(1).ToLower();
          ++i;
          switch (key)
          {
            case "sec":
              m_section = args[i];
              break;

            case "key":
              m_key = args[i];
              break;

            case "val":
              m_value = args[i];
              break;
          }
        }
      }

      if (m_iniFile == null || m_key == null || m_section == null || m_value == null)
      {
        throw new Exception("Not all nesessary keys are provided.");
      }
    }

    static void MakeIniFileWritable()
    {
      FileAttributes fa = File.GetAttributes(m_iniFile);
      fa &= ~FileAttributes.ReadOnly;
      File.SetAttributes(m_iniFile, fa);
    }

    static void ChangeKey()
    {
      if (!WritePrivateProfileString(m_section, m_key, m_value, m_iniFile))
      {
        throw new Exception(
          string.Format("The last Win32 Error was: {0}", Marshal.GetLastWin32Error()));
      }
    }

    static void CheckArguments()
    {
      //string fp = Path.GetFullPath(m_iniFile).ToLower();
      //string src = m_iniFile.ToLower();
      //if (fp != src)
      //{
      //  throw new Exception("The full path to ini-file should be provided.");
      //}
    }

    static int Main(string[] args)
    {
      try
      {
        if (args.Length < 7)
        {
          PrintUsage();
          return 1;
        }

        Parse(args);
        CheckArguments();
        MakeIniFileWritable();
        ChangeKey();
      }
      catch (Exception ex)
      {
        Console.WriteLine("\nError: ");
        Console.WriteLine(ex.Message);
        return 2;
      }

      return 0;
    }
  }
}
