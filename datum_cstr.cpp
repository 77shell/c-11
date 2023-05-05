#include <iostream>
#include <cstring>
using namespace std;

struct cstr {
	cstr();
	cstr(const cstr &);
	cstr(const char *str);
	operator const char*() const { return s; }
	const char* get() const { return s; };
	bool operator==(const cstr &);
	bool operator!=(const cstr &);
	bool operator>(const cstr &);
	bool operator<(const cstr &);
	cstr& operator=(const char *);
	cstr& operator=(const cstr &);
	char& operator[](const size_t idx);

	static constexpr size_t len {63};
	static constexpr size_t bufsize {len + 1};

private:
	char s[bufsize] __attribute__((aligned(__BIGGEST_ALIGNMENT__)));/* null character */
};

template <class Type>
class DEFAULTval {
public:
	DEFAULTval(Type up, Type val, Type low)
		: mUpper {up}, mValue {val}, mLower {low}
		{}

	int size() { return sizeof(Type) * 3; }

	Type mUpper;
	Type mValue;
	Type mLower;
};

template <>
DEFAULTval<cstr>::DEFAULTval(cstr up, cstr val, cstr low)
{
}

cstr::cstr()
{
	std::memset(s, ' ', len);
	s[len] = '\0';
}

cstr::cstr(const cstr &r)
{
	std::memcpy(s, r.s, bufsize);
}

cstr::cstr(const char *str)
{
	std::memset(s, 0, bufsize);
        *this = str;
}

bool
cstr::operator==(const cstr &rhs)
{
        return strcmp(s, rhs.s) == 0;
}

bool
cstr::operator!=(const cstr &rhs)
{
        return strcmp(s, rhs.s) != 0;
}

bool
cstr::operator>(const cstr &rhs)
{
        return strlen(s) > strlen(rhs.s);
}

bool
cstr::operator<(const cstr &rhs)
{
        return strlen(s) < strlen(rhs.s);
}

cstr&
cstr::operator=(const char *str)
{
        const size_t str_len {strlen(str)};
        if (str_len <= len) {
                strcpy(s, str);
	}
        else {
                strncpy(s, str, len);
                s[len] = '\0';
        }
        return *this;
}

cstr&
cstr::operator=(const cstr &src)
{
	std::memcpy(s, src.s, bufsize);
        return *this;
}

char&
cstr::operator[](const size_t idx)
{
	if (idx >= len) {
		std::string msg {"Access " + std::to_string(idx) + " exceeds maximum data length 63-bytes"};
		throw std::length_error {msg};
	}
	return s[idx];
}

using u16 = uint16_t;

template<class Type>
class DATUM {
public:
	DATUM(u16 grp, u16 idx, Type def_upper, Type def_val, Type def_lower);
	operator Type() { return mValue; }

	DEFAULTval<Type> mDefVal;
	Type mUpper;
	Type mValue;
	Type mLower;
};

template <class Type>
DATUM<Type>::DATUM(u16 grp, u16 idx, Type def_upper, Type def_val, Type def_lower)
	: mDefVal {def_upper, def_val, def_lower}, mUpper {def_upper}, mValue {def_val}, mLower {def_lower}
{}


class NETWORKdata {
public:
	NETWORKdata();

	class SNMP {
	public:
		SNMP();
		DATUM<cstr> readCommunity;
		DATUM<cstr> writeCommunity;
	} snmp;

	DATUM<u16> adminPW;
	DATUM<u16> userPW;
};

NETWORKdata::NETWORKdata()
        :
        snmp {dbm},
        adminPW      {GroupNbr, 19, 9999, 8888, 0, dbm, "WEB admin pw"},
        userPW       {GroupNbr, 20, 9999, 1234, 0, dbm, "WEB user pw"}
{
        mMembers.push_back(&snmp.readCommunity);
        mMembers.push_back(&snmp.writeCommunity);
}

NETWORKdata::SNMP::SNMP(SETPOINTdbm &dbm)
        :
        readCommunity   {GroupNbr, 1, cstr {""}, cstr {"Read community"}, cstr {""}},
        writeCommunity  {GroupNbr, 2, cstr {""}, cstr {"Write community"}, cstr {""}}
{}


int
main(int argc, char *argv[])
{
	DATUM<cstr> datum {1, 1, cstr {""}, cstr {"Read community"}, cstr {""}};
	cstr c {static_cast<cstr>(datum)};
	cout << c.get() << endl;
}
