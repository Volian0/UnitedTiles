#pragma once

class Unique
{
protected:
	Unique() = default;
	virtual ~Unique() = default;
private:
	Unique(Unique const&) = delete;
	Unique& operator=(Unique const&) = delete;
};