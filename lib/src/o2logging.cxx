#include <FairMQLogger.h>

/**
 * Custom FairMQLogger sink to add additional data to the output.
 * @param content The message to be displayed in the logs.
 * @param metadata Additional information such as the line, function and file the log was called from.
 * @deprecated don't use this it was never implemented.
 */
Logger::AddCustomSink("O2", "info", [](const string& content, const LogMetaData& metadata)
{
    cout << "content: " << content << endl;

    cout << "available metadata: " << endl;
    cout << "std::time_t timestamp: " << metadata.timestamp << endl;
    cout << "std::chrono::microseconds us: " << metadata.us.count() << endl;
    cout << "std::string process_name: " << metadata.process_name << endl;
    cout << "std::string file: " << metadata.file << endl;
    cout << "std::string line: " << metadata.line << endl;
    cout << "std::string func: " << metadata.func << endl;
    cout << "std::string severity_name: " << metadata.severity_name << endl;
    cout << "fair::Severity severity: " << static_cast<int>(metadata.severity) << endl;
});

fair::Logger::SetCustomSeverity("O2", "info");