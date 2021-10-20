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
#include <mutex>
#include <condition_variable>

// #define __DEBUG_MESG


class semaphore {
public:
	semaphore()
		: mSignal {false}
                {}

	template<class Rep, class Period>
	bool take(std::chrono::duration<Rep, Period> wait)
                {
                        std::unique_lock<std::mutex> lock {mMutex};
                        bool r {mCondVar.wait_for(lock, wait, [&]{return mSignal;})};
                        mSignal = false;
                        lock.unlock();
                        return r;
                }

	void take()
                {
                        std::unique_lock<std::mutex> lock {mMutex};
                        mCondVar.wait(lock, [&]{return mSignal;});
                        mSignal = false;
                }

	void give()
		{
			{
				std::unique_lock<std::mutex> lock {mMutex};
				mSignal = true;
			}
			mCondVar.notify_one();
		}

private:
	std::mutex mMutex;
	std::condition_variable mCondVar;
	bool mSignal;
};


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
		eInvalid_query,
		eEmpty_data
	};

	int error_code;
};

struct RECORDER {
	RECORDER(std::size_t duration_in_min, std::size_t sampling_per_min, std::size_t shortest_period_min)
		: duration_min {duration_in_min}, sampling_times_min {sampling_per_min}, minimum_judge_time_min {shortest_period_min},
		  recording_idx {0}, head_idx {duration_min - 1}, data_length {0}, data(duration_in_min, sampling_times_min)
		{}

	std::size_t duration_min;
	std::size_t sampling_times_min;
	std::size_t minimum_judge_time_min;
	std::size_t recording_idx; // point to present recording data_t
	std::size_t head_idx;      // point to last complete sampling
	std::size_t data_length;
	std::vector<data_t> data;
	semaphore   sem_sample_arrive;

	/* Range: 1 ~ duration_min
	 
	   ----+--------------------+---> T (minute)
               ^                    ^
          duration_min             Now (1)

	 */
	data_t& open(std::size_t min_th);
	data_t& open_1st();
	data_t& open_last();
	void save(uint16_t fuel_level);
	size_t data_length_min();
	bool new_sample_arrive();
	size_t  start_judging_min();
	int16_t level_change(uint16_t present_level, uint16_t old_level);

	template<class Rep, class Period>
	void get_sampling_interval(std::chrono::duration<Rep,Period> &r)
		{
			r = std::chrono::duration_cast<std::chrono::duration<Rep,Period>>(std::chrono::seconds {1}) / sampling_times_min;
		}
};

data_t&
RECORDER::open(std::size_t min_th)
{
	if(min_th == 0 || min_th > data_length || min_th > duration_min)
		throw recorder_error_t {recorder_error_t::eInvalid_query};
	
	if(data_length == 0)
		throw recorder_error_t {recorder_error_t::eEmpty_data};

	size_t h {head_idx};
	min_th--;
	if(min_th > head_idx) {
		min_th -= h;
		h = duration_min - min_th;
	}
	else {
		h -= min_th;
	}

	try {
#ifdef __DEBUG_MESG
		std::cerr << "recording idx: " << recording_idx;
		std::cerr << "\thead idx: " << h << "\t";
#endif
		return data.at(h);
	}
	catch (std::out_of_range const &r) {
		std::cerr << r.what() << std::endl;
		throw r;
	}
}

data_t&
RECORDER::open_1st()
{
	return open(1);
}

data_t&
RECORDER::open_last()
{
	return open(data_length);
}

void
RECORDER::save(uint16_t lvl)
{
	data_t &min {data.at(recording_idx)};
	min.feed_sample(lvl);

	if(min.freshed())
	{
		// Update recording index
		recording_idx++;
		recording_idx %= duration_min;

		head_idx++;
		head_idx %= duration_min;

		// Update data length
		if(data_length < duration_min)
			data_length++;

		sem_sample_arrive.give();
	}
}

size_t
RECORDER::data_length_min()
{
	return data_length;
}

bool
RECORDER::new_sample_arrive()
{
	return sem_sample_arrive.take(std::chrono::seconds {0});
}

size_t
RECORDER::start_judging_min()
{
	return minimum_judge_time_min;
}

int16_t
RECORDER::level_change(uint16_t present_level, uint16_t old_level)
{
	return present_level - old_level;
}

int
main(int argc, char* argv[])
{
	constexpr size_t _60_minutes {60};
	constexpr size_t sampling_time {10};
	constexpr size_t start_judge_min {1};
	RECORDER fuelevel {_60_minutes, sampling_time, start_judge_min};

	std::chrono::milliseconds ms;
	fuelevel.get_sampling_interval(ms);
	std::cerr << "Sampling interval: " << ms.count() << "ms\n";
	try {
		for(uint16_t l=0; l<729; l++) {
			fuelevel.save(l%121);
			// std::cerr << "Recorder length: " << fuelevel.length();
			// std::cerr << "\tHead idx: " << fuelevel.head_idx << std::endl;
			if(fuelevel.new_sample_arrive()) {
				std::cerr << "New sample arrive\n";
				try {
					if(fuelevel.data_length_min() >= fuelevel.start_judging_min()) {
						std::cerr << "1st min FL: " << fuelevel.open(1).average << std::endl;
						std::cerr << "Last min FL: " << fuelevel.open(fuelevel.data_length_min()).average << std::endl;
						std::cerr << "1st min FL: " << fuelevel.open_1st().average << std::endl;
						std::cerr << "Last min FL: " << fuelevel.open_last().average << std::endl;
						std::cerr << "Level change: " << fuelevel.level_change(fuelevel.open_1st().average, fuelevel.open_last().average) << std::endl;
					}
				}
				catch (recorder_error_t &err) {
					std::cerr << "Got error\n";
				}
			}
		}
		std::cerr << "---------------------------------------------\n";
		for(size_t i=1; i<=60; i++) {
			if(i == 60)
				std::cerr << i << "th min\t" << "\taverage: " << fuelevel.open(i).average << std::endl;
		}

		std::cerr << "\n1st min: " << fuelevel.open(1).average << std::endl;
		std::cerr << "60th min: " << fuelevel.open(60).average << std::endl;
	}
	catch (...) {
		std::cerr << "Got exception\n"; 
	}
}
