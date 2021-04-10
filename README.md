# NTP Localtime Server Docker Image

## What does it do?

This simple NTP server delivers time over ntp protocol to dummy devices like ip cameras, cheap routers that cannot handle
time zone changes and day light saving correctly. With this container You can set your dummy device to use UTC time with
no daylight saving and then point it to sychronize time ovet ntp protocol. This container will handle timezone correctly
and deliver local time LTC to dummy device instead of UTC time (like regular ntp server does).


## Usage

Local time zone can be different than docker host by supplying TZ variable:
-e "TZ=Europe/Warsaw"


`docker run -e "TZ=Europe/Warsaw" --name ntpServerLTC -d -p 123:123/udp k4cus/ntpserverltc`

The image exposes NTP port 123/udp. Probably there is ntpd running at docker host, so better add bridge network with custom ip
address to this container to avoid port is use error.


### Configuration
Make sure to set TZ variable to desired timezone.

Find Your time zone at:

https://en.wikipedia.org/wiki/List_of_tz_database_time_zones

e.g. for Poland TZ value is: Europe/Warsaw
