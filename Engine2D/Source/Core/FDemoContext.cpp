#include "FDemoContext.h"

// Binds the context to its world; the render/component pointers are filled in later by the demo.
FDemoContext::FDemoContext(UWorld* InWorld)
	: World(InWorld)
{
}