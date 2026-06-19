#pragma once

#include <cstdint>
#include <format>
#include <string>
#include <string_view>
#include <source_location>

/**
 * Severity of a log message, ordered from most to least critical.
 *
 * The ordering mirrors Unreal's ELogVerbosity: a lower enumerator value means a
 * more severe message. Keeping this order makes future filtering trivial - a
 * category can store a "maximum active verbosity" and reject everything with
 * `MessageVerbosity > Threshold`.
 */
enum class ELogVerbosity : uint8_t
{
	/** Logs the message and then terminates the process via std::abort(). */
	Fatal,

	/** A serious problem that must never be ignored. Printed in red. */
	Error,

	/** A recoverable problem, or something worth the developer's attention. Printed in yellow. */
	Warning,

	/** Ordinary informational message. The default, lowest-severity level. */
	Log,
};

/**
 * Identifies a logging channel (a "category"), e.g. LogEngine or LogPhysics.
 *
 * Every log line is tagged with its category name, which makes the output easy
 * to scan for a single subsystem and easy to filter later on.
 */
struct FLogCategory
{
	/** Channel name printed inside square brackets on every line. */
	std::string_view Name;

	/**
	 * Active verbosity threshold for this channel. A message is printed only
	 * when it is at least this severe - i.e. MessageVerbosity <= Threshold,
	 * remembering that a lower enum value means a more severe message.
	 *
	 * Defaults to Log, which lets every level through. Because Fatal has the
	 * smallest value it always passes, no matter how strict the threshold is.
	 * The field is mutable so it can be retuned at runtime via SetLogVerbosity.
	 */
	ELogVerbosity Threshold = ELogVerbosity::Log;
};

/**
 * Declares a log category so it can be referenced from any translation unit.
 * Place this in a header (or near the top of the .cpp that owns the subsystem).
 *
 * The category is intentionally non-const: its Threshold field is meant to be
 * retuned at runtime (see SetLogVerbosity). Mirrors DECLARE_LOG_CATEGORY_EXTERN.
 */
#define DECLARE_LOG_CATEGORY_EXTERN(CategoryName) extern FLogCategory CategoryName

/**
 * Provides the storage for a category declared with DECLARE_LOG_CATEGORY_EXTERN.
 * Place this in exactly one .cpp file. The stringized name becomes Category.Name
 * and the channel starts with the Log threshold (everything is shown).
 *
 * Mirrors Unreal's DEFINE_LOG_CATEGORY.
 */
#define DEFINE_LOG_CATEGORY(CategoryName) \
	FLogCategory CategoryName             \
	{                                     \
		#CategoryName, ELogVerbosity::Log \
	}

/**
 * Like DEFINE_LOG_CATEGORY, but starts the channel at a custom threshold, e.g.
 * DEFINE_LOG_CATEGORY_VERBOSITY(LogPhysics, Warning) hides plain Log lines until
 * something raises the threshold again at runtime.
 */
#define DEFINE_LOG_CATEGORY_VERBOSITY(CategoryName, DefaultVerbosity) \
	FLogCategory CategoryName                                         \
	{                                                                 \
		#CategoryName, ELogVerbosity::DefaultVerbosity                \
	}

// Engine-wide log categories. When you add a subsystem, declare its category
// next to that subsystem instead of growing this central list forever.
DECLARE_LOG_CATEGORY_EXTERN(LogTemp);
DECLARE_LOG_CATEGORY_EXTERN(LogEngine);
DECLARE_LOG_CATEGORY_EXTERN(LogPhysics);
DECLARE_LOG_CATEGORY_EXTERN(LogRender);

namespace EngineCore
{
	/**
	 * Low-level sink for a single, fully-formatted log line. Prefer the UE_LOG
	 * macro over calling this directly: the macro expands the format string and
	 * forwards the category and verbosity for you.
	 *
	 * @param Category       Channel the message belongs to.
	 * @param Verbosity      Severity of the message.
	 * @param Location       Describes the code location where the logging event occurred, typically capturing file, line, and function information.
	 * @param FormattedText  Final message text, already expanded by std::format.
	 */
	void LogMessageImpl(const FLogCategory& Category, ELogVerbosity Verbosity, const std::source_location& Location, std::string_view FormattedText);

	/**
	 * Retunes a category's verbosity threshold at runtime. Afterwards the channel
	 * prints every message at least as severe as NewThreshold and silently drops
	 * the rest. Pass ELogVerbosity::Log to see everything, ELogVerbosity::Error
	 * to keep only errors and fatals, and so on.
	 *
	 * @param Category      The channel to reconfigure (the global category object).
	 * @param NewThreshold  Least-severe level that should still be printed.
	 */
	inline void SetLogVerbosity(FLogCategory& Category, ELogVerbosity NewThreshold)
	{
		Category.Threshold = NewThreshold;
	}

