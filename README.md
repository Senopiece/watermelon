# watermelon
automatic irrigation on arduino

# quick hardware overview
it needs to be connected with 8 relays on pins 2-9, `DS3231` clock module and `HC-06` bluetooth module

# quick cmd overview
```get time
get shedule
get time
set time 1:00:00
put 1:00 - 2:00 to 1
pull 1:00 - 2:00 from 1
manual mode
(in manual mode) open 1,2,3
(in manual mode) close 1,2,3
exit manual mode


## versioning

backend:
x.x.x~hardware+comment#patch

frontend:
x.x.x~hardware+comment#patch

module:
x.x.x~hardware+comment#patch
