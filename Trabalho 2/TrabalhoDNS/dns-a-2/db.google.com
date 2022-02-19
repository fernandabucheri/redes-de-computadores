$TTL	60000
@	IN	SOA	dns.google.com.	admin.dns.google.com. (
	2021121901	; serial
	30		; refresh
	15		; retry
	60000		; expire
	0		; negative cache ttl
	)
@		IN	NS	dns.google.com.
dns.google.com.	IN	A	10.0.10.10
web2		IN	A	10.0.10.11
www		IN	CNAME	web2
