
#include "stdafx.h"
#include "Horloge.h"

namespace PM3D
{

Horloge::Horloge()
{
	LARGE_INTEGER counterFrequency;
	::QueryPerformanceFrequency(&counterFrequency);
	m_SecPerCount = 1.0 / static_cast<double>(counterFrequency.QuadPart);
}

int64_t Horloge::GetTimeCount() const
{
	LARGE_INTEGER countNumber;
	::QueryPerformanceCounter(&countNumber);
	return countNumber.QuadPart;
}

double Horloge::GetTimeBetweenCounts(int64_t start, int64_t stop) const
{
	return static_cast<double>(stop - start) * m_SecPerCount;
}

} // namespace PM3D
