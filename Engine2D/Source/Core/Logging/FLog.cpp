#include "FLog.h"

#include <algorithm>
#include <chrono>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <string>

#if defined(_WIN32)
	#define WIN32_LEAN_AND_MEAN
	#define NOMINMAX
	#include <Windows.h>  // IsDebuggerPresent
#endif

// Storage for the engine-wide categories declared in FLog.h. This must live in
// exactly one translation unit, otherwise the linker reports duplicate symbols.
DEFINE_LOG_CATEGORY(LogTemp);
DEFINE_LOG_CATEGORY(LogEngine);
DEFINE_LOG_CATEGORY(LogPhysics);
DEFINE_LOG_CATEGORY(LogRender);

namespace EngineCore
{
	namespace
	{
		// ANSI escape sequences used to colour console output by severity.
		// "\033[1;31m" = bold red, "\033[1;33m" = bold yellow, "\033[0m" = reset.
		constexpr std::string_view AnsiRed = "\033[1;31m";
		constexpr std::string_view AnsiYellow = "\033[1;33m";
		constexpr std::string_view AnsiReset = "\033[0m";

		/** Returns the human-readable label printed for a verbosity level. */
		constexpr std::string_view VerbosityToString(ELogVerbosity Verbosity)
		{
			switch (Verbosity)
			{
				case ELogVerbosity::Fatal:
					return "Fatal";
				case ELogVerbosity::Error:
					return "Error";
				case ELogVerbosity::Warning:
					return "Warning";
				case ELogVerbosity::Log:
					return "Log";
			}
			return "Unknown";
		}

		/** Returns the ANSI colour prefix for a verbosity, or an empty view for none. */
		constexpr std::string_view VerbosityToColor(ELogVerbosity Verbosity)
		{
			switch (Verbosity)
			{
				case ELogVerbosity::Fatal:
				case ELogVerbosity::Error:
					return AnsiRed;
				case ELogVerbosity::Warning:
					return AnsiYellow;
				default:
					return {};
			}
		}

		/**
		 * Trims a compiler-specific function signature down to a readable
		 * `Class::Method` form. std::source_location::function_name() returns the
		 * full signature, which differs per compiler and is noisy - on MSVC it is
		 * "void __cdecl USceneComponent::AttachToComponent(class USceneComponent *)".
		 *
		 * We drop the parameter list (everything from the first '(') and the return
		 * type / calling convention before it (everything up to the last space),
		 * leaving "USceneComponent::AttachToComponent". If the expected punctuation
		 * is missing we fall back to the raw signature rather than risk an empty view.
		 */
		constexpr std::string_view ExtractFunctionName(std::string_view Signature)
		{
			// Cut the parameter list: keep everything up to the first '('.
			if (const size_t Paren = Signature.find('('); Paren != std::string_view::npos)
			{
				Signature = Signature.substr(0, Paren);
			}

			// Cut the return type / calling convention: keep everything after the
			// last space (e.g. "void __cdecl Foo::Bar" -> "Foo::Bar").
			if (const size_t Space = Signature.rfind(' '); Space != std::string_view::npos)
			{
				Signature = Signature.substr(Space + 1);
			}

			return Signature;
		}
	}  // namespace

	std::string FormatLogMessage(const char* Format, ...)
	{
		// First pass: measure the exact length the formatted text needs.
		va_list Args;
		va_start(Args, Format);

		va_list ArgsCopy;
		va_copy(ArgsCopy, Args);

		const int Size = std::vsnprintf(nullptr, 0, Format, Args);
		va_end(Args);

		// A negative result signals an encoding error; fall back to the raw format.
		if (Size < 0)
		{
			va_end(ArgsCopy);
			return std::string(Format);
		}

		// Second pass: render into a buffer sized to hold exactly Size characters
		// (vsnprintf also writes the trailing null into the string's reserved slot).
		std::string Result(static_cast<size_t>(Size), '\0');
		if (Size > 0)
		{
			std::vsnprintf(Result.data(), static_cast<size_t>(Size) + 1, Format, ArgsCopy);
		}
		va_end(ArgsCopy);

		return Result;
	}

