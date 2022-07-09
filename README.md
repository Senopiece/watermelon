# watermelon
`mock` automatic irrigation on arduino

# quick hardware overview
it needs only `HC-06` bluetooth module as it's just a mock prototype.

# quick cmd overview
- get shedule > `1: 12:00 - 13:00;2: 12:00 - 13:00`
- get time > `12:00:13`
- set time to 1:00:00 > `ok`
- put 1:00 - 2:00 to 1 > `ok`
- pull 1:00 - 2:00 from 1 > `ok`
- manual mode > `ok`
- (in manual mode) open 1,2,3 > `recognized 1, 2, 3`
- (in manual mode) close 1,2,3 > `recognized 1, 2, 3`
- exit manual mode > `ok`
- get version > `aleph-x.x.x`
