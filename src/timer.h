#ifndef TIMER_H_
#define TIMER_H_

unsigned long get_time_msec(void);
void sleep_msec(unsigned long msec);

double get_time_sec(void);
void sleep_sec(double sec);


class Timer {
private:
	unsigned long start_time, pause_time;

public:
	Timer();

	void reset();

	void start();
	void stop();

	bool is_running() const;

	unsigned long get_msec() const;
	double get_sec() const;
};

#endif	// TIMER_H_
