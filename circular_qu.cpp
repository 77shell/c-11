/*****************************************************************
 *  Copyright (c) 2021 Delta Products
 *
 *  THIS IS UNPUBLISHED PROPRIETARY TRADE SECRET SOURCE CODE OF
 *  Delta Products
 *
 *  The copyright above and this notice must be preserved in all copies of this
 *  source code.  The copyright notice above does not evidence any actual or
 *  intended publication of such source code.  This source code may not be 
 *  copied, disclosed, distributed, demonstrated or licensed except as expressly
 *  authorized by Delta Products.
 *************************************************************/
#include <cstdint>
#include <stdexcept>
#include <iostream>
#include <vector>
#include <iterator>
#include <numeric>
#include <algorithm>
#include <chrono>

#define __DEBUG_MESG

struct data_t {
	data_t(size_t sampling_time)
		: sample_nbr {sampling_time}, sample_vec(sampling_time), sample_ite {sample_vec.begin()}, average {0}
		{}

	data_t(const data_t &r)
		: sample_nbr {r.sample_nbr}, sample_vec {r.sample_vec}, sample_ite {sample_vec.begin()}, average {0}
		{}

	data_t(data_t &&r)
		: sample_nbr {r.sample_nbr}, sample_vec {std::move(r.sample_vec)}, sample_ite {sample_vec.begin()}, average {0}
		{}

	std::size_t sample_nbr;
	std::vector<uint16_t> sample_vec;
	std::vector<uint16_t>::iterator sample_ite;
	uint16_t average;

	void feed_sample(uint16_t fuel_level);
	bool freshed();
};

void
data_t::feed_sample(uint16_t fuel_level)
{
	constexpr uint16_t max_level {120};
	if(fuel_level > max_level)
		return;

	if(sample_ite == sample_vec.end()) {
		sample_ite = sample_vec.begin();
		*sample_ite = fuel_level;
		sample_ite++;
	}
	else {
		*sample_ite = fuel_level;
		sample_ite++;
	}

	if(sample_ite == sample_vec.end()) {
		uint16_t sum = std::accumulate(sample_vec.begin(), sample_vec.end(), 0);
		average = sum / sample_nbr;
#ifdef __DEBUG_MESG
		std::for_each(sample_vec.begin(), sample_vec.end(), [](uint16_t level) {
									    std::cerr << "level: " << level << std::endl;
								    }
			);
		std::cerr << "average level: " << average << std::endl;
#endif
	}

}

bool
data_t::freshed()
{
	return sample_ite == sample_vec.end();
}

struct recorder_error_t {
	recorder_error_t(int code)
		: error_code {code}
		{}

	enum {
		eInvalid_query
	};

	int error_code;
};

struct RECORDER {
	RECORDER(std::size_t min, std::size_t sampling_per_min)
		: interval_min {min}, sampling_times_min {sampling_per_min}, head_idx {0}, data_length {0}, data(min, sampling_times_min)
		{}

	std::size_t interval_min;
	std::size_t sampling_times_min;
	std::size_t head_idx;
	std::size_t data_length;
	std::vector<data_t> data;

	/* Range: 1 ~ interval_min
	 
	   ----+--------------------+---> T (minute)
               ^                    ^
          interval_min             Now (1)

	 */
	data_t& open(std::size_t min_th);
	void save(uint16_t fuel_level);
	size_t length();

	template<class Rep, class Period>
	void sampling_interval(std::chrono::duration<Rep, Period> &r)
		{
			r = std::chrono::duration_cast<std::chrono::duration<Rep,Period>>(std::chrono::seconds {1}) / sampling_times_min;
		}
};

data_t&
RECORDER::open(std::size_t min_th)
{
	if(min_th == 0 || min_th > interval_min)
		throw recorder_error_t {recorder_error_t::eInvalid_query};

	size_t h {head_idx};
	min_th--;
	if(min_th > head_idx) {
		min_th -= h;
		h = interval_min - min_th;
	}
	else {
		h -= min_th;
	}

	try {
		return data.at(h);
	}
	catch (std::out_of_range const &r) {
		std::cerr << r.what() << std::endl;
		throw r;
	}
}

void
RECORDER::save(uint16_t lvl)
{
	data_t &min {data.at(head_idx)};
	min.feed_sample(lvl);

	if(min.freshed())
	{
		// Update header
		head_idx++;
		head_idx %= interval_min;

		// Update data length
		if(data_length < interval_min)
			data_length++;
	}
}

size_t
RECORDER::length()
{
	return data_length;
}

int
main(int argc, char* argv[])
{
	constexpr size_t _60_minutes {60};
	constexpr size_t sampling_time {3};
	RECORDER fuelevel {_60_minutes, sampling_time};

	std::chrono::milliseconds ms;
	fuelevel.sampling_interval(ms);
	std::cerr << "Sampling interval: " << ms.count() << "ms\n";
	try {
		for(uint16_t l=0; l<660; l++) {
			fuelevel.save(l%121);
			std::cerr << "Recorder length: " << fuelevel.length();
			std::cerr << "\tHead idx: " << fuelevel.head_idx << std::endl;
			if(fuelevel.head_idx == 59)
				std::cerr << "here\n";
		}
	}
	catch (...) {
		std::cerr << "Got exception\n"; 
	}
}


