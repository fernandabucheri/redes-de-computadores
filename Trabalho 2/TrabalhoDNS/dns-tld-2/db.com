$TTL	60000
@	IN	SOA	dns.com.	admin.dns.com. (
	2021121901	; serial
	30		; refresh
	15		; retry
	60000		; expire
	0		; negative cache ttl
	)
@		IN	NS	dns.com.
dns.com.	IN	A	10.0.4.10

google.com.	IN	NS	dns.google.com.
dns.google.com.	IN	A	10.0.10.10
