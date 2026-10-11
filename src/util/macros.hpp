#ifndef MACROS_H
#define MACROS_H

// convenient macros to avoid repetive typing (should only be used within main plugin class)
// ... (args) param comes last to support multiple variables in capture list

#define DELAY(delay, body, ...) gameWrapper->SetTimeout([ this, ##__VA_ARGS__ ](GameWrapper * gw) body, delay)

#define GAME_THREAD_EXECUTE(body, ...)                                                                                                     \
	do                                                                                                                                     \
	{                                                                                                                                      \
		gameWrapper->Execute([ this, ##__VA_ARGS__ ](GameWrapper * gw) body);                                                              \
	} while (0)

#define INTERVAL(delaySeconds, numIntervals, code)                                                                                         \
	for (int i = 0; i < numIntervals; i++)                                                                                                 \
	{                                                                                                                                      \
		gameWrapper->SetTimeout([this](GameWrapper* gw) { code }, delaySeconds * i);                                                       \
	}

#define INTERVAL_CAPTURE(delaySeconds, numIntervals, code, ...)                                                                            \
	for (int i = 0; i < numIntervals; i++)                                                                                                 \
	{                                                                                                                                      \
		gameWrapper->SetTimeout([this, __VA_ARGS__](GameWrapper* gw) { code }, delaySeconds * i);                                          \
	}

#endif