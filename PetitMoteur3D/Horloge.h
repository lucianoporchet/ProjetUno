#pragma once

namespace PM3D
{

class Horloge
{
public:
	Horloge();

	int64_t GetTimeCount() const;
	double GetSecPerCount() const { return m_SecPerCount; }
	// retourne le temps en millisecondes entre deux count.
	double GetTimeBetweenCounts(int64_t start, int64_t stop) const;

private:
	double m_SecPerCount;
};

} // namespace PM3D
