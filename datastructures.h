class TimePoint
{
public:
  byte hours;
  byte minutes;

  TimePoint()
  {
    hours = 0;
    minutes = 0;
  }

  int timestamp()
  {
    return (hours * 60) + minutes;
  }

  bool is_correct()
  {
    return hours < 24 && minutes < 60;
  }

  friend bool operator==(const TimePoint& left, const TimePoint& right)
  {
    return left.hours == right.hours && left.minutes == right.minutes;
  }
};


class TimeInterval
{
public:
  TimePoint from;
  TimePoint to;

  bool is_correct()
  {
    return from.is_correct() && to.is_correct() && to.timestamp() >= from.timestamp();
  }

  friend bool operator==(const TimeInterval& left, const TimeInterval& right)
  {
    return left.from == right.from && left.to == right.to;
  }
};
