const string TRACKING_MOD_LOG_FOLDER = TRACKING_MOD_ROOT_FOLDER + "Logging\\";
const string TRACKING_MOD_LOGGER_LOG_DIR = TRACKING_MOD_LOG_FOLDER + "Logs\\";
const string TRACKING_MOD_LOGGER_LOG_FILE = TRACKING_MOD_LOGGER_LOG_DIR + "TrackingMod_%1.log";
const string TRACKING_MOD_LOGGER_CONFIG_FILE = TRACKING_MOD_ROOT_FOLDER + "LoggerConfig.json";

class TrackingModLoggerConfigJson
{
	int Level;
	void TrackingModLoggerConfigJson()
	{
		Level = 4;
	}
}

class TrackingMod
{
	private static FileHandle m_LogFile;
	private static string m_LogFilePath;
	static int m_LogLevel = 4;
	
	static int GetLogLevel()
	{
		return m_LogLevel;
	}
	
	static void LoadLoggerConfig()
	{
		TrackingModLoggerConfigJson config;
		
		config = new TrackingModLoggerConfigJson();
		if (FileExist(TRACKING_MOD_LOGGER_CONFIG_FILE))
		{
			JsonFileLoader<TrackingModLoggerConfigJson>.JsonLoadFile(TRACKING_MOD_LOGGER_CONFIG_FILE, config);
			if (config && config.Level >= 0 && config.Level <= 6)
			{
				m_LogLevel = config.Level;
				Print("[TrackingMod] Log level loaded from config: " + m_LogLevel);
			}
		}
		else
		{
			CheckDirectories();
			JsonFileLoader<TrackingModLoggerConfigJson>.JsonSaveFile(TRACKING_MOD_LOGGER_CONFIG_FILE, config);
			Print("[TrackingMod] Created default logger config file: " + TRACKING_MOD_LOGGER_CONFIG_FILE);
		}
	}
	
	static void InitLogFile()
	{
		LoadLoggerConfig();
		if (m_LogLevel == 0)
			return;
		CheckDirectories();
		m_LogFilePath = TRACKING_MOD_LOGGER_LOG_FILE;
		m_LogFilePath.Replace("%1", GetDateTime());
		m_LogFile = OpenFile(m_LogFilePath, FileMode.APPEND);
		if (m_LogFile)
		{
			Print("[TrackingMod] Log file initialized: " + m_LogFilePath);
			FPrintln(m_LogFile, "[" + GetTime() + "] [Init] Log file created for this session.");
			CloseFile(m_LogFile);
			m_LogFile = null;
		}
		else
		{
			Print("[TrackingMod ERROR] Failed to initialize log file: " + m_LogFilePath);
		}
	}
	
	static void CheckDirectories()
	{
		bool rootCreated;
		bool logFolderCreated;
		bool logsCreated;
		
		if (!FileExist(TRACKING_MOD_ROOT_FOLDER))
		{
			rootCreated = MakeDirectory(TRACKING_MOD_ROOT_FOLDER);
			if (rootCreated)
			{
				LogDebug("Created root directory: " + TRACKING_MOD_ROOT_FOLDER);
			}
			else
			{
				LogCritical("Failed to create root directory: " + TRACKING_MOD_ROOT_FOLDER);
			}
		}
		if (!FileExist(TRACKING_MOD_LOG_FOLDER))
		{
			logFolderCreated = MakeDirectory(TRACKING_MOD_LOG_FOLDER);
			if (logFolderCreated)
			{
				LogDebug("Created logging folder: " + TRACKING_MOD_LOG_FOLDER);
			}
			else
			{
				LogCritical("Failed to create logging folder: " + TRACKING_MOD_LOG_FOLDER);
			}
		}
		if (!FileExist(TRACKING_MOD_LOGGER_LOG_DIR))
		{
			logsCreated = MakeDirectory(TRACKING_MOD_LOGGER_LOG_DIR);
			if (logsCreated)
			{
				LogDebug("Created logs directory: " + TRACKING_MOD_LOGGER_LOG_DIR);
			}
			else
			{
				LogCritical("Failed to create logs directory: " + TRACKING_MOD_LOGGER_LOG_DIR);
			}
		}
	}
	
	static void SetLogLevel(int logLevel)
	{
		if (m_LogLevel == logLevel)
			return;
		m_LogLevel = logLevel;
		Print("[TrackingMod] Log level set to: " + m_LogLevel);
	}
	
	static void Log(int level, string message)
	{
		bool shouldLog;
		string logMessage;
		FileHandle tempLogFile;
		
		if (m_LogLevel == 0)
			return;
		shouldLog = false;
		if (m_LogLevel == 6)
		{
			shouldLog = true;
		}
		else if (m_LogLevel == 5 && level == 5)
		{
			shouldLog = true;
		}
		else if (level <= m_LogLevel)
		{
			shouldLog = true;
		}
		if (!shouldLog)
			return;
		logMessage = "[" + GetTime() + "] [Level " + level.ToString() + "] " + message;
		if (m_LogLevel == 0)
			return;
		CheckDirectories();
		if (m_LogFilePath == "")
		{
			m_LogFilePath = TRACKING_MOD_LOGGER_LOG_FILE;
			m_LogFilePath.Replace("%1", GetDateTime());
		}
		tempLogFile = OpenFile(m_LogFilePath, FileMode.APPEND);
		if (tempLogFile)
		{
			FPrintln(tempLogFile, logMessage);
			CloseFile(tempLogFile);
		}
		else
		{
			Print("[TrackingMod ERROR] Could not open log file: " + m_LogFilePath);
		}
	}
	
	static void LogCritical(string message)
	{
		Log(1, "[Critical] " + message);
	}
	
	static void LogWarning(string message)
	{
		Log(2, "[Warning] " + message);
	}
	
	static void LogInfo(string message)
	{
		Log(3, "[Info ] " + message);
	}
	
	static void LogDebug(string message)
	{
		Log(4, "[Debug] " + message);
	}
	
	static void LogRPC(string message)
	{
		Log(4, "[RPC] " + message);
	}
	
	static void LogEverything(string message)
	{
		Log(6, "[Everything] " + message);
	}
	
	static void CloseLogFile()
	{
		if (m_LogFile)
		{
			CloseFile(m_LogFile);
			m_LogFile = null;
			Print("[TrackingMod] Log file closed.");
		}
	}
	
	private static string GetDateTime()
	{
		int year;
		int month;
		int day;
		int hour;
		int minute;
		int second;
		GetYearMonthDay(year, month, day);
		GetHourMinuteSecond(hour, minute, second);
		string dateTime;
		dateTime = year.ToString() + "-";
		if (month < 10) dateTime += "0";
		dateTime += month.ToString() + "-";
		if (day < 10) dateTime += "0";
		dateTime += day.ToString() + "_";
		if (hour < 10) dateTime += "0";
		dateTime += hour.ToString() + "-";
		if (minute < 10) dateTime += "0";
		dateTime += minute.ToString() + "-";
		if (second < 10) dateTime += "0";
		dateTime += second.ToString();
		return dateTime;
	}
	
	private static string GetTime()
	{
		int hour;
		int minute;
		int second;
		string time;
		
		GetHourMinuteSecond(hour, minute, second);
		time = "";
		if (hour < 10) time += "0";
		time += hour.ToString() + ":";
		if (minute < 10) time += "0";
		time += minute.ToString() + ":";
		if (second < 10) time += "0";
		time += second.ToString();
		return time;
	}
}
