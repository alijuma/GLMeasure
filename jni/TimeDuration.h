#ifndef TIMEDURATION_H_
#define TIMEDURATION_H_

#include <time.h>

// Simplified versions of mozilla::TimeStamp and mozilla:TimeDuration.

class Duration {
public:
  Duration(const Duration& aDuration)
  {
    mMilliseconds = aDuration.mMilliseconds;
  }

  Duration& operator=(const Duration& aDuration)
  {
    mMilliseconds = aDuration.mMilliseconds;
    return *this;
  }

  double ToMilliseconds()
  {
    return mMilliseconds;
  }

  friend class Time;
private:
  Duration(double aMilliseconds) : mMilliseconds(aMilliseconds) {}

  double mMilliseconds;
};

class Time {
public:
  Time() {}

  Time(const Time& aTime)
  {
    mTimespec = aTime.mTimespec;
  }

  static Time Now()
  {
    Time t;
    clock_gettime(CLOCK_MONOTONIC, &t.mTimespec);
    return t;
  }

  Duration operator-(const Time& aTime)
  {
    double difference = (mTimespec.tv_sec - aTime.mTimespec.tv_sec)*1000 +
                        (mTimespec.tv_nsec - aTime.mTimespec.tv_nsec)/1000000;
    return Duration(difference);
  }

private:
  struct timespec mTimespec;
};


#endif /* TIMEDURATION_H_ */
