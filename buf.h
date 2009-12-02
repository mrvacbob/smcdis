/*
 *  buf.h
 *  smcdis
 *
 *  Created by Alexander Strange on 12/20/07.
 *
 */

#ifndef __BUF_H__
#define __BUF_H__

#include <stdint.h>
#include <unistd.h>
#include <string.h>

#ifdef __BIG_ENDIAN__
#define NATIVE_ENDIAN BIG_ENDIAN
#else
#define NATIVE_ENDIAN LITTLE_ENDIAN
#endif

#ifndef BIT_ENDIAN
#define BIT_ENDIAN BIG_ENDIAN
#endif

template<typename T> T bswap(T in) {
    T v = in;
	if (sizeof(T) == 2) return ((v << 8) & 0xff00) | ((v >> 8) & 0xff);
	else if (sizeof(T) == 4) return (v >> 24) | ((v >> 8) & 0xff00) | ((v << 8) & 0xff0000) | (v << 24);
	return v;
}

class mapped_file
{
public:
	mapped_file(const char *file);
	~mapped_file();
	off_t size() {return fsize;}
	uint8_t *request(off_t off, size_t size) {return data + off;}
	
private:
	bool mmapped;
	off_t fsize;
	uint8_t *data;
};

class stream_reader
{
protected:
	void assert_bytes(size_t s) {}
	
	typedef enum endian_t {
		BIG_ENDIAN = 0,
		LITTLE_ENDIAN
	} endian_t;
	
	typedef size_t fsize_t; // no 64-bit seeking
	
	template<typename T> inline T _peek_bytes(endian_t endian)
	{
		assert_bytes(sizeof(T));
		T ret = *reinterpret_cast<T*>(cur());
		if (endian != NATIVE_ENDIAN) ret = bswap<T>(ret);
		return ret;
	}
	
	template<typename T> inline T _peek_bits_typed(unsigned bits_wanted)
	{
		T v = _peek_bytes<T>(NATIVE_ENDIAN);
		v <<= bit_off;
		v >>= (sizeof(T)*8) - bits_wanted;
		return v;
	}
	
	size_t buf_size;
	uint8_t *buf;
	size_t at;
	unsigned char bit_off;
public:
	stream_reader(mapped_file &rf) {buf_size = rf.size(); buf = rf.request(0, buf_size); at = bit_off = 0;}
	~stream_reader() {}
	size_t size() {return buf_size;}
	void seek(fsize_t n) {at = n; bit_off=0;}
	void skip(size_t s) {at += s;}
	uint8_t *cur() {return buf + at;}
	void skip_bits(size_t s) {unsigned tmp = bit_off+s; skip(tmp/8); bit_off = tmp % 8;}
	size_t pos() {return at;}
	
#define ReadPeekFunc(name, type, param, endian) \
	type peek_##name(param) {return _peek_bytes<type>(endian);} \
	type read_##name(param) {type ret = _peek_bytes<type>(endian); skip(sizeof(type)); return ret;}
	
#define ByteReadFunc(name, type) \
	ReadPeekFunc(name, type, endian_t endian = NATIVE_ENDIAN, endian); \
	ReadPeekFunc(name##le, type, , LITTLE_ENDIAN); \
	ReadPeekFunc(name##be, type, , BIG_ENDIAN);

	ByteReadFunc(s8, int8_t);
	ByteReadFunc(u8, uint8_t);
	
	ByteReadFunc(s16, int16_t);
	ByteReadFunc(u16, uint16_t);
	
	ByteReadFunc(s32, int32_t);
	ByteReadFunc(u32, uint32_t);
	
	ByteReadFunc(s64, int64_t);
	ByteReadFunc(u64, uint64_t);
	
	void peek_bytes(void *dest, size_t s) {
		assert_bytes(s);
		memcpy(dest, cur(), s);
	}
	
	void read_bytes(void *dest, size_t s) {
		peek_bytes(dest, s);
		skip(s);
	}
	
	inline uint16_t read_bits(unsigned bits) {
		uint16_t ret;
		
		if (__builtin_constant_p(bits)) {
			if (bits <= 8) ret = _peek_bits_typed<uint16_t>(bits);
			else ret = _peek_bits_typed<uint32_t>(bits);
		} else ret = _peek_bits_typed<uint32_t>(bits);
		
		skip_bits(bits);
		return ret;
	}

	inline int16_t read_sbits(unsigned bits) {
		int16_t ret;
		
		if (__builtin_constant_p(bits)) {
			if (bits <= 8) ret = _peek_bits_typed<int16_t>(bits);
			else ret = _peek_bits_typed<int32_t>(bits);
		} else ret = _peek_bits_typed<int32_t>(bits);
		
		skip_bits(bits);
		return ret;
	}
};

#undef ByteReadFunc
#undef ReadPeekFunc
#endif