	void LogMessageImpl(const FLogCategory& Category, ELogVerbosity Verbosity, const std::source_location& Location, std::string_view FormattedText)
	{
		// 0. Verbosity filter. Lower enum value = more severe, so a message is
		//    kept while Verbosity <= Threshold and dropped otherwise. Returning
		//    here skips all timestamp/formatting work for suppressed lines.
		//    Fatal (value 0) is never filtered, regardless of the threshold.
		if (Verbosity > Category.Threshold)
		{
			return;
		}

		// 1. Capture the current wall-clock time for the line's timestamp.
		const auto Now = std::chrono::system_clock::now();
		const auto TimeT = std::chrono::system_clock::to_time_t(Now);

		// 2. Break the timestamp into local calendar fields using the
		//    platform-safe variant (localtime itself is not thread-safe).
		tm Parts{};
#if defined(_WIN32)
		localtime_s(&Parts, &TimeT);
#else
		localtime_r(&TimeT, &Parts);
#endif

		// 3. Tint the whole line according to severity (empty for plain Log).
		const std::string_view Color = VerbosityToColor(Verbosity);

		// 4. Build the call-site prefix. Only Warning and more-severe lines carry
		//    it: those are the ones a developer wants to trace, while ordinary Log
		//    spam stays clean. Lower enum value = more severe, hence <= Warning.
		//
		//    Format is  file:///path:line:column [Function]. Rider turns the file://
		//    URL into a link in the console; clicking it hands the URL to the OS,
		//    which opens .cpp with its default Windows app. With that association set
		//    to JetBrains Rider, the file opens at the given line:column - this is how
		//    the click jumps to source from the Debug console. We normalise separators
		//    to '/' (Windows __FILE__ uses backslashes) and make sure the path starts
		//    with a slash so the scheme ends up with three: POSIX paths already begin
		//    with '/', Windows drive paths (D:/...) need one prepended (file:///D:/...).
		//    File, line and column stay separate std::format arguments - file_name()
		//    is a const char* and line() a number, so they cannot be added together.
		std::string LocationPrefix;
		if (Verbosity <= ELogVerbosity::Warning)
		{
			std::string FilePath = Location.file_name();
			std::replace(FilePath.begin(), FilePath.end(), '\\', '/');

			// Prepend the file:// scheme. POSIX paths already start with '/', so they
			// become file:///abs/path; Windows drive paths (D:/...) get an extra slash
			// so they become file:///D:/... (three slashes after the scheme).
			std::string FileUrl = "file://";
			if (FilePath.empty() || FilePath.front() != '/')
			{
				FileUrl += '/';
			}
			FileUrl += FilePath;

			LocationPrefix = std::format("{}:{}:{} [{}] ",								 //
										 FileUrl, Location.line(), Location.column(),	 //
										 ExtractFunctionName(Location.function_name()));
		}

		// 5. Compose the final line:
		//    [YYYY.MM.DD-HH:MM:SS][Category] Verbosity: file:///path:line:column [Function] Message
		const std::string Line = std::format("[{:04}.{:02}.{:02}-{:02}:{:02}:{:02}][{}] {}: {}{}\n",	 //
											 Parts.tm_year + 1900, Parts.tm_mon + 1, Parts.tm_mday,		 //
											 Parts.tm_hour, Parts.tm_min, Parts.tm_sec,				 //
											 Category.Name, VerbosityToString(Verbosity), LocationPrefix, FormattedText);

		// 6. Console output, tinted by severity. Flush each line: when an IDE captures
		//    stdout through a pipe (not a real terminal) the stream is fully buffered,
		//    so without this the log would only appear when the program exits instead
		//    of in real time.
		std::cout << Color << Line;
		if (!Color.empty())
		{
			std::cout << AnsiReset;
		}
		std::cout.flush();

		// 7. A Fatal message is unrecoverable: the stream is already flushed above,
		//    so crash immediately to surface the problem.
		if (Verbosity == ELogVerbosity::Fatal)
		{
			std::abort();
		}
	}

	bool IsDebuggerAttached()
	{
#if defined(_WIN32)
		return ::IsDebuggerPresent() != 0;
#else
		return false;
#endif
	}

	void EnsureFailedImpl(const std::source_location& Location, std::string_view Expression, std::string_view Message)
	{
		// Compose "Ensure condition failed: <expr>" plus an optional " - <detail>",
		// then route it through the normal sink at Error so it inherits the colour
		// and the clickable file:// call-site prefix. The expression is appended as
		// plain text (not via a format string) so braces inside it - e.g.
		// ensure(Foo{}) - are never mistaken for std::format placeholders.
		std::string Text = "Ensure condition failed: ";
		Text += Expression;
		if (!Message.empty())
		{
			Text += " - ";
			Text += Message;
		}

		LogMessageImpl(LogEngine, ELogVerbosity::Error, Location, Text);
	}
}  // namespace EngineCore