#if 0
NONvolatile::NONvolatile(SECTOR &s, u16 maxNo)
   : RECORDER(maxNo), mEeRegionAddr(s.start())
{}


void
NONvolatile::init()
{
   if( !Eeprom.isOK() ) return;
   uc16
      headerAddr(msHeader_Addr + mEeRegionAddr),
      lengthAddr(msLength_Addr + mEeRegionAddr);
   
   Eeprom.read(mHeader, headerAddr);
   Eeprom.read(mLength, lengthAddr);
   if(mHeader > maxNo()) {
      __Warning("NONvolatile: reset header");
      mHeader = 0;
      Eeprom.write(mHeader, headerAddr);
   }
   if(mLength > maxNo()) {
      __Warning("NONvolatile: reset length");
      mLength = 0;
      Eeprom.write(mLength, lengthAddr);
   }
   
   if(mLength < maxNo() && mLength != mHeader) {
      if(mLength > mHeader) {
         mHeader = mLength;
         Eeprom.write(mHeader, headerAddr);
      }
      else {
         mLength = mHeader;
         Eeprom.write(mLength, lengthAddr);
      }
   }
   
   mStartAddr = mEeRegionAddr + msLog_Addr;
   RECORDER::init();
}


void
NONvolatile::reset()
{
   RECORDER::reset();
   saveHeader();
   saveLength();
}


u16
NONvolatile::_getNextAddr()
{
   u16 header(mHeader);
   if(++header == maxNo()) header = 0;
   return header * msSizeOfLog + mStartAddr;
}


u16
NONvolatile::_getAddr(u16 serialNo)
{
   u16 header(mHeader);
   --serialNo;
   if(serialNo > header) {
      serialNo -= header;
      header = maxNo() - serialNo;
   }
   else {
      header -= serialNo;
   }
   return header * msSizeOfLog + mStartAddr;
}


bool
NONvolatile::save(u16 id, bool saveLenHead)
{
   if( !Eeprom.isOK() ) return false;
   if(take() == false) { mSemErrW++; return false; }
   Time t(System.time());
   uc16 addr(_getNextAddr());
   if(   !Eeprom.write(id, addr)
      || !Eeprom.write(t, addr + sizeof(u16)) ) {
         give();
         return false;
   }
   updateHeader();
   updateLength();
   if(saveLenHead) saveHeader_Length();
   give();
   return true;
}


/****************************************************************
 * Function      :  open
 * Description   :  To get a log.
 *                  
 * Input         :  serial No. of log.
 *                  1: the latest one
 *
 * Output        :  true, grab successful
 *                  false, grab failed
 *
 * History       :  ysh 3-31-2010     created.
 ****************************************************************/
bool
NONvolatile::open(u16 serialNo, LOG &log)
{
   if( !Eeprom.isOK() ) return false;
   if(serialNo == 0 || serialNo > mLength) return false;
   if(take() == false) { mSemErrR++; return false; }
   uc16 addr(_getAddr(serialNo));
   bool 
      r1(Eeprom.read(log.mId, addr)),
      r2(Eeprom.read(log.mTime, addr + sizeof(u16)));
   give();
   return r1 && r2;
}


void
NONvolatile::openByHeader(u16 h, LOG &log)
{
   if( !Eeprom.isOK() ) return;
   if(h >= maxNo()) {
      __Errmsg("Illegal header");
      return;
   }
   if(take() == false) return;
   uc16 addr(h * msSizeOfLog + mStartAddr);
   if(   !Eeprom.read(log.mId, addr) 
      || !Eeprom.read(log.mTime, addr + sizeof(u16)) ){
         __Errmsg("Open log");
      }
   give();
}


void
NONvolatile::saveHeader()
{
   for(int retry=10; retry>0; retry--)
      if(Eeprom.write(mHeader, msHeader_Addr + mEeRegionAddr)) return;
}


void
NONvolatile::saveLength()
{
   for(int retry=10; retry>0; retry--)
      if(Eeprom.write(mLength, msLength_Addr + mEeRegionAddr)) return;
}


void
NONvolatile::saveHeader_Length()
{
   uc32 headLeng(mHeader + (mLength << 16));

   for(int retry=10; retry>0; retry--)
      if(Eeprom.write(headLeng, msHeader_Addr + mEeRegionAddr)) return;
}


void
NONvolatile::_testRW()
{
   LOG log;
   for(u16 h=0; h<maxNo(); h++)
   {
      mHeader = h;
      for(int i=1; i<=maxNo(); i++) {
         if(!save(i)) puts("Log test save err\n\r");
      }
      int j(maxNo());
      for(int i=1; i<=maxNo(); i++, j--) {
         if(!open(i, log)) puts("Log test open err\n\r");
         if(j != log.id()) printf("Logging error: %d\n\r", i);
      }
      printf("Log test round: %d\n\r", h);
   }
   puts("Log test done!");
}


void
NONvolatile::_testPowerLost()
{
   printf("Header: %d Length: %d\n\r", mHeader, mLength);
   save(0x20);
}


void
NONvolatile::test()
{
   _testRW();
   //_testPowerLost();
}
#endif
