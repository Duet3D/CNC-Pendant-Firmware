/*
 * CRC16.h
 *
 *  Created on: 4 Dec 2020
 *      Author: David
 *
 * This calculates CRC16 CCIT with initial CRC value Zero.
 */

#ifndef CRC16_H_
#define CRC16_H_

#include <Arduino.h>

class CRC16
{
private:
	uint16_t crc;

public:
	CRC16() noexcept;

	void Update(char c) noexcept;
	void Update(const char *c, size_t len) noexcept;
	void Reset(uint16_t initialValue) noexcept;
	uint16_t Get() const noexcept;
};

inline uint16_t CRC16::Get() const noexcept
{
	return crc;
}

inline void CRC16::Reset(uint16_t initialValue) noexcept
{
	crc = initialValue;
}

#endif /* CRC16_H_ */
