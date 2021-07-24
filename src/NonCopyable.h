#pragma once

class NonCopyable
{
protected:
	NonCopyable() = default;
	virtual ~NonCopyable() = default;
private:
	NonCopyable(const NonCopyable&) = delete;
	NonCopyable& operator=(const NonCopyable&) = delete;
};