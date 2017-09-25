#
# Regular cron jobs for the qtango package
#
0 4	* * *	root	[ -x /usr/bin/qtango_maintenance ] && /usr/bin/qtango_maintenance
