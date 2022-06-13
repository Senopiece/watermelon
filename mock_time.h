// just a mock time utilities to replace a missing real RTC
// also it will reset on each boot

long shift = 0;

long timeinsecs() {
  return (millis() / 1000L) + shift;
}

long hour() {
  return (timeinsecs() / 3600L) % 24L;
}

long minute() {
  return (timeinsecs() / 60L) % 60L;
}

long second() {
  return timeinsecs() % 60L;
}

// note that is does not properly implement the desired feauture, so just setting hours, minutes and seconds
void setTime(long h, long m, long s, long w, long t, long f) {
  long expected = h*3600L + m*60L + s;
  long now = (millis() / 1000L) % (60L*60L*24L);
  shift = expected - now;
}