	/**
	 * Returns true when a debugger is currently attached to the process. The
	 * ensure* macros consult this so they break into the debugger only when one is
	 * present - an unconditional breakpoint would crash a normally-launched build.
	 * Only Windows is wired up; everywhere else this is always false (log, no break).
	 */
	bool IsDebuggerAttached();

	/**
	 * Backs the ensure* macros. Logs a single Error line through the normal sink
	 * (so it inherits the colour and the clickable file:// call-site prefix),
	 * composed as:  Ensure condition failed: <Expression> - <Message>
	 * The " - <Message>" tail is omitted when Message is empty.
	 *
	 * @param Location    Call site of the ensure, captured by the macro.
	 * @param Expression  Stringized failing condition, e.g. "Component != nullptr".
	 * @param Message     Optional pre-formatted detail, or an empty view.
	 */
	void EnsureFailedImpl(const std::source_location& Location, std::string_view Expression, std::string_view Message);
}  // namespace EngineCore

/**
 * Primary logging entry point, modelled on Unreal Engine's UE_LOG.
 *
 * Example:
 *     UE_LOG(LogEngine, Warning, "Player '{}' took {} damage", PlayerName, Amount);
 *
 * @param Category   A category declared via DECLARE_LOG_CATEGORY_EXTERN (unquoted).
 * @param Verbosity  One of the ELogVerbosity enumerators (unquoted, e.g. Warning).
 * @param Format     A std::format format string - use {} placeholders, not %d.
 * @param ...        Arguments substituted into the {} placeholders.
 */
// clang-format off
#define UE_LOG(Category, Verbosity, Format, ...) \
	EngineCore::LogMessageImpl( \
		Category, \
		ELogVerbosity::Verbosity, \
		std::source_location::current(), \
		std::format(Format, ##__VA_ARGS__))
// clang-format on

/**
 * Breaks into an attached debugger. Compiler intrinsic, no headers required.
 * The ensure* macros gate this behind IsDebuggerAttached() so it never fires in
 * a build that was launched without a debugger.
 */
#if defined(_MSC_VER)
	#define ENGINE_DEBUG_BREAK() (__debugbreak())
#elif defined(__clang__)
	#define ENGINE_DEBUG_BREAK() (__builtin_debugtrap())
#elif defined(__GNUC__)
	#define ENGINE_DEBUG_BREAK() (__builtin_trap())
#else
	#define ENGINE_DEBUG_BREAK() ((void)0)
#endif

/**
 * UE-style assertions that LOG AND CONTINUE, modelled on Unreal's ensure family.
 *
 * Unlike a fatal check/assert, a failed ensure reports the problem and lets
 * execution carry on, so it is safe to place on a hot path that runs every
 * frame: only the FIRST failure at each call site is logged. Use the *Always
 * variants when you want every failure logged instead.
 *
 * Each macro evaluates Expression exactly once and yields its bool result, so it
 * reads naturally as a guard:
 *     if (!ensure(Component != nullptr)) return;
 *     if (!ensureMsgf(Index < Count, "Index {} out of range {}", Index, Count)) return;
 *
 * On failure an Error line is logged (with the clickable call-site prefix) and,
 * if a debugger is attached, execution breaks right at the call site.
 *
 * Note: like UE's macros, a top-level comma in Expression confuses the
 * preprocessor (e.g. ensure(is_same_v<A, B>)) - wrap it in extra parens:
 * ensure((is_same_v<A, B>)).
 */
// clang-format off
#define UE_PRIVATE_ENSURE_IMPL(Expression, bLogEveryTime, Message)                  \
	([&](const std::source_location EnsureLoc) -> bool {                            \
		const bool bEnsureResult = static_cast<bool>(Expression);                   \
		if (!bEnsureResult) [[unlikely]]                                            \
		{                                                                           \
			static bool bEnsureLogged = false;                                      \
			if ((bLogEveryTime) || !bEnsureLogged)                                  \
			{                                                                       \
				bEnsureLogged = true;                                               \
				EngineCore::EnsureFailedImpl(EnsureLoc, #Expression, (Message));     \
				if (EngineCore::IsDebuggerAttached())                               \
				{                                                                   \
					ENGINE_DEBUG_BREAK();                                           \
				}                                                                   \
			}                                                                       \
		}                                                                           \
		return bEnsureResult;                                                       \
	}(std::source_location::current()))

#define ensure(Expression)                  UE_PRIVATE_ENSURE_IMPL(Expression, false, std::string_view{})
#define ensureMsgf(Expression, Format, ...) UE_PRIVATE_ENSURE_IMPL(Expression, false, std::format(Format, ##__VA_ARGS__))
#define ensureAlways(Expression)            UE_PRIVATE_ENSURE_IMPL(Expression, true,  std::string_view{})
#define ensureAlwaysMsgf(Expression, Format, ...) UE_PRIVATE_ENSURE_IMPL(Expression, true, std::format(Format, ##__VA_ARGS__))
// clang-format on